#include "BarnesHut.h"
#include "Body.h"
#include "Physics.h"

float BarnesHut::approximation_value;

void BarnesHut::set_approximation(float to_set)
{
	approximation_value = to_set;
}

BarnesHut::BarnesHut(float size, float set_approximation) :
	dimensions{ -size / 2.0f, -size / 2.0f, size, size }
{
	approximation_value = set_approximation;
}

void BarnesHut::set_size(float size)
{
	dimensions = { -size / 2.0f, -size / 2.0f, size, size };
}

BarnesHut::BarnesHut(float x, float y, float size) :
	dimensions{ x, y, size, size }
{}


float BarnesHut::dist_ratio(const Body& body) const
{
	return dimensions.width / Physics::dist(center_of_mass, { body.x, body.y });
}

bool BarnesHut::sufficiently_far(const Body& body) const
{
	return dist_ratio(body) < approximation_value;
}
void BarnesHut::update_mass_add(const Body& to_add)
{
	/*
	* Need to calculate center of mass change for both x and y coordinates.
	* The method is the same for both coordinates.
	* 
	* A body's mass moment on the x coordinate is: mass * x
	* 
	* Center of mass for x coordinate between bodies :
	* CM_X =
	* 	sum of their moments (m1x1 + m2x2 + ...)
	* 	/
	* 	sum of their mass (m1 + m2 + ...)
	* 
	* currently, our CM_X is :
	* 
	* CM_X =
	*	current_moment_sum
	*	/
	*	current_mass_sum
	* 
	* we are adding the nth body, so new CM_X is
	* 
	* CM_X = 
	*	current_moment_sum + moment_n
	*	/
	*	current_mass_sum + mass_n
	* 
	* The only variable we don't immediately have is current_moment_sum.
	* 
	* But we have our current center of mass(x) and mass sum.
	* So we can derive our current sum of moments.
	* 
	* current_moment_sum = CM_X * current_mass_sum
	* 
	* using that, we can calculate our new center of mass for the x coordinate.
	* 
	* We can do the same thing for the y coordinate.
	* After that, our center of mass is updated.
	*/

	if (is_leaf()) {
		// Leaf nodes mathematically SHOULD be able to be handled without being a special case.
		// But the center of mass calculation sometimes results in minor floating point error,
		// which causes the distance between the center mass and its only node body to be non-zero, and very small.
		// which then causes the net force calculation of that center mass on its own body to be enormous, throwing the body out of bounds.

		// This is only a problem in leaf nodes, parent nodes' center of masses should not have this problem, even with rounding error.

		// This body is the only one affecting this node's center mass, so
		center_of_mass.x = to_add.x;
		center_of_mass.y = to_add.y;

		mass_sum += to_add.mass;
		return;
	}

	Vector2 body_moment = to_add.get_mass_moment();

	// find current moment sum for x and y.
	Vector2 current_moment_sum { center_of_mass.x * mass_sum , center_of_mass.y * mass_sum };

	float combined_mass = mass_sum + to_add.mass;

	center_of_mass.x = (current_moment_sum.x + body_moment.x) / combined_mass;
	center_of_mass.y = (current_moment_sum.y + body_moment.y) / combined_mass;

	mass_sum = combined_mass;

}

void BarnesHut::add_body(Body& to_add)
{
	if (is_leaf()) {
		if (is_empty()) {
			node_body = &to_add;
		}
		else {
			split();
			add_to_child(to_add);
		}
	}
	else {
		// add to one of our children
		add_to_child(to_add);
	}

	update_mass_add(to_add);
}

void BarnesHut::add_to_child(Body& to_add)
{
	// Get the child quad that fully contains the body.
	BarnesHut* contained_in = get_quad([&to_add](const BarnesHut& quad) { return quad.contains(to_add); });

	contained_in->add_body(to_add);
}


BarnesHut* BarnesHut::get_quad(std::function<bool(const BarnesHut&)> predicate) const
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

bool BarnesHut::contains(Body& body) const
{
	return Physics::point_in_rect({ body.x, body.y }, dimensions);
}

void BarnesHut::update(std::span<const std::unique_ptr<Body>> bodies)
{
	concatenate();
	center_of_mass = { 0,0 };
	mass_sum = 0;

	for (const std::unique_ptr<Body>& body : bodies) {
		add_body(*body);
	}

}

void BarnesHut::split()
{
	float x = dimensions.x;
	float y = dimensions.y;
	float mid_x = dimensions.x + dimensions.width / 2.0f;
	float mid_y = dimensions.y + dimensions.height / 2.0f;

	float size = dimensions.width / 2.0f;

	UL = std::make_unique<BarnesHut>(x, y, size);
	UR = std::make_unique<BarnesHut>(mid_x, y, size);
	LL = std::make_unique<BarnesHut>(x, mid_y, size);
	LR = std::make_unique<BarnesHut>(mid_x, mid_y, size);

	UL->parent = this;
	UR->parent = this;
	LL->parent = this;
	LR->parent = this;

	// add current body to correct quad
	add_to_child(*node_body);
	node_body = nullptr;
}

void BarnesHut::concatenate()
{
	UL.reset();
	UR.reset();
	LL.reset();
	LR.reset();
}

void BarnesHut::handle_gravity(Body& body, float grav_const) const
{

	if (is_leaf() or sufficiently_far(body)) {
		// Use center of mass and mass sum as an approximate grav pull.
		// This is an approximation of a grav pull on the body by the group of bodies in child nodes.
		// can add a check to see if mass_sum == 0.
		Physics::grav_pull(body, center_of_mass, mass_sum, grav_const);
	}
	else {
		// Not a leaf, and not sufficiently far away from this body.
		UL->handle_gravity(body, grav_const);
		UR->handle_gravity(body, grav_const);
		LL->handle_gravity(body, grav_const);
		LR->handle_gravity(body, grav_const);
	}

}