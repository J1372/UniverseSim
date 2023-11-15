#include "NullPartitioning.h"
#include "Body.h"
#include "Collision.h"

std::vector<Collision> NullPartitioning::get_collisions_impl()
{
	if (bodies.empty()) return {};
	std::vector<Collision> collisions;

	collisions.reserve(bodies.size()); // could reserve on start, and on create_body resize it (after done handling).

	for (auto it1 = bodies.begin(); it1 != bodies.end() - 1; it1++)
	{
		Body& body1 = **it1;
		for (auto it2 = it1 + 1; it2 != bodies.end(); it2++)
		{
			Body& body2 = **it2;

			num_collision_checks_tick++;

			if (body1.collided_with(body2))
			{
				collisions.emplace_back(Body::get_sorted_pair(body1, body2));
			}

		}
	}

	return collisions;
}

void NullPartitioning::add_body(Body& body)
{
	bodies.push_back(&body);
}

void NullPartitioning::rem_body(const Body& body)
{
	auto it = std::find(bodies.begin(), bodies.end(), &body);
	std::swap(*it, bodies.back());
	bodies.pop_back();
}

void NullPartitioning::notify_move(const Body* from, Body* to)
{
	auto it1 = std::find(bodies.begin(), bodies.end(), from);
	*it1 = to;
}

void NullPartitioning::update()
{}

Body* NullPartitioning::find_body(Vector2 point) const
{
	auto it = std::find_if(bodies.begin(), bodies.end(), [point](const Body* b)
	{
		return b->contains_point(point);
	});

	if (it != bodies.end())
	{
		return *it;
	}
	else
	{
		return nullptr;
	}
}

std::vector<Rectangle> NullPartitioning::get_representation() const
{
	return {};
}

void NullPartitioning::get_info(const Body& body, DebugInfo& info) const
{}
