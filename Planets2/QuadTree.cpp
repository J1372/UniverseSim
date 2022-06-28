#include "QuadTree.h"
#include <string>
#include "Physics.h"
#include "Body.h"

#include "Collision.h"

DynamicPool<QuadChildren<QuadTree>> QuadTree::quad_pool{ 100 };
int QuadTree::max_bodies_per_quad = 10;
int QuadTree::max_depth = 5;
int QuadTree::quads_generated = 0;

QuadTree::QuadTree(float size, int max_bodies_per_quad, int max_depth) :
	dimensions{ -size / 2.0f, -size / 2.0f, size, size }, quad_id(quads_generated++), depth(0)
{
	QuadTree::max_bodies_per_quad = max_bodies_per_quad;
	QuadTree::max_depth = max_depth;
	QuadTree::quads_generated = 0;
}

QuadTree::QuadTree(float x, float y, float size, int depth) :
	dimensions{ x, y, size, size }, quad_id(quads_generated++), depth(depth)
{}

int QuadTree::get_collisions(std::vector<Collision>& collisions) const
{
	int checks = 0;

	if (is_leaf()) {
		if (!quad_bodies.empty()) {
			for (auto it = quad_bodies.begin(); it != quad_bodies.end() - 1; it++) {
				Body& body = **it;
				checks += get_collisions_internal(body, it + 1, quad_bodies.end(), collisions);
			}
		}
	}
	else {

		/* Need to do a collision check on our bodies.
		*
		* This is different from a collision check in a leaf node.
		* Our bodies can collide with each other, and with bodies in our child nodes.
		*
		* Specifically the child nodes that contains_partially(our_body)
		* This involves recursion downwards.
		*/


		if (!quad_bodies.empty()) {
			// First, collision check with our node's bodies.
			for (auto it = quad_bodies.begin(); it != quad_bodies.end() - 1; it++) {
				Body& body = **it;
				checks += get_collisions_internal(body, it + 1, quad_bodies.end(), collisions);
			}

			// Now, do a collision check on each body with the bodies of relevant child nodes.
			for (auto it = quad_bodies.begin(); it != quad_bodies.end(); it++) {
				Body& body = **it;
				checks += get_collisions_child(body, collisions);
			}
		}

		// Can come before or after earlier checks.
		for (QuadTree& node : *children) {
			checks += node.get_collisions(collisions);
		}

	}

	return checks;
}

std::vector<Collision> QuadTree::get_collisions()
{
	std::vector<Collision> collisions;
	num_collision_checks_tick = get_collisions(collisions);
	return collisions;
}

int QuadTree::get_collisions_child(Body& checking, std::vector<Collision>& collisions) const
{
	/* 
	* 
	* For each child quad which contains partially the body:
	*	handle_collision body with all in that node
	* 
	*/

	int checks = 0;

	// Get a list of all our child quads that at least partially contain the body.
	std::vector<QuadTree*> to_check = children->get_quads<&QuadTree::contains_partially>(checking);

	// Check for collisions between the given body and the bodies of relevant child nodes.
	for (QuadTree* quad : to_check) {
		// get_collisions_internal can be static, and renamed.
		checks += quad->get_collisions_internal(checking, quad->quad_bodies.begin(), quad->quad_bodies.end(), collisions);

		if (!quad->is_leaf()) {
			checks += quad->get_collisions_child(checking, collisions);
		}

	}

	return checks;

}

QuadTree* QuadTree::add_internal(Body& body)
{
	cur_size++;

	if (is_leaf()) {
		quad_bodies.push_back(&body);
		return this;
	}
	else {
		if (is_root() and !contains_fully(body)) {
			// Body is slightly out of bounds of the entire quad tree.
			quad_bodies.push_back(&body);
			return this;
		}
		else {
			return selective_add(body);
		}
	}
}

