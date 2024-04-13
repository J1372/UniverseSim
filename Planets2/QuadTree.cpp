#include "QuadTree.h"
#include <string>
#include "Physics.h"
#include "Body.h"

#include "Collision.h"
#include "DebugInfo.h"
#include <algorithm>

QuadNode::QuadNode(float x, float y, float size, QuadNode* parent, int depth, int max_bodies) :
	dimensions { x, y, size, size }, depth(depth), parent(parent)
{
	quad_bodies.reserve(max_bodies);
}

int QuadNode::get_collisions(std::vector<Collision>& collisions)
{
	int checks = 0;

	Body** end = quad_bodies.data() + quad_bodies.size();
	for (Body** it = quad_bodies.data(); it < end - 1; ++it)
	{
		Body& body = **it;
		checks += get_collisions_internal(body, it + 1, end, collisions);
	}

	if (!is_leaf())
	{
		// Need to do a collision check with bodies in child nodes.
		for (Body* body : quad_bodies)
		{
			checks += get_collisions_child(*body, collisions);
		}

		// Can come before or after earlier checks.
		for (QuadNode& node : *children)
		{
			checks += node.get_collisions(collisions);
		}
	}

	return checks;
}

int QuadNode::get_collisions_child(Body& checking, std::vector<Collision>& collisions)
{
	/*
	*
	* For each child quad which contains partially the body:
	*	handle_collision body with all in that node
	*
	*/

	int checks = 0;

	// Check for collisions between the given body and the bodies of relevant child nodes.
	for (QuadNode& quad : *children)
	{
		if (quad.contains_partially(checking))
		{
			// get_collisions_internal can be static, and renamed.
			checks += quad.get_collisions_internal(checking, quad.quad_bodies.data(), quad.quad_bodies.data() + quad.quad_bodies.size(), collisions);

			if (!quad.is_leaf())
			{
				checks += quad.get_collisions_child(checking, collisions);
			}
		}
	}

	return checks;

}

int QuadNode::get_collisions_internal(Body& checking, Body** start, Body** end, std::vector<Collision>& collisions)
{
	for (Body** it = start; it < end; ++it)
	{
		Body& body = **it;

		if (checking.collided_with(body))
		{
			collisions.emplace_back(Body::get_sorted_pair(checking, body));
		}
	}

	return end - start;
}

void QuadNode::add_body(Body& new_body, int max_bodies, int max_depth)
{
	++cur_size;

	if (is_leaf())
	{
		quad_bodies.push_back(&new_body);
		leaf_split_check(max_bodies, max_depth);
	}
	else
	{
		QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(new_body);

		if (contained_in)
		{
			contained_in->add_body(new_body, max_bodies, max_depth);
		}
		else
		{
			quad_bodies.push_back(&new_body);
		}
	}
}

void QuadNode::rem_body(const Body& body, int max_bodies)
{
	--cur_size;

	QuadNode* contained_in = is_leaf() ? nullptr : children->get_quad<&QuadNode::contains_fully>(body);
	// If a child node fully contains the body we are looking for, then recurse into it.
	if (contained_in)
	{
		contained_in->rem_body(body, max_bodies);
	}
	else
	{
		auto it = std::ranges::find(quad_bodies, &body);
		quad_bodies.erase(it);
		concat_check(max_bodies);
	}
}

void QuadNode::notify_move(const Body* from, Body* to)
{
	if (is_leaf())
	{
		auto it = std::ranges::find(quad_bodies, from);
		*it = to;
	}
	else
	{
		QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(*from);
		if (contained_in)
		{
			contained_in->notify_move(from, to);
		}
		else
		{
			auto it = std::ranges::find(quad_bodies, from);
			*it = to;
		}
	}

}

bool QuadNode::is_leaf() const
{
	return children == nullptr;
}

bool QuadNode::is_root() const
{
	return parent == nullptr;
}

bool QuadNode::is_empty() const
{
	return cur_size == 0;
}

bool QuadNode::has_room(int max_bodies) const
{
	return cur_size < max_bodies;
}

bool QuadNode::is_full(int max_bodies) const
{
	return cur_size >= max_bodies;
}

bool QuadNode::should_concatenate(int max_bodies) const
{
	return cur_size <= max_bodies;
}

Body* QuadNode::find_body(Vector2 point) const
{
	// Because bodies can be in branch nodes too, can't just go to the leaf node and search.
	auto found = std::ranges::find_if(quad_bodies, [point](const Body* body) { return body->contains_point(point); });

	if (found != quad_bodies.end())
	{
		return *found;
	}
	else if (!is_leaf())
	{
		// Find the quad that the (x,y) point should be in, then recurse into it.
		QuadNode* quad = children->get_quad<&QuadNode::contains_point>(point);
		return quad->find_body(point);
	}

	return nullptr;
}

