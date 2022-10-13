#pragma once

#include <vector>
#include <unordered_map>
#include "Body.h"
#include <span>

class BodyList
{
	// Bodies that are being updated every tick.
	// No longer guaranteed sorted by id.
	std::vector<Body> active_bodies;

	// Maps body ids to indices in active_bodies.
	std::unordered_map<int, int> id_map;

	// Total number of generated bodies (through calls to add())
	int generated_bodies = 0;

public:

	// Adds body to the list and assigns it an id.
	void add(Body&& body);

	// Removes the body associated with the id from active_bodies.
	void rem(Body& body);

	int size() const;
	bool empty() const;

	void clear();
	void reserve(int size);

	Body& back();
	
	std::span<const Body> span() const;

	// using vector iterators instead of custom.
	std::vector<Body>::iterator begin();
	std::vector<Body>::iterator end();
	std::vector<Body>::const_iterator cbegin() const;
	std::vector<Body>::const_iterator cend() const;

	Body& operator[](int index);

	int get_index(int id) const;


};