int QuadTree::get_collisions_internal(Body& checking, std::vector<Body*>::const_iterator it, std::vector<Body*>::const_iterator end, std::vector<Collision>& collisions) const
{
	int checks = 0;

	while (it != end) {
		Body& body2 = **it;

		checks++;

		if (Physics::have_collided(checking, body2)) {
			collisions.emplace_back(Body::get_sorted_pair(checking, body2));
		}

		it++;
	}

	return checks;
}

bool QuadTree::in_more_than_one_child(const Body& body) const
{
	// Get a list of our child quads that at least partially contain the body.
	std::vector<QuadTree*> contained_in = children->get_quads<&QuadTree::contains_partially>(body);

	// if in more than one child quad, return true.
	return contained_in.size() > 1;
}

QuadTree* QuadTree::selective_add(Body& new_body)
{
	// This = parent node.
	// 
	// Add to a child if body fully contained in it, else add to self.

	if (in_more_than_one_child(new_body)) {
		quad_bodies.push_back(&new_body);
		return this;
	}
	else { // is only in one child node.
		return add_to_child(new_body); // adds to the child that contains the body fully.
	}
}

void QuadTree::add_body(Body& new_body)
{
	QuadTree* added_to = add_internal(new_body);

	// should never be nullptr.
	added_to->split_check();
	
}

void QuadTree::rem_body(const Body& body)
{
	// Could be an outside command, so body could be in one of our child nodes.
	QuadTree& quad = find_quad(body);
	quad.rem_body_internal(body);
}

bool QuadTree::is_leaf() const
{
	return children == nullptr;
}

bool QuadTree::is_root() const
{
	return parent == nullptr;
}

bool QuadTree::is_empty() const
{
	return cur_size == 0;
}

bool QuadTree::has_room() const
{
	return cur_size < max_bodies_per_quad;
}

bool QuadTree::is_full() const
{
	return cur_size >= max_bodies_per_quad;
}
bool QuadTree::should_concatenate() const
{
	return cur_size <= max_bodies_per_quad;
}

bool QuadTree::should_split() const
{
	return cur_size > max_bodies_per_quad;
}

bool QuadTree::reached_depth_limit() const
{
	return depth >= max_depth;
}

void QuadTree::rem_body_internal(const Body& body)
{
	auto it = std::find(quad_bodies.begin(), quad_bodies.end(), &body);
	quad_bodies.erase(it);
	notify_child_removed();
	concat_check();
}

Body* QuadTree::find_body(Vector2 point) const
{
	// Because bodies can be in branch nodes too, can't just go to the leaf node and search.
	auto found = std::find_if(quad_bodies.begin(), quad_bodies.end(), [point](const Body* body) { return body->contains_point(point); });

	if (found != quad_bodies.end()) {
		return *found;
	} else if (!is_leaf()) {
		// Find the quad that the (x,y) point should be in, then recurse into it.
		QuadTree* quad = children->get_quad<&QuadTree::contains_point>(point);
		return quad->find_body(point);
	}

	return nullptr;
}

std::span<const QuadTree, 4> QuadTree::get_quads() const
{
	return children->data();
}

void QuadTree::update()
{
	update_internal();
	split_check();
}

void QuadTree::update_internal()
{
	// After body position update tick, update the quad with new positions.
	if (is_leaf()) {
		auto it = quad_bodies.begin();
		while (it != quad_bodies.end()) {
			Body& body = **it;

			if (contains_fully(body) or is_root()) { // still fully contains this body, no changes needed.
				it++;
			}
			else { // body no longer completely inside this leaf node.
				it = move_up(it);
			}

		}

	}
	else {
		int to_check = quad_bodies.size();

		for (QuadTree& node : *children) {
			node.update_internal();
		}

		// only check bodies which child nodes have not just reinserted upwards to this node.
		auto it = quad_bodies.begin();
		while (to_check != 0) {
			Body& body = **it;

			if (contains_fully(body)) {
				// move to child node if body is fully contained by it.
				
				// Get the child quad that fully contains the body, if any do.
				QuadTree* contained_in = children->get_quad<&QuadTree::contains_fully>(body);

				// If the body is fully contained in a child quad, move it into that quad.
				if (contained_in) {
					// all child nodes have already been updated, safe to call add_body (possible split) on them.
					// may want to add_internal though, since we will do a split check anyway.
					contained_in->add_body(body); 
					it = quad_bodies.erase(it);
				}
				else { // No child quad fully contains the body, so keep it in this node.
					it++;
				}


			}
			else { // body no longer completely inside this node.

				if (!is_root()) {
					it = move_up(it);
				}
				else {
					// wraparound or deletion is about to happen.
					it++;
				}

			}

			to_check--;

		}

		// We may have reinserted upwards from our node or child nodes.
		if (should_concatenate()) {
			concatenate();
		}
	}
}

