#include "QuadTree.h"
#include <iostream>
#include <string>

int QuadTree::quads_generated = 0;

void QuadTree::collision_check(std::vector<Body*>& to_remove)
{
	if (is_leaf()) {
		std::vector<Body*>::iterator it = quad_bodies.begin();
		while (it != quad_bodies.end()) {
			if (!handle_collision(it, it + 1, *this, *this, to_remove)) {
				it++;
			}
		}
	}
	else {

		UL->collision_check(to_remove);
		UR->collision_check(to_remove);
		LL->collision_check(to_remove);
		LR->collision_check(to_remove);


		/* Now we need to do a collision check on our bodies.
		* 
		* This is different from a collision check in a leaf node.
		* Our bodies can collide with each other, and with bodies in our child nodes.
		* 
		* Specifically the child nodes that contains_partially(our_body)
		* This involves recursion downwards.
		*/ 

		// First, collision check with our node's bodies.
		// If a collision occurs internally, this reduces the number of collision checks needed with child nodes.
		//		which has the possibility of being more expensive.
		std::vector<Body*>::iterator it = quad_bodies.begin();
		while (it != quad_bodies.end()) {
			if (!handle_collision(it, it + 1, *this, *this, to_remove)) {
				it++;
			}
		}

		// of the remaining bodies, do a collision check on each body with the bodies of relevant child nodes.
		it = quad_bodies.begin();
		while (it != quad_bodies.end()) {
			if (!handle_collision_child(it, *this, to_remove)) {
				it++;
			}
		}

		// Child methods may have removed objects.
		if (has_room()) {
			concatenate();
		}
	}
}

bool QuadTree::handle_collision_child(std::vector<Body*>::iterator& it, QuadTree& original_quad, std::vector<Body*>& to_remove) {
	Body& body = **it;

	/* 
	* 
	* For each child quad which contains partially the body:
	*	handle_collision body with all in that node
	*	if body still living, and the node is not a leaf, recurse to that node with handle_collision_child.
	* 
	*/

	// Get a list of our child quads that at least partially contain the body.
	std::vector<QuadTree*> to_check = get_quads([&body](const QuadTree& quad) { return quad.contains_partially(body); });

	// Check collisions on each valid quad. Returns true if the body being checked was absorbed by another.
	for (QuadTree* quad : to_check) {
		if (handle_collision(it, quad->quad_bodies.begin(), original_quad, *quad, to_remove)) {
			return true;
		}

		if (!quad->is_leaf()) {
			if (quad->handle_collision_child(it, original_quad, to_remove)) {
				return true;
			}
		}
	}

	// The original body still exists, so return false.
	return false;

}

bool QuadTree::handle_collision(std::vector<Body*>::iterator& it, std::vector<Body*>::iterator&& it2,
	QuadTree& quad1, QuadTree& quad2, std::vector<Body*>& to_remove)
{
	Body& body1 = **it;
	while (it2 != quad2.quad_bodies.end()) {
		Body& body2 = **it2;

		if (body1.check_col(body2)) { 

			if (body1.can_eat(body2)) { // it1 eats it2
				body1.absorb(body2);

				to_remove.push_back(&body2);

				it2 = quad2.rem_body(it2);
			}
			else { // it2 eats it1
				body2.absorb(body1);

				to_remove.push_back(&body1);

				it = quad1.rem_body(it); // it1 no longer exists, no more checks on other it2s.
				return true;
			}
		}
		else { // no collision. move to next check.
			it2++;
		}

	}

	return false;

}

bool QuadTree::in_more_than_one_child(Body& body)
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

bool QuadTree::rem_body(const Body& body)
{
	quad_bodies.erase(std::find(quad_bodies.begin(), quad_bodies.end(), &body));
	notify_child_removed();

	if (!is_leaf()) {

		if (has_room() and !in_coll_check) {
			concatenate();
			return true;
		}
	}

	return false;
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
	return point.x >= x and point.x < end_x and
		point.y >= y and point.y < end_y;
}

bool QuadTree::contains_fully(const Body& body) const
{
	// True if no part of the circle goes outside the quad.
	return body.top().y >= y and body.bottom().y <= end_y and
		body.left().x >= x and body.right().x <= end_x;
}

bool QuadTree::contains_partially(const Body& body) const
{
	// True if circle intersects with quad.
	float dist_x = std::abs(body.x - x - width() / 2);
	float dist_y = std::abs(body.y - y - height() / 2);

	if (dist_x > (width() / 2 + body.radius)) { return false; }
	if (dist_y > (height() / 2 + body.radius)) { return false; }

	if (dist_x <= (width() / 2)) { return true; }
	if (dist_y <= (height() / 2)) { return true; }

	int corner_dist= std::pow((dist_x - width() / 2), 2) + std::pow((dist_y - height() / 2), 2);

	return corner_dist <= std::pow(body.radius, 2);
}

std::vector<Body*>::iterator QuadTree::rem_body(std::vector<Body*>::iterator it)
{
	auto next_it = quad_bodies.erase(it);
	notify_child_removed();

	if (!is_leaf()) {

		if (has_room()) {
			// Concatenation won't result in vector resizing / iterator invalidation since new size < maximum quad size.
			concatenate();
		}
	}

	return next_it;
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
		std::cout << "\t\tx:\t" << x << '\n';
		std::cout << "\t\ty:\t" << y << '\n';
		std::cout << "\t\tend_x:\t" << end_x << '\n';
		std::cout << "\t\tend_y:\t" << end_y << '\n';

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
	float mid_x = (x + end_x) / 2;
	float mid_y = (y + end_y) / 2;

	UL = std::make_unique<QuadTree>(x, y, mid_x, mid_y);
	UR = std::make_unique<QuadTree>(mid_x, y, end_x, mid_y);
	LL = std::make_unique<QuadTree>(x, mid_y, mid_x, end_y);
	LR = std::make_unique<QuadTree>(mid_x, mid_y, end_x, end_y);

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
		rep.push_back(representation);
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
