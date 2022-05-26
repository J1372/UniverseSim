#include "QuadTree.h"
#include <iostream>
#include <string>
#include "Physics.h"

#include "Collision.h"

int QuadTree::quads_generated = 0;

QuadTree::QuadTree(float size) : dimensions{ -size / 2.0f, -size / 2.0f, size, size }, quad_id(quads_generated++)
{}

QuadTree::QuadTree(float x, float y, float size) : dimensions{ x, y, size, size }, quad_id(quads_generated++)
{}

void QuadTree::get_collisions(std::vector<Collision>& collisions) const
{
	if (is_leaf()) {
		if (!quad_bodies.empty()) {
			for (auto it = quad_bodies.begin(); it != quad_bodies.end() - 1; it++) {
				Body& body = **it;
				get_collisions_internal(body, it + 1, quad_bodies.end(), collisions);
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
				get_collisions_internal(body, it + 1, quad_bodies.end(), collisions);
			}

			// Now, do a collision check on each body with the bodies of relevant child nodes.
			for (auto it = quad_bodies.begin(); it != quad_bodies.end(); it++) {
				Body& body = **it;
				get_collisions_child(body, collisions);
			}
		}

		// Can come before or after earlier checks.
		UL->get_collisions(collisions);
		UR->get_collisions(collisions);
		LL->get_collisions(collisions);
		LR->get_collisions(collisions);

	}
}

std::vector<Collision> QuadTree::get_collisions() const
{
	std::vector<Collision> collisions;

	if (is_leaf()) {
		if (!quad_bodies.empty()) {
			for (auto it = quad_bodies.begin(); it != quad_bodies.end() - 1; it++) {
				Body& body = **it;
				get_collisions_internal(body, it + 1, quad_bodies.end(), collisions);
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
				get_collisions_internal(body, it + 1, quad_bodies.end(), collisions);
			}

			// Now, do a collision check on each body with the bodies of relevant child nodes.
			for (auto it = quad_bodies.begin(); it != quad_bodies.end(); it++) {
				Body& body = **it;
				get_collisions_child(body, collisions);
			}

		}

		// Can come before or after earlier checks.
		UL->get_collisions(collisions);
		UR->get_collisions(collisions);
		LL->get_collisions(collisions);
		LR->get_collisions(collisions);

	}

	return collisions;
}

void QuadTree::get_collisions_child(Body& checking, std::vector<Collision>& collisions) const
{
	/* 
	* 
	* For each child quad which contains partially the body:
	*	handle_collision body with all in that node
	* 
	*/

	// Get a list of all our child quads (max depth) that at least partially contain the body.
	auto partially_in_child = [&checking](const QuadTree& quad) { return quad.contains_partially(checking); };
	std::vector<QuadTree*> to_check = get_quads(partially_in_child);

	// Check for collisions between the given body and the bodies of relevant child nodes.
	for (QuadTree* quad : to_check) {
		// get_collisions_internal can be static, and renamed.
		quad->get_collisions_internal(checking, quad->quad_bodies.begin(), quad->quad_bodies.end(), collisions);

		// can remove if use get_all_quads instead.
		if (!quad->is_leaf()) {
			quad->get_collisions_child(checking, collisions);
		}

	}

}

void QuadTree::get_collisions_internal(Body& checking, std::vector<Body*>::const_iterator it, std::vector<Body*>::const_iterator end, std::vector<Collision>& collisions) const
{
	while (it != end) {
		Body& body2 = **it;

		if (Physics::have_collided(checking, body2)) {
			collisions.emplace_back(Body::get_sorted_pair(checking, body2));
		}
		//Physics::handle_collision(checking, body2);

		it++;
	}
}

bool QuadTree::in_more_than_one_child(const Body& body) const
{
	// Get a list of our child quads that at least partially contain the body.
	std::vector<QuadTree*> contained_in = get_quads([&body](const QuadTree& quad) { return quad.contains_partially(body); });

	// if in more than one child quad, return true.
	return contained_in.size() > 1;
}

void QuadTree::selective_add(Body& new_body)
{
	// This = parent node.
	// 
	// Add to a child if body fully contained in it, else add to self.

	if (in_more_than_one_child(new_body)) {
		quad_bodies.push_back(&new_body);
	}
	else { // is only in one child node.
		add_to_child(new_body); // adds to the child that contains the body fully.
	}
}