bool QuadTree::contains_point(Vector2 point) const
{
	return Physics::point_in_rect(point, dimensions);
}

bool QuadTree::contains_fully(const Body& body) const
{
	return Physics::body_inside_rect(body, dimensions);
}

bool QuadTree::contains_partially(const Body& body) const
{
	return Physics::body_intersects_rect(body, dimensions);
}

void QuadTree::notify_child_removed()
{
	cur_size--;

	if (!is_root()) {
		parent->notify_child_removed();
	}
}

std::vector<Body*>::iterator QuadTree::move_to_child(std::vector<Body*>::iterator it)
{
	add_to_child(**it);
	// Body moved to child node, so no need to decrease our size.

	return quad_bodies.erase(it);
}

std::vector<Body*>::iterator QuadTree::move_up(std::vector<Body*>::iterator it)
{
	// Body is leaving this quad entirely, so decrease size.
	cur_size--;
	parent->reinsert(**it);

	return quad_bodies.erase(it);

}

QuadTree* QuadTree::add_to_child(Body& new_body)
{
	// Get the child quad that fully contains the body, if any do.
	QuadTree* contained_in = children->get_quad<&QuadTree::contains_fully>(new_body);
	// if any child quad fully contains the body, add it to the quad.
	if (contained_in) {
		return contained_in->add_internal(new_body);
	}
	else {
		return nullptr;
	}
}

void QuadTree::concatenate()
{
	// If we are concatenating, then all of our child nodes are leaves.
	// So we only have to move their quad bodies here, no need to recurse.
	
	// Calculating and reserving capacity shouldn't be necessary,
	// since we only split because we reached max capacity.
	// And we are only concatenating because we are below max capacity.

	// copy all body pointers from a quad, without erasing them in that quad.
	auto copy_bodies = [](const QuadTree& quad, std::vector<Body*>& copy_to) {
		copy_to.insert(copy_to.end(), quad.quad_bodies.begin(), quad.quad_bodies.end());
	};

	for (QuadTree& node : *children) {
		copy_bodies(node, quad_bodies);
	}

	children.reset();
}

void QuadTree::split()
{
	float x = dimensions.x;
	float y = dimensions.y;

	// Depth level of the child nodes.
	int next_depth = depth + 1;

	children = quad_pool.get(x, y, dimensions.width, next_depth);

	for (QuadTree& node : *children) {
		node.parent = this;
	}

	// add bodies to respective quads

	auto it = quad_bodies.begin();
	while (it < quad_bodies.end()) { //&& ref
		Body& body = **it;

		if (in_more_than_one_child(body)) {
			// stays in parent (here), since it is not unique to any child node.
			it++;
		}
		else {
			if (is_root() and !contains_fully(body)) {
				// Body is slightly out of bounds of the entire quad tree.
				it++;
			}
			else {
				// move body to the child node that contains it.
				it = move_to_child(it);
			}
		}
	}
}

