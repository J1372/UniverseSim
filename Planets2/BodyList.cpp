#include "BodyList.h"

void BodyList::add(Body&& body)
{
	Body& added = active_bodies.emplace_back(body);
	int id = generated_bodies++;
	added.set_id(id);

	// Map id to index that the new body was placed in.
	id_map[id] = active_bodies.size() - 1;

}

void BodyList::rem(Body& body)
{
	Body& last = active_bodies.back();

	// This code works even if body == last, no conflict.
	int index_removed = id_map[body.get_id()];
	id_map[last.get_id()] = index_removed;
	id_map.erase(body.get_id());


	std::swap(body, last);
	active_bodies.pop_back();
}

int BodyList::size() const
{
	return active_bodies.size();
}

bool BodyList::empty() const
{
	return active_bodies.empty();
}

void BodyList::clear()
{
	generated_bodies = 0;
	active_bodies.clear();
	id_map.clear();
}

void BodyList::reserve(int size)
{
	active_bodies.reserve(size);
}

Body& BodyList::back()
{
	return active_bodies.back();
}

std::span<const Body> BodyList::span() const
{
	return active_bodies;
}

std::vector<Body>::iterator BodyList::begin()
{
	return active_bodies.begin();
}

std::vector<Body>::iterator BodyList::end()
{
	return active_bodies.end();
}

std::vector<Body>::const_iterator BodyList::cbegin() const
{
	return active_bodies.cbegin();
}

std::vector<Body>::const_iterator BodyList::cend() const
{
	return active_bodies.cend();
}

Body& BodyList::operator[](int index)
{
	return active_bodies[index];
}

int BodyList::get_index(int id) const
{
	auto it = id_map.find(id);

	if (it != id_map.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}
