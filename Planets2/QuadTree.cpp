#include "QuadTree.h"
#include <string>
#include "Physics.h"
#include "Body.h"

#include "Collision.h"
#include "DebugInfo.h"
#include <algorithm>

DynamicPool<QuadChildren<QuadNode>> QuadNode::quad_pool { 100 };

QuadNode::QuadNode(float x, float y, float size, int depth) :
	dimensions { x, y, size, size }, depth(depth)
{}

int QuadNode::get_collisions(std::vector<Collision>& collisions) const
{
	int checks = 0;

	if (is_leaf())
	{
		if (!quad_bodies.empty())
		{
			for (auto it = quad_bodies.begin(); it != quad_bodies.end() - 1; it++)
			{
				Body& body = **it;
				checks += get_collisions_internal(body, it + 1, quad_bodies.end(), collisions);
			}
		}
	}
	else
	{

		/* Need to do a collision check on our bodies.
		*
		* This is different from a collision check in a leaf node.
		* Our bodies can collide with each other, and with bodies in our child nodes.
		*
		* Specifically the child nodes that contains_partially(our_body)
		* This involves recursion downwards.
		*/


		if (!quad_bodies.empty())
		{
			// First, collision check with our node's bodies.
			for (auto it = quad_bodies.begin(); it != quad_bodies.end() - 1; it++)
			{
				Body& body = **it;
				checks += get_collisions_internal(body, it + 1, quad_bodies.end(), collisions);
			}

			// Now, do a collision check on each body with the bodies of relevant child nodes.
			for (auto it = quad_bodies.begin(); it != quad_bodies.end(); it++)
			{
				Body& body = **it;
				checks += get_collisions_child(body, collisions);
			}
		}

		// Can come before or after earlier checks.
		for (QuadNode& node : *children)
		{
			checks += node.get_collisions(collisions);
		}

	}

	return checks;
}

int QuadNode::get_collisions_child(Body& checking, std::vector<Collision>& collisions) const
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
			checks += quad.get_collisions_internal(checking, quad.quad_bodies.begin(), quad.quad_bodies.end(), collisions);

			if (!quad.is_leaf())
			{
				checks += quad.get_collisions_child(checking, collisions);
			}
		}
	}

	return checks;

}

QuadNode* QuadNode::add_internal(Body& body)
{
	cur_size++;

	if (is_leaf())
	{
		quad_bodies.push_back(&body);
		return this;
	}
	else
	{
		if (is_root() and !contains_fully(body))
		{
			// Body is slightly out of bounds of the entire quad tree.
			quad_bodies.push_back(&body);
			return this;
		}
		else
		{
			return selective_add(body);
		}
	}
}

int QuadNode::get_collisions_internal(Body& checking, std::vector<Body*>::const_iterator it, std::vector<Body*>::const_iterator end, std::vector<Collision>& collisions) const
{
	int checks = 0;

	while (it != end)
	{
		Body& body2 = **it;

		checks++;

		if (checking.collided_with(body2))
		{
			collisions.emplace_back(Body::get_sorted_pair(checking, body2));
		}

		it++;
	}

	return checks;
}

bool QuadNode::in_more_than_one_child(const Body& body) const
{
	return std::ranges::count_if(*children, [&body](const QuadNode& n) { return n.contains_partially(body); }) > 1;
}

QuadNode* QuadNode::selective_add(Body& new_body)
{
	// This = parent node.
	// 
	// Add to a child if body fully contained in it, else add to self.

	if (in_more_than_one_child(new_body))
	{
		quad_bodies.push_back(&new_body);
		return this;
	}
	else
	{
		// is only in one child node.
		return add_to_child(new_body); // adds to the child that contains the body fully.
	}
}

void QuadNode::add_body(Body& new_body, int max_bodies, int max_depth)
{
	QuadNode* added_to = add_internal(new_body);

	// should never be nullptr.
	added_to->split_check(max_bodies, max_depth);

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

bool QuadNode::should_split(int max_bodies) const
{
	return cur_size > max_bodies;
}

bool QuadNode::reached_depth_limit(int max_depth) const
{
	return depth >= max_depth;
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
			{ // still fully contains this body, no changes needed.
				it++;
			}
			else
			{ // body no longer completely inside this leaf node.
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
					// all child nodes have already been updated, safe to call add_body (possible split) on them.
					// may want to add_internal though, since we will do a split check anyway.
					contained_in->add_body(body, max_bodies, max_depth);
					it = quad_bodies.erase(it);
				}
				else
				{ // No child quad fully contains the body, so keep it in this node.
					it++;
				}


			}
			else
			{ // body no longer completely inside this node.

				if (!is_root())
				{
					it = move_up(it);
				}
				else
				{
					// wraparound or deletion is about to happen.
					it++;
				}

			}

			to_check--;

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

std::vector<Body*>::iterator QuadNode::move_to_child(std::vector<Body*>::iterator it)
{
	add_to_child(**it);
	// Body moved to child node, so no need to decrease our size.

	return quad_bodies.erase(it);
}

std::vector<Body*>::iterator QuadNode::move_up(std::vector<Body*>::iterator it)
{
	// Body is leaving this quad entirely, so decrease size.
	cur_size--;
	parent->reinsert(**it);

	return quad_bodies.erase(it);

}

QuadNode* QuadNode::add_to_child(Body& new_body)
{
	// Get the child quad that fully contains the body, if any do.
	QuadNode* contained_in = children->get_quad<&QuadNode::contains_fully>(new_body);
	// if any child quad fully contains the body, add it to the quad.
	if (contained_in)
	{
		return contained_in->add_internal(new_body);
	}
	else
	{
		return nullptr;
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

void QuadNode::split(int max_depth)
{
	float x = dimensions.x;
	float y = dimensions.y;

	// Depth level of the child nodes.
	int next_depth = depth + 1;

	children = quad_pool.get(x, y, dimensions.width, next_depth);

	for (QuadNode& node : *children)
	{
		node.parent = this;
	}

	// add bodies to respective quads

	auto it = quad_bodies.begin();
	while (it < quad_bodies.end())
	{
		Body& body = **it;

		if (in_more_than_one_child(body))
		{
			// stays in parent (here), since it is not unique to any child node.
			it++;
		}
		else
		{
			if (is_root() and !contains_fully(body))
			{
				// Body is slightly out of bounds of the entire quad tree.
				it++;
			}
			else
			{
				// move body to the child node that contains it.
				it = move_to_child(it);
			}
		}
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
		selective_add(body);
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

	if (is_root())
	{
		if (!is_leaf() and should_concatenate(max_bodies))
		{
			concatenate();
		}
		return;
	}

	// not root.

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
		parent->concat_check(max_bodies);
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
		if (should_split(max_bodies) and !reached_depth_limit(max_depth))
		{
			split(max_depth);

			for (QuadNode& node : *children)
			{
				node.split_check(max_bodies, max_depth);
			}
		}
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
	root { -size / 2.0f, -size / 2.0f, size, 0 }, max_bodies_per_quad(max_bodies_per_quad), max_depth(max_depth)
{}

std::vector<Collision> QuadTree::get_collisions_impl()
{
	std::vector<Collision> collisions;
	num_collision_checks_tick = root.get_collisions(collisions);
	return collisions;
}