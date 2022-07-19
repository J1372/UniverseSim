#include "BodyList.h"

void BodyList::add(Body&& body)
{
	body.set_id(generated_bodies++);
	active_bodies.emplace_back(body);
}

void BodyList::rem(Body& body)
{
	Body& last = active_bodies.back();

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
