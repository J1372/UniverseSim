#include "QuadTree.h"
#include <iostream>

void QuadTree::perform_collision_check(std::vector<Body*>& to_remove)
{
	if (is_leaf()) {
		std::vector<Body*>::iterator it = quad_bodies.begin();
		while (it != quad_bodies.end()) {
			handle_collision(it, to_remove);
		}
	}
	else {
		int prev_size = to_remove.size();

		UL->perform_collision_check(to_remove);
		UR->perform_collision_check(to_remove);
		LL->perform_collision_check(to_remove);
		LR->perform_collision_check(to_remove);

		int children_removed = to_remove.size() - prev_size;

		for (int i = prev_size; i < to_remove.size(); i++) {
			Body* body = to_remove[i];

			quad_bodies.erase(std::find(quad_bodies.begin(), quad_bodies.end(), body));
		}

		// Child methods may have removed objects.
		if (has_room()) {
			concatenate();
		}
	}
}

void QuadTree::handle_collision(std::vector<Body*>::iterator& it, std::vector<Body*>& to_remove)
{
	Body& body1 = **it;
	auto it2 = std::next(it, 1);
	while (it2 != quad_bodies.end()) {
		////std::cout << "\t\tCollision check against " << it2->first << "\n\n";
		Body& body2 = **it2;

		if (body1.check_col(body2)) { // there is a collision
			////std::cout << "\t\t\tCollision!" << '\n';
			////std::cout << "\t\t\t" << body1.x << ',' << body1.y << ',' << body1.radius << '\n';
			////std::cout << "\t\t\t" << body2.x << ',' << body2.y << ',' << body2.radius << '\n';

			if (body1.can_eat(body2)) { // it1 eats it2
				body1.absorb(body2);

				to_remove.push_back(&body2);

				it2 = quad_bodies.erase(it2); // move to next check
			}
			else { // it2 eats it1
				body2.absorb(body1);

				to_remove.push_back(&body1);

				it = quad_bodies.erase(it); // it1 no longer exists, no more checks on other it2s.
				return;
			}
		}
		else { // no collision. move to next check.
			it2++;
		}

	}
	it++;
}

void QuadTree::add_body(Body& new_body)
{
	if (is_leaf()) {
		if (is_full()) {
			// split then add like non_leaf
			/*std::cout << "Splitting" << '\n';

			std::cout << "Body.x: " << new_body->x << '\n';
			std::cout << "Body.y: " << new_body->y << "\n\n";

			std::cout << "Quad.x: " << x << '\n';
			std::cout << "Quad.y: " << y << '\n';
			std::cout << "Quad.end_x: " << end_x << '\n';
			std::cout << "Quad.end_y: " << end_y << '\n';
			*/

			split();
			add_to_child(new_body);
		}

		// add to self

		quad_bodies[new_body.id] = &new_body;
	}
	else { // has 4 quads, add to 1 of them
		quad_bodies[new_body.id] = &new_body;

		add_to_child(new_body);

	}
}

bool QuadTree::rem_body(const Body& body)
{
	quad_bodies.erase(std::find(quad_bodies.begin(), quad_bodies.end(), &body));

	if (!is_leaf()) {

		if (has_room()) {
			concatenate();
			return true;
		}
		else {
			rem_from_child(body);
		}
	}

	return false;
}

const std::array<QuadTree*, 4> QuadTree::get_quads() const
{
	return { UL.get(), UR.get(), LL.get(), LR.get() };
}

void QuadTree::update_pos(float wraparound_val)
{
	if (is_leaf()) {
		for (auto it = quad_bodies.begin(); it != quad_bodies.end();) {
			Body& body = **it;

			body.pos_update(wraparound_val); // Want to do this on all bodies. can move this out into Universe.

			if (in_bounds(body.x, body.y)) {
				it++;
			}
			else {
				it = quad_bodies.erase(it);
				parent->reinsert(body);
			}

		}

	}
	else {
		UL->update_pos(wraparound_val);
		UR->update_pos(wraparound_val);
		LL->update_pos(wraparound_val);
		LR->update_pos(wraparound_val);

		// Child methods may have called reinsert on us.
		if (has_room()) {
			concatenate();
		}
	}
}

void QuadTree::add_to_child(Body& new_body)
{
	if (UL->in_bounds(new_body.x, new_body.y)) {
		UL->add_body(new_body);
	}
	else if (UR->in_bounds(new_body.x, new_body.y)) {
		UR->add_body(new_body);
	}
	else if (LL->in_bounds(new_body.x, new_body.y)) {
		LL->add_body(new_body);
	}
	else if (LR->in_bounds(new_body.x, new_body.y)) { // LR
		LR->add_body(new_body);
	}
	else {
		std::cout << "add_to_child added nothing to nowhere !!!" << '\n';
	}
}

void QuadTree::rem_from_child(const Body& body)
{
	if (UL->in_bounds(body.x, body.y)) {
		UL->rem_body(body);
	}
	else if (UR->in_bounds(body.x, body.y)) {
		UR->rem_body(body);
	}
	else if (LL->in_bounds(body.x, body.y)) {
		LL->rem_body(body);
	}
	else if (LR->in_bounds(body.x, body.y)) { // LR
		LR->rem_body(body);
	}
}

void QuadTree::concatenate()
{
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

	for (auto it = quad_bodies.begin(); it != quad_bodies.end(); ++it) { //&& ref
		Body& body = **it;

		add_to_child(body);
	}
}

void QuadTree::reinsert(Body& body)
{
	if (in_bounds(body.x, body.y)) {
		// no rem_body or rem_child, sub nodes already took care of that (they know it's not in their quad).
		// don't have to remove from mine, because it is still in my quad.
		add_to_child(body); // add to correct quad.
	}
	else {
		quad_bodies.erase(std::find(quad_bodies.begin(), quad_bodies.end(), &body));
		if (is_root()) {
			// expand and add_to_child()
			// expand(body);
			add_to_child(body); // don't need to add_body, it is already in my quad.
		}
		else {
			parent->reinsert(body);
		}
	}
}
