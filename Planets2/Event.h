#pragma once
#include <functional>
#include <limits>
#include <utility>
#include <algorithm>

template <class ParamType>
class EventHandle;

/* 
* Represents an observable event.
* Methods can be subscribed to this event.
* When the event is triggered (by notify_all),
* subscribed methods are called with a message of ParamType. 
*/
template <class ParamType>
class Event {

	int next_id = std::numeric_limits<int>::min();
	std::vector<std::pair<int, std::function<void(const ParamType&)>>> observers;

public:

	// Adds observer callback and returns its id.
	// Returns an RAII handle that will remove itself from the pool when destructed.
	[[nodiscard]]
	EventHandle<ParamType> add_observer(std::function<void(const ParamType&)> observer);

	// Removes the observer callback associated with the given id.
	void rem_observer(int id);

	// Calls all callback methods with the given message parameter.
	void notify_all(const ParamType& message);

};


template <class ParamType>
class EventHandle
{

	Event<ParamType>* observer_pool = nullptr;
	int id = 0; // only valid when observer_pool is not null.

public:

	EventHandle() = default;

	EventHandle(int id, Event<ParamType>& observer_pool);

	EventHandle(const EventHandle& other) = delete;
	EventHandle& operator=(const EventHandle& other) = delete;

	EventHandle(EventHandle&& other);
	EventHandle& operator=(EventHandle&& other);
	~EventHandle();

	void detach();
};

template<class ParamType>
inline EventHandle<ParamType> Event<ParamType>::add_observer(std::function<void(const ParamType&)> observer)
{
	int ret_id = next_id++;
	observers.emplace_back(ret_id, observer);
	return { ret_id, *this };
}

template<class ParamType>
inline void Event<ParamType>::rem_observer(int id)
{
	// Could use lower_bound bin search, or even a map.
	// But expect number of observers to be pretty low.
	auto it = std::find_if(observers.begin(), observers.end(), [id](const auto& pair)
	{
		return pair.first == id;
	});

	observers.erase(it);
}

template<class ParamType>
inline void Event<ParamType>::notify_all(const ParamType& message)
{
	for (auto& pair : observers)
	{
		auto& observer = pair.second;
		observer(message);
	}
}




template<class ParamType>
inline EventHandle<ParamType>:: EventHandle(int id, Event<ParamType>& observer_pool)
	: id(id), observer_pool(&observer_pool)
{}


template<class ParamType>
inline EventHandle<ParamType>::EventHandle(EventHandle<ParamType>&& other)
: id(other.id), observer_pool(other.observer_pool)
{
	other.observer_pool = nullptr;
}

template<class ParamType>
inline EventHandle<ParamType>& EventHandle<ParamType>::operator=(EventHandle<ParamType>&& other)
{
	detach();
	id = other.id;
	observer_pool = other.observer_pool;

	other.observer_pool = nullptr;
	return *this;
}

template<class ParamType>
inline void EventHandle<ParamType>::detach()
{
	if (observer_pool)
	{
		observer_pool->rem_observer(id);
		observer_pool = nullptr;
	}
}

template<class ParamType>
inline EventHandle<ParamType>::~EventHandle()
{
	detach();
}
