#pragma once

#include <array>
#include <vector>
#include <functional>
#include <type_traits>
#include <span>

// Represents the 4 child nodes of a node in a quadtree.
template <typename T>
class QuadChildren
{
	// Assert T has a default ctor.
	static_assert(std::is_default_constructible_v<T>,
		"QuadChildren: node type must have a default constructor.");

	std::array<T, 4> children;

	// Constructs and returns the parent's 4 child nodes.
	template <class... ArgTypes>
	std::array<T, 4> construct_array(float x, float y, float parent_size, ArgTypes&&... additional_args)
	{
		// Assert that T has a ctor which takes the given argument types.
		static_assert(std::is_constructible_v<T, float, float, float, ArgTypes&&...>,
			"QuadChildren: node type must have a constructor that takes the given argument types.");

		float child_size = parent_size / 2;
		float mid_x = x + child_size;
		float mid_y = y + child_size;
		
		return
		{
			T{ x, y, child_size, additional_args... },
			T{ mid_x, y, child_size, additional_args... },
			T{ x, mid_y, child_size, additional_args... },
			T{ mid_x, mid_y, child_size, std::forward<ArgTypes>(additional_args)... },
		};
	}

public:

	template <class... ArgTypes>
	QuadChildren<T>(float x, float y, float parent_size, ArgTypes&&... additional_args)
		: children(construct_array(x, y, parent_size, std::forward<ArgTypes>(additional_args)...))
	{}

	QuadChildren<T>() = default;

	// Returns the upper left child node.
	T& UL() { return children[0]; }

	// Returns the upper right child node.
	T& UR() { return children[1]; }

	// Returns the lower left child node.
	T& LL() { return children[2]; }

	// Returns the lower right child node.
	T& LR() { return children[3]; }

	// Using array iterators instead of custom.

	std::array<T, 4>::iterator begin()
	{
		return children.begin();
	}

	std::array<T, 4>::iterator end()
	{
		return children.end();
	}

	std::span<const T, 4> data() const
	{
		return children;
	}

	/*
	Returns the first child node where the predicate is true, or nullptr if predicate false in all four child nodes.
	Checks in order: UL, UR, LL, LR.
	*/
	template<auto bool_func, class... ArgTypes>
	T* get_quad(ArgTypes&&... args)
	{
		// Assert provided bool_func returns a bool when called on a quad node with args... parameter types.
		static_assert(std::is_invocable_r_v<bool, decltype(bool_func), T&&, ArgTypes&&...>,
			"Given function must return a bool when called on a quad node with the given parameter types.");

		for (T& child : children) {
			if (std::invoke(bool_func, child, args...)) {
				return &child;
			}
		}

		return nullptr;
	}

	// Returns all child quads (of UL, UR, LL, LR)  where the predicate is true.
	template<auto bool_func, class... ArgTypes>
	std::vector<T*> get_quads(ArgTypes&&... args)
	{
		// Assert provided bool_func returns a bool when called on a quad node with args... parameter types.
		static_assert(std::is_invocable_r_v<bool, decltype(bool_func), T&&, ArgTypes&&...>,
			"Given function must return a bool when called on a quad node with the given parameter types.");

		std::vector<T*> quads;
		quads.reserve(4);

		for (T& child : children) {
			if (std::invoke(bool_func, child, args...)) {
				quads.push_back(&child);
			}
		}

		return quads;
	}

	// add_to_child

};
