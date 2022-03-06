#include "QuadTree.h"
#include <iostream>

void QuadTree::perform_collision_check(std::vector<int>& to_remove)
{
	if (is_leaf()) {
		std::unordered_map<int, Body*>::iterator it = quad_bodies.begin();
		//bool curr_eaten = false;
		while (it != quad_bodies.end()) {
			handle_collision(it, to_remove);
		}
	}
	else {
		UL->perform_collision_check(to_remove);
		UR->perform_collision_check(to_remove);
		LL->perform_collision_check(to_remove);
		LR->perform_collision_check(to_remove);
	}
}

void QuadTree::handle_collision(std::unordered_map<int, Body*>::iterator& it, std::vector<int>& to_remove)
{
	Body& body1 = *it->second;
	auto it2 = std::next(it, 1);
	while (it2 != quad_bodies.end()) {
		////std::cout << "\t\tCollision check against " << it2->first << "\n\n";
		Body& body2 = *it2->second;

		if (body1.check_col(body2)) { // there is a collision
			////std::cout << "\t\t\tCollision!" << '\n';
			////std::cout << "\t\t\t" << body1.x << ',' << body1.y << ',' << body1.radius << '\n';
			////std::cout << "\t\t\t" << body2.x << ',' << body2.y << ',' << body2.radius << '\n';

			if (body1.can_eat(body2)) { // it1 eats it2
				body1.absorb(body2);

				to_remove.push_back(body2.id);

				it2 = quad_bodies.erase(it2); // move to next check
			}
			else { // it2 eats it1
				body2.absorb(body1);

				to_remove.push_back(body1.id);

				it = quad_bodies.erase(it); // it1 no longer exists, no more checks on other it2s.
				return;
			}
		}
		else { // no collision. move to next check.
			++it2;
		}

	}
	++it;
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
	quad_bodies.erase(body.id);

	if (!is_leaf()) {

		if (quad_bodies.size() < MAX_BODIES) {
			concatenate();
			return true;
		}
		else {
			rem_from_child(body);
		}
	}

	return false;
}

void QuadTree::get_quads(const QuadTree* (&ret)[4]) const
{
	ret[0] = UL.get();
	ret[1] = UR.get();
	ret[2] = LL.get();
	ret[3] = LR.get();

}

void QuadTree::update_pos()
{
	if (is_leaf()) {

		// need to make copies of our parent and our bodies so that if reinsert concatenates our quad,
		// we can still call reinsert to parent for all our bodies.
		QuadTree* parent_copy = parent;

		int x = this->x;
		int y = this->y;
		int end_x = this->end_x;
		int end_y = this->end_y;

		std::vector<Body*> check_bodies;
		check_bodies.reserve(quad_bodies.size());

		for (auto it = quad_bodies.begin(); it != quad_bodies.end(); ++it) {
			Body& body = *it->second;

			body.pos_update();

			check_bodies.push_back(&body);

		}

		bool still_exist = true;
		for (int i = 0; i < check_bodies.size(); ++i) {
			Body& body = *check_bodies[i];

			// check if body moved to neighbor quad same parent or totally different quad.
			if (still_exist) {
				if (!in_bounds(body.x, body.y)) {
					quad_bodies.erase(body.id);
					still_exist = !parent->reinsert(body);
				}
			}
			else {

			}
		}




	}
	else {
		UL->update_pos();
		UR->update_pos();
		LL->update_pos();
		LR->update_pos();
	}
}

int QuadTree::get_total_size() const  // this doesn't make sense, quad_bodies.size() includes the others
{
	int size = quad_bodies.size();

	if (!is_leaf()) {
		size += UL->get_total_size();
		size += UR->get_total_size();
		size += LL->get_total_size();
		size += LR->get_total_size();
	}

	return size;
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
		Body* body = it->second;

		add_to_child(*body);
	}
}

bool QuadTree::reinsert(Body& body)
{
	if (in_bounds(body.x, body.y)) {
		// no rem_body or rem_child, sub nodes already took care of that (they know it's not in their quad).
		// don't have to remove from mine, because it is still in my quad.
		add_to_child(body); // add to correct quad.
		return false; // no body removed from my quads, no concatenation.
	}
	else {
		quad_bodies.erase(body.id);
		if (is_root()) {
			// expand and add_to_child()
			// expand(body);
			add_to_child(body); // don't need to add_body, it is already in my quad.
		}
		else {
			parent->reinsert(body);

			if (has_room()) {
				concatenate();
				return true; // will stop updating positions and checking if reinserts needed.
			}
		}
	}
	return false;
}

void QuadTree::expand(Body& out_of_bounds_body)
{
	if (out_of_bounds_body.x < x) { // to the left, (determing if curr quad copy will be LR or UR)

	}

	if (out_of_bounds_body.y < y) { // to the top, determine if curr quad copy will be LL or LR

	}
}