void QuadNode::update(int max_bodies, int max_depth)
{
	update_internal(max_bodies, max_depth);
	split_check(max_bodies, max_depth);
}

void QuadNode::update_internal(int max_bodies, int max_depth)
{
	// After body position update tick, update the quad with new positions.
	if (is_leaf())
	{
		auto it = quad_bodies.begin();
		while (it != quad_bodies.end())
		{
			Body& body = **it;

			if (contains_fully(body) or is_root())
			{
				// still fully contains this body, no changes needed.
				it++;
			}
			else
			{
				// body no longer completely inside this leaf node.
				it = move_up(it);
			}
		}
	}
	else
	{
		int to_check = quad_bodies.size();

		for (QuadNode& node : *children)
		{
			node.update_internal(max_bodies, max_depth);
		}

		// only check bodies which child nodes have not just reinserted upwards to this node.
		auto it = quad_bodies.begin();
		while (to_check != 0)
		{
			Body& body = **it;

			if (contains_fully(body))
			{
				// move to child node if body is fully contained by it.

				// Get the child quad that fully contains the body, if any do.
				QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(body);

				// If the body is fully contained in a child quad, move it into that quad.
				if (contained_in)
				{
					// all child nodes have already been updated, would be safe to call add_body (possible split) on them.
					contained_in->add_no_split(body);
					it = quad_bodies.erase(it);
				}
				else
				{
					// No child quad fully contains the body, so keep it in this node.
					it++;
				}
			}
			else if (is_root())
			{
				// wraparound or deletion is about to happen.
				it++;
			}
			else
			{
				it = move_up(it);
			}
			
			--to_check;
		}

		// We may have reinserted upwards from our node or child nodes.
		if (should_concatenate(max_bodies))
		{
			concatenate();
		}
	}
}

bool QuadNode::contains_point(Vector2 point) const
{
	return Physics::point_in_rect(point, dimensions);
}

bool QuadNode::contains_fully(const Body& body) const
{
	return body.in_rect(dimensions);
}

bool QuadNode::contains_partially(const Body& body) const
{
	return body.intersects_rect(dimensions);
}

std::vector<Body*>::iterator QuadNode::move_up(std::vector<Body*>::iterator it)
{
	// Body is leaving this quad entirely, so decrease size.
	cur_size--;
	parent->reinsert(**it);

	return quad_bodies.erase(it);

}

void QuadNode::leaf_split_check(int max_bodies, int max_depth)
{
	if (cur_size > max_bodies and depth < max_depth)
	{
		split(max_bodies, max_depth);
	}
}

void QuadNode::concatenate()
{
	// If we are concatenating, then all of our child nodes are leaves.
	// So we only have to move their quad bodies here, no need to recurse.

	// Calculating and reserving capacity shouldn't be necessary,
	// since we only split because we reached max capacity.
	// And we are only concatenating because we are below max capacity.

	// copy all body pointers from a quad, without erasing them in that quad.
	auto copy_bodies = [](const QuadNode& quad, std::vector<Body*>& copy_to)
	{
		copy_to.insert(copy_to.end(), quad.quad_bodies.begin(), quad.quad_bodies.end());
	};

	for (QuadNode& node : *children)
	{
		copy_bodies(node, quad_bodies);
	}

	children.reset();
}

void QuadNode::split(int max_bodies, int max_depth)
{
	float x = dimensions.x;
	float y = dimensions.y;

	// Depth level of the child nodes.
	int next_depth = depth + 1;

	children = std::make_unique<QuadChildren<QuadNode>>(x, y, dimensions.width, this, next_depth, max_bodies);

	// add bodies to respective quads

	auto it = quad_bodies.begin();
	while (it != quad_bodies.end())
	{
		Body& body = **it;

		QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(body);
		if (contained_in)
		{
			// move to child
			contained_in->quad_bodies.push_back(&body);
			++contained_in->cur_size;
			it = quad_bodies.erase(it);
		}
		else
		{
			// stays in parent (here), since it is not unique to any child node.
			it++;
		}
	}

	// If all objects in quad moved into one child, then may need to split that child as well.
	for (QuadNode& node : *children)
	{
		node.leaf_split_check(max_bodies, max_depth);
	}
}

void QuadNode::add_no_split(Body& body)
{
	++cur_size;

	if (is_leaf())
	{
		quad_bodies.push_back(&body);
	}
	else if (QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(body))
	{
		contained_in->add_no_split(body);
	}
	else
	{
		quad_bodies.push_back(&body);
	}
}