void QuadTree::add_body(Body& new_body)
{
	cur_size++;

	if (is_leaf()) {
		if (is_full()) {
			split();
			selective_add(new_body);
		}
		else {
			quad_bodies.push_back(&new_body);
		}
	}
	else {
		selective_add(new_body);
	}
}

void QuadTree::rem_body(const Body& body)
{
	// Could be an outside command, so body could be in one of our child nodes.
	QuadTree& quad = find_quad(body);
	quad.rem_body_internal(body);
}

void QuadTree::rem_body_internal(const Body& body)
{
	auto it = std::find(quad_bodies.begin(), quad_bodies.end(), &body);
	quad_bodies.erase(it);
	notify_child_removed();

	if (!is_leaf() and has_room()) {
		concatenate();
	}
}

Body* QuadTree::find_body(Vector2 point) const
{
	// Because bodies can be in branch nodes too, can't just go to the leaf node and search.
	auto found = std::find_if(quad_bodies.begin(), quad_bodies.end(), [point](const Body* body) { return body->contains_point(point); });

	if (found != quad_bodies.end()) {
		return *found;
	} else if (!is_leaf()) {
		// Find the quad that the (x,y) point should be in, then recurse into it.
		QuadTree* quad = get_quad([point](const QuadTree& quad) { return quad.contains_point(point); });
		return quad->find_body(point);
	}

	return nullptr;
}

const std::array<QuadTree*, 4> QuadTree::get_quads() const
{
	return { UL.get(), UR.get(), LL.get(), LR.get() };
}

