#include "QuadTree.h"
#include <iostream>

void QuadTree::collision_check(std::vector<Body*>& to_remove)
{
	if (is_leaf()) {
		std::vector<Body*>::iterator it = quad_bodies.begin();
		while (it != quad_bodies.end()) {
			handle_collision(it, to_remove);
		}
	}
	else {
		int prev_size = to_remove.size();

		UL->collision_check(to_remove);
		UR->collision_check(to_remove);
		LL->collision_check(to_remove);
		LR->collision_check(to_remove);

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

int QuadTree::update_internal()
{
	return 0;
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

bool QuadTree::in_more_than_one_child(Body& body)
{
	int num_children = 0;

	if (UL->contains_partially(body)) {
		num_children++;
	}

	if (UR->contains_partially(body)) {
		num_children++;
	}

	if (LL->contains_partially(body)) {
		num_children++;
	}

	if (LL->contains_partially(body)) {
		num_children++;
	}

	return num_children > 1;
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
	cur_size--;

	if (!is_leaf()) {

		if (has_room()) {
			concatenate();
			return true;
		}
	}

	return false;
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
				cur_size--;  // TODO look into using rem_body ?compatibly alongside iterators. or having rem_body(iterator)
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
				if (UL->contains_fully(body)) {
					UL->add_body(body);
					it = quad_bodies.erase(it);
				}
				else if (UR->contains_fully(body)) {
					UR->add_body(body);
					it = quad_bodies.erase(it);
				}
				else if (LL->contains_fully(body)) {
					LL->add_body(body);
					it = quad_bodies.erase(it);
				}
				else if (LR->contains_fully(body)) {
					LR->add_body(body);
					it = quad_bodies.erase(it);
				}
				else {
					it++;
				}


			}
			else { // body no longer completely inside this node.
				it = quad_bodies.erase(it);
				cur_size--; // TODO look into using rem_body ?compatibly alongside iterators. or having rem_body(iterator)
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
	return body.top().y >= y and body.bottom().y < end_y and
		body.left().x >= x and body.right().x < end_x;
}

bool QuadTree::contains_partially(const Body& body) const
{
	// True if circle intersects with quad.
	float dist_x = std::abs(body.x - x + width() / 2);
	float dist_y = std::abs(body.y - y + height() / 2);

	if (dist_x > (width() / 2 + body.radius)) { return false; }
	if (dist_y > (height() / 2 + body.radius)) { return false; }

	if (dist_x <= (width() / 2)) { return true; }
	if (dist_y <= (height() / 2)) { return true; }

	int corner_dist= std::pow((dist_x - width() / 2), 2) + std::pow((dist_y - height() / 2), 2);

	return corner_dist <= std::pow(body.radius, 2);
}

void QuadTree::move_to_parent(Body& body)
{
	rem_body(body);
	parent->quad_bodies.push_back(&body);
}

void QuadTree::move_to_child(Body& body)
{
	quad_bodies.erase(std::find(quad_bodies.begin(), quad_bodies.end(), &body));
	add_to_child(body);
}

void QuadTree::add_to_child(Body& new_body)
{
	if (UL->contains_fully(new_body)) {
		UL->add_body(new_body);
	}
	else if (UR->contains_fully(new_body)) {
		UR->add_body(new_body);
	}
	else if (LL->contains_fully(new_body)) {
		LL->add_body(new_body);
	}
	else if (LR->contains_fully(new_body)) {
		LR->add_body(new_body);
	}
	else {
		std::cout << "add_to_child added nothing to nowhere !!!" << '\n';
		std::cout << "\tBody Position: (" << new_body.x << ", " << new_body.y << ")\n";
		std::cout << "\tQuad Dimensions:\n";
		std::cout << "\t\tx:\t" << x << '\n';
		std::cout << "\t\ty:\t" << y << '\n';
		std::cout << "\t\tend_x:\t" << end_x << '\n';
		std::cout << "\t\tend_y:\t" << end_y << '\n';

	}
}

/*
void QuadTree::rem_from_child(const Body& body) // no longer used in our update, but may be useful later if allow user to delete a planet.
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

	for (auto it = quad_bodies.begin(); it != quad_bodies.end(); ++it) { //&& ref
		Body& body = **it;

		if (in_more_than_one_child(body)) {
			// stays in parent, since it is not unique to any child node.
		}
		else { // move body to child.
			move_to_child(body);
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
	else { // Not fully contained in this node, needs to continue moving upwards.
		cur_size--; // was previously in a child node, and now is moving to our parent node.
		parent->reinsert(body);
	}
}


void QuadTree::draw_debug(const Camera2D &camera) const {

	if (is_leaf()) {
		Rectangle rec{ x, y, get_width(), get_height() };

		DrawRectangleLinesEx(rec, 50, RAYWHITE);
	}
	else {
		UL->draw_debug(camera);
		UR->draw_debug(camera);
		LL->draw_debug(camera);
		LR->draw_debug(camera);
	}
	
}