void QuadTree::reinsert(Body& body)
{
	// Reinserting body upwards from a child node (which may or may not be a leaf node).

	// The current node is never a leaf node.

	// We want to keep moving up the chain if the body is not fully contained in this node.
	// We want to add the body to the current node if it is fully contained in it.
	//  If the body is fully contained in one of our children, add it to there instead.
	if (contains_fully(body)) { // add to self. or add to child if fully fits in a child node.
		// No need to increment cur_size, since the body is already counted (was in child node).
		selective_add(body);
	}
	else if (is_root()) {
		// If body is not completely in the root quadtree, it is about to be wrapped around to the other side
		//	since the quadtree root's size is the same size as the max universe size.
		quad_bodies.push_back(&body);
	}
	else { // not fully contained in this node, and this node is not the root (parent != nullptr)
		// Not fully contained in this node, needs to continue moving upwards.
		cur_size--; // was previously in a child node, and now is moving to our parent node.
		parent->reinsert(body);
	}
}

void QuadTree::concat_check()
{

	// After a body has been removed from the quad tree entirely,
	// we do this check on the node it was removed from and up the parent chain.
	// 
	// A removal in a child node can result in a sequence of concatenations of its parents.
	// If a node does not concatenate as a result of the removal, its parent wont concatenate either.

	if (is_root()) {
		if (!is_leaf() and should_concatenate()) {
			concatenate();
		}
		return;
	}

	// not root.

	if (is_leaf()) { // leaves cannot concatenate, but check parent.
		parent->concat_check();
	}
	else if (should_concatenate()) { // parent, and not at max capacity.
		// if we are a parent and we concatenate, we still need to tell our parent to check for concatenation.
		concatenate();
		parent->concat_check();
	}
	else {
		// parent and doesn't need to concatenate.
		// do nothing and stop checking.
		// if we are a parent that didn't concatenate, then none of our parents concatenated either.
	}
}

void QuadTree::get_representation_internal(std::vector<Rectangle>& rep) const {
	if (is_leaf()) {
		rep.push_back(dimensions);
	}
	else {
		for (QuadTree& node : *children) {
			node.get_representation_internal(rep);
		}
	}

}

void QuadTree::split_check()
{
	if (is_leaf()) {
		if (should_split() and !reached_depth_limit()) {
			split();

			for (QuadTree& node : *children) {
				node.split_check();
			}
		}
	}
	else {
		for (QuadTree& node : *children) {
			node.split_check();
		}
	}
}


std::vector<Rectangle> QuadTree::get_representation() const {
	std::vector<Rectangle> rep;
	get_representation_internal(rep);
	return rep;
}

QuadTree& QuadTree::find_quad(const Body& body)
{
	// Can't recurse any further, and all bodies are somewhere in the quadtree, so it must be in this one.
	if (is_leaf()) {
		return *this;
	}

	// Is a parent node.

	// Get the child quad that fully contains the body, if any do.
	QuadTree* contained_in = children->get_quad<&QuadTree::contains_fully>(body);

	// If a child node fully contains the body we are looking for, then recurse into it.
	if (contained_in) {
		return contained_in->find_quad(body);
	}

	// No child node fully contains the body, and all bodies are somewhere in the quadtree, so it must be in this one.
	return *this;
}

const QuadTree& QuadTree::find_quad(const Body& body) const
{
	// Can't recurse any further, and all bodies are somewhere in the quadtree, so it must be in this one.
	if (is_leaf()) {
		return *this;
	}

	// Is a parent node.

	// Get the child quad that fully contains the body, if any do.
	QuadTree* contained_in = children->get_quad<&QuadTree::contains_fully>(body);

	// If a child node fully contains the body we are looking for, then recurse into it.
	if (contained_in) {
		return contained_in->find_quad(body);
	}

	// No child node fully contains the body, and all bodies are somewhere in the quadtree, so it must be in this one.
	return *this;
}

void QuadTree::attach_debug_text(Body& body) const
{
	const QuadTree& quad = find_quad(body);

	body.add_debug_text("Quad ID: " + std::to_string(quad.quad_id));
	body.add_debug_text("Quad Bodies: " + std::to_string(quad.cur_size));

}