void QuadTree::update()
{
	// After body position update tick, update the quad with new positions.
	if (is_leaf()) {
		for (auto it = quad_bodies.begin(); it != quad_bodies.end();) {
			Body& body = **it;

			if (contains_fully(body)) { // still fully contains this body, no changes needed.
				it++;
			}
			else { // body no longer completely inside this leaf node.
				it = quad_bodies.erase(it);
				cur_size--;
				parent->reinsert(body);
			}

		}

	}
	else {
		int to_check = quad_bodies.size();

		UL->update();
		UR->update();
		LL->update();
		LR->update();

		// only check bodies which child nodes have not just reinserted upwards to this node.
		auto it = quad_bodies.begin();
		while (to_check != 0) {
			Body& body = **it;

			if (contains_fully(body)) {
				// move to child node if body is fully contained by it.
				// TODO look into move_to_child(iterator)
				
				// Get the child quad that fully contains the body, if any do.
				QuadTree* contained_in = get_quad([&body](const QuadTree& quad) { return quad.contains_fully(body); });

				// If the body is fully contained in a child quad, move it into that quad.
				if (contained_in) {
					contained_in->add_body(body);
					it = quad_bodies.erase(it);
				}
				else { // No child quad fully contains the body, so keep it in this node.
					it++;
				}


			}
			else { // body no longer completely inside this node.
				it = quad_bodies.erase(it);
				cur_size--;
				parent->reinsert(body);
			}

			to_check--;

		}

		// Child methods may have called reinsert on us.
		if (has_room()) {
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

void QuadTree::move_to_child(std::vector<Body*>::iterator& it)
{
	add_to_child(**it);

	// Body moved to child node, so no need to decrease our size.
	it = quad_bodies.erase(it);
}

void QuadTree::add_to_child(Body& new_body)
{
	// Get the child quad that fully contains the body, if any do.
	QuadTree* contained_in = get_quad([&new_body](const QuadTree& quad) { return quad.contains_fully(new_body); });

	// if any child quad fully contains the body, add it to the quad.
	if (contained_in) {
		contained_in->add_body(new_body);
	}
	else {
		std::cout << "add_to_child added nothing to nowhere !!!" << '\n';
		std::cout << "\tBody ID: " << new_body.id << '\n';
		std::cout << "\tBody Position: (" << new_body.x << ", " << new_body.y << ")\n";
		std::cout << "\tQuad Dimensions:\n";
		std::cout << "\t\tx:\t" << dimensions.x << '\n';
		std::cout << "\t\ty:\t" << dimensions.y << '\n';
		std::cout << "\t\twidth:\t" << dimensions.width << '\n';
		std::cout << "\t\theight:\t" << dimensions.height << '\n';

	}
}


QuadTree* QuadTree::get_quad(std::function<bool(const QuadTree&)> predicate) const
{
	if (predicate(*UL)) {
		return UL.get();
	}
	else if (predicate(*UR)) {
		return UR.get();
	}
	else if (predicate(*LL)) {
		return LL.get();
	}
	else if (predicate(*LR)) {
		return LR.get();
	}

	return nullptr;
}

std::vector<QuadTree*> QuadTree::get_quads(std::function<bool(const QuadTree&)> predicate) const
{
	std::vector<QuadTree*> quads;
	quads.reserve(4);

	if (predicate(*UL)) {
		quads.push_back(UL.get());
	}

	if (predicate(*UR)) {
		quads.push_back(UR.get());
	}

	if (predicate(*LL)) {
		quads.push_back(LL.get());
	}

	if (predicate(*LR)) {
		quads.push_back(LR.get());
	}

	return quads;
}

/*
void QuadTree::rem_from_child(const Body& body) // no longer used in our update, but may be useful later if allow user to delete a planet.
{
	// Get the child quad that fully contains the body, if any do.
	QuadTree* contained_in = get_quad([&body](const QuadTree& quad) { return quad.contains_fully(body); });

	if (contained_in)) {
		contained_in->rem_body(body);
	}
}*/


std::vector<QuadTree*> QuadTree::get_all_quads(std::function<bool(const QuadTree&)> predicate) const
{
	std::vector<QuadTree*> quads = get_quads(predicate);

	int cur_index = 0;
	while (cur_index < quads.size()) {
		QuadTree* quad = quads[cur_index];

		if (!quad->is_leaf()) {
			std::vector<QuadTree*> next_level = quad->get_quads(predicate);
			quads.insert(quads.end(), next_level.begin(), next_level.end());
		}

		cur_index++;
	}

	return quads;
}

void QuadTree::concatenate()
{
	// copy all body pointers from a quad, without erasing them in that quad.
	auto copy_bodies = [](const QuadTree& quad, std::vector<Body*>& copy_to) {
		for (Body* body : quad.quad_bodies) {
			copy_to.push_back(body);
		}
	};

	copy_bodies(*UL, quad_bodies);
	copy_bodies(*UR, quad_bodies);
	copy_bodies(*LL, quad_bodies);
	copy_bodies(*LR, quad_bodies);

	UL.reset();
	UR.reset();
	LL.reset();
	LR.reset();
}

void QuadTree::split()
{
	float x = dimensions.x;
	float y = dimensions.y;
	float mid_x = dimensions.x + dimensions.width / 2.0f;
	float mid_y = dimensions.y + dimensions.height / 2.0f;

	float size = dimensions.width / 2.0f;

	UL = std::make_unique<QuadTree>(x, y, size);
	UR = std::make_unique<QuadTree>(mid_x, y, size);
	LL = std::make_unique<QuadTree>(x, mid_y, size);
	LR = std::make_unique<QuadTree>(mid_x, mid_y, size);

	UL->parent = this;
	UR->parent = this;
	LL->parent = this;
	LR->parent = this;

	// add bodies to respective quads

	auto it = quad_bodies.begin();
	while (it < quad_bodies.end()) { //&& ref
		Body& body = **it;

		if (in_more_than_one_child(body)) {
			// stays in parent, since it is not unique to any child node.
			it++;
		}
		else {
			// move body to child.
			move_to_child(it);
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
	}
	else { // not fully contained in this node, and this node is not the root (parent != nullptr)
		// Not fully contained in this node, needs to continue moving upwards.
		cur_size--; // was previously in a child node, and now is moving to our parent node.
		parent->reinsert(body);
	}
}

void QuadTree::get_representation_internal(std::vector<Rectangle>& rep) const {
	if (is_leaf()) {
		rep.push_back(dimensions);
	}
	else {
		UL->get_representation_internal(rep);
		UR->get_representation_internal(rep);
		LL->get_representation_internal(rep);
		LR->get_representation_internal(rep);
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
	QuadTree* contained_in = get_quad([&body](const QuadTree& quad) { return quad.contains_fully(body); });


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
	QuadTree* contained_in = get_quad([&body](const QuadTree& quad) { return quad.contains_fully(body); });


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
