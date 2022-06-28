#pragma once

#include <memory>
#include <type_traits>

template <typename T>
class PoolPtr;


// This is an object pool for use with frequently allocated/deallocated objects.
// For use when pool size can't be estimated easily or restricted at compile-time.
template <typename T>
class DynamicPool
{
	// Assert that T is not abstract.
	static_assert(std::negation_v<std::is_abstract<T>>,
		"DynamicPool: Pool object cannot be abstract.");

	// Assert that T has a default constructor to be called if the pool resizes.
	static_assert(std::is_default_constructible_v<T>,
		"DynamicPool: Pool object must have a default constructor.");

	// The current list of objects.
	// Memory acquired by unique_ptrs is not released until pool destruction.
	std::vector<std::unique_ptr<T>> objects;

	// List of pointers to available objects.
	std::vector<T*> available;

	// Expands the object pool.
	void expand();

	// Expands the object pool from the previous size to the new size.
	void expand(int prev_size, int new_size);

public:

	// Initializes the pool to hold the given number of objects.
	DynamicPool<T>(int start_capacity);

	// Constructs the object of type T with the given args somewhere in the pool, expanding if the pool has reached maximum capacity.
	// Returns an owning pointer to the created T object.
	template <typename... ArgTypes>
	PoolPtr<T> get(ArgTypes&&... args)
	{
		// Assert that T has a constructor that takes the given arg types.
		static_assert(std::is_constructible_v<T, ArgTypes&&...>,
			"DynamicPool: Pool object does not have a constructor that takes the given argument types as its parameters.");

		// Expand the pool if no memory is available.
		if (available.empty()) {
			expand();
		}

		// Pop the most recently freed object.
		T& object = *available.back();
		available.pop_back();

		// Construct the object with the given arguments.
		object = { std::forward<ArgTypes>(args)... };

		// Return a pointer to the object, encapsulated in an owning pointer.
		return PoolPtr<T>(&object, *this);
	}

	// Frees the object back to the pool to be reused.
	void free(T* ptr); // may make private friend of pool ptr.

};

template<typename T>
inline void DynamicPool<T>::expand()
{
	// No available pool indices left. available.size() == 0.
	int prev_size = available.capacity();
	int new_size = 2 * prev_size;
	expand(prev_size, new_size);
}

template<typename T>
inline void DynamicPool<T>::expand(int prev_size, int new_size)
{
	int added = new_size - prev_size;

	objects.resize(new_size);
	available.reserve(new_size);
	available.resize(added);

	for (int i = prev_size; i < new_size; ++i) {
		objects[i] = std::make_unique<T>();
	}

	for (int i = 0; i < added; ++i) {
		// insert backwards ( new_size - 1 -> prev size), since we pop off back.
		available[i] = objects[new_size - 1 - i].get();
	}

}

template<typename T>
inline void DynamicPool<T>::free(T* ptr)
{
	available.push_back(ptr);
}

template<typename T>
inline DynamicPool<T>::DynamicPool(int start_capacity)
{
	expand(0, start_capacity);
}


// An owning pointer to an object in a DynamicPool.
// On destruction or release, frees the object back to the pool.
template <typename T>
class PoolPtr
{
	// Could use unique_ptr<T, PoolDeleter> instead.
	
	// A pointer to an object in the pool or nullptr.
	T* object = nullptr;

	// The pool that object will be in.
	DynamicPool<T>& pool;

	// Give private ctor access to object pool.
	friend class DynamicPool<T>;

	// Constructs an owning pointer to an object in the given pool.
	PoolPtr<T>(T* object, DynamicPool<T>& pool);

	bool is_null() const;

public:


	PoolPtr<T>() = delete;

	// Construct a null PoolPtr, setting its target pool.
	PoolPtr<T>(DynamicPool<T>& pool);

	PoolPtr<T>(const PoolPtr<T>& copy) = delete;

	// A noexcept move ctor for moving ownership
	// and DynamicPool resizing (T may own PoolPtr<T>s).
	PoolPtr<T>(PoolPtr<T>&& move) noexcept;

	// On destruction, if object not nullptr, frees its object back to the pool.
	~PoolPtr<T>();

	// Returns a pointer to the object owned by this PoolPtr.
	T* get() const;

	// If object not nullptr, frees object back to the pool.
	// Sets this PoolPtr's object to nullptr.
	void reset();

	PoolPtr<T>& operator =(const PoolPtr<T>& copy) = delete;
	PoolPtr<T>& operator =(PoolPtr<T>&& move) noexcept;

	// Resets the PoolPtr.
	bool operator =(std::nullptr_t null_val);

	// Returns true if object is nullptr, else false.
	bool operator ==(std::nullptr_t null_val) const;

	// Returns a reference to the underlying object owned by this PoolPtr.
	T& operator *() const;

	// Returns the pointer to the object owned by this PoolPtr.
	T* operator ->() const;


};

template<typename T>
inline PoolPtr<T>::PoolPtr(T* object, DynamicPool<T>& pool) : object(object), pool(pool)
{}

template<typename T>
inline PoolPtr<T>::PoolPtr(DynamicPool<T>& pool) : object(nullptr), pool(pool)
{}

template<typename T>
inline PoolPtr<T>::PoolPtr(PoolPtr<T>&& move) noexcept : object(move.object), pool(move.pool)
{
	move.object = nullptr;
}

template<typename T>
inline PoolPtr<T>::~PoolPtr()
{
	reset();
}

template<typename T>
inline bool PoolPtr<T>::is_null() const
{
	return object == nullptr;
}

template<typename T>
inline T* PoolPtr<T>::get() const
{
	return object;
}

template<typename T>
inline void PoolPtr<T>::reset()
{
	if (!is_null()) {
		object->~T(); // call object's destructor.
		pool.free(object);
		object = nullptr;
	}
}

template<typename T>
inline PoolPtr<T>& PoolPtr<T>::operator=(PoolPtr<T>&& move) noexcept
{
	reset();
	
	// this->object is nullptr

	object = move.object;
	move.object = nullptr;

	// if move.object == this->object, object will equal nullptr.
	// This should not be possible unless user assigns a PoolPtr to itself, since
	// functional PoolPtrs are only returned by private ctor used by DynamicPool::get,
	// which only returns unique objects.
	return *this;
}

template<typename T>
inline bool PoolPtr<T>::operator=(std::nullptr_t null_val)
{
	reset();
}

template<typename T>
inline bool PoolPtr<T>::operator==(std::nullptr_t null_val) const
{
	return is_null();
}

template<typename T>
inline T& PoolPtr<T>::operator*() const
{
	return *object;
}

template<typename T>
inline T* PoolPtr<T>::operator->() const
{
	return object;
}