void QuadNode::reinsert(Body& body)
{
	// Reinserting body upwards from a child node (which may or may not be a leaf node).

	// The current node is never a leaf node.

	// We want to keep moving up the chain if the body is not fully contained in this node.
	// We want to add the body to the current node if it is fully contained in it.
	//  If the body is fully contained in one of our children, add it to there instead.
	if (contains_fully(body))
	{
		// add to self. or add to child if fully fits in a child node.
		// No need to increment cur_size, since the body is already counted (was in child node).
		QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(body);
		if (contained_in)
		{
			contained_in->add_no_split(body);
		}
		else
		{
			quad_bodies.push_back(&body);
		}
	}
	else if (is_root())
	{
		// If body is not completely in the root quadtree, it is about to be wrapped around to the other side
		//	since the quadtree root's size is the same size as the max universe size.
		quad_bodies.push_back(&body);
	}
	else
	{
		// not fully contained in this node, and this node is not the root (parent != nullptr)
		// Not fully contained in this node, needs to continue moving upwards.
		--cur_size; // was previously in a child node, and now is moving to our parent node.
		parent->reinsert(body);
	}
}

void QuadNode::concat_check(int max_bodies)
{
	// After a body has been removed from the quad tree entirely,
	// we do this check on the node it was removed from and up the parent chain.
	// 
	// A removal in a child node can result in a sequence of concatenations of its parents.
	// If a node does not concatenate as a result of the removal, its parent wont concatenate either.

	if (is_leaf())
	{
		// leaves cannot concatenate, but check parent.
		parent->concat_check(max_bodies);
	}
	else if (should_concatenate(max_bodies))
	{
		// parent, and not at max capacity.
		// if we are a parent and we concatenate, we still need to tell our parent to check for concatenation.
		concatenate();
		if (!is_root())
		{
			parent->concat_check(max_bodies);
		}
	}
	else
	{
		// parent and doesn't need to concatenate.
		// do nothing and stop checking.
		// if we are a parent that didn't concatenate, then none of our parents concatenated either.
	}
}

void QuadNode::get_representation_internal(std::vector<Rectangle>& rep) const
{
	if (is_leaf())
	{
		rep.push_back(dimensions);
	}
	else
	{
		for (QuadNode& node : *children)
		{
			node.get_representation_internal(rep);
		}
	}

}

void QuadNode::split_check(int max_bodies, int max_depth)
{
	if (is_leaf())
	{
		leaf_split_check(max_bodies, max_depth);
	}
	else
	{
		for (QuadNode& node : *children)
		{
			node.split_check(max_bodies, max_depth);
		}
	}
}


std::vector<Rectangle> QuadNode::get_representation() const
{
	std::vector<Rectangle> rep;
	get_representation_internal(rep);
	return rep;
}

QuadNode& QuadNode::find_quad(const Body& body)
{
	// Can't recurse any further, and all bodies are somewhere in the quadtree, so it must be in this one.
	if (is_leaf())
	{
		return *this;
	}

	// Is a parent node.

	// Get the child quad that fully contains the body, if any do.
	QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(body);

	// If a child node fully contains the body we are looking for, then recurse into it.
	if (contained_in)
	{
		return contained_in->find_quad(body);
	}

	// No child node fully contains the body, and all bodies are somewhere in the quadtree, so it must be in this one.
	return *this;
}

const QuadNode& QuadNode::find_quad(const Body& body) const
{
	// Can't recurse any further, and all bodies are somewhere in the quadtree, so it must be in this one.
	if (is_leaf())
	{
		return *this;
	}

	// Is a parent node.

	// Get the child quad that fully contains the body, if any do.
	QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(body);

	// If a child node fully contains the body we are looking for, then recurse into it.
	if (contained_in)
	{
		return contained_in->find_quad(body);
	}

	// No child node fully contains the body, and all bodies are somewhere in the quadtree, so it must be in this one.
	return *this;
}

void QuadNode::get_info(const Body& body, DebugInfo& info) const
{
	const QuadNode& quad = find_quad(body);

	info.add("Quad Bodies: " + std::to_string(quad.cur_size));
	info.add("Quad Depth: " + std::to_string(quad.depth));

}

QuadTree::QuadTree(float size, int max_bodies_per_quad, int max_depth) :
	root { -size / 2.0f, -size / 2.0f, size, nullptr, 0, max_bodies_per_quad },
	max_bodies_per_quad(max_bodies_per_quad),
	max_depth(max_depth)
{}

std::vector<Collision> QuadTree::get_collisions_impl()
{
	std::vector<Collision> collisions;
	num_collision_checks_tick = root.get_collisions(collisions);
	return collisions;
}
