#include "BarnesHut.h"
#include "Body.h"
#include "Physics.h"
#include <raymath.h>

DynamicPool<QuadChildren<BarnesHut>> BarnesHut::quad_pool {1000};
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

BarnesHut::BarnesHut(float x, float y, float size) :
	dimensions{ x, y, size, size }
{}

float BarnesHut::dist_ratio(const Body& body) const
{
	return dimensions.width / Physics::dist(center_of_mass, body.pos());
}

bool BarnesHut::sufficiently_far(const Body& body) const
{
	return dist_ratio(body) < approximation_value;
}

void BarnesHut::update_mass_add(Vector2 center, long mass)
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
		center_of_mass = center;
		mass_sum = mass;

		return;
	}

	Vector2 new_moment = Physics::moment(center, mass);

	// find current moment sum for x and y.
	Vector2 current_moment_sum = Physics::moment(center_of_mass, mass_sum);

	long combined_mass = mass_sum + mass;

	center_of_mass.x = (current_moment_sum.x + new_moment.x) / combined_mass;
	center_of_mass.y = (current_moment_sum.y + new_moment.y) / combined_mass;

	mass_sum = combined_mass;

}

void BarnesHut::add_body(Vector2 center, long mass)
{
	if (is_leaf()) {
		if (!is_empty()) {
			split();
			add_to_child(center, mass);
		}
	}
	else {
		// add to one of our children
		add_to_child(center, mass);
	}

	update_mass_add(center, mass);
}

void BarnesHut::add_to_child(Vector2 center, long mass)
{
	// Get the child quad that fully contains the body.
	BarnesHut* contained_in = children->get_quad<&BarnesHut::contains>(center);

	if (contained_in) {
		contained_in->add_body(center, mass);
	}
	else {
		/*
		* This should be rare.
		* 
		* Due to every node only being able to hold one body before splitting,
		* this quadtree reaches a very high depth often.
		* Since its dimensions are defined as a Rectangle (Raylib struct with 4 floats),
		* this can produce significant floating point error. This is exacerbated when the universe's max size
		* is increased.
		* 
		* Changing this quad's dimensions to use 4 doubles may help, though may still be unreliable with
		* increasing universe max size.
		* 
		* Since we only use this quadtree for gravity calculation approximation, it's better
		* to potentially place to_add in a wrong quad (especially since the quads at this depth are very small),
		* than it is to crash because the depth got too high and the quad boxes don't line up perfectly.
		*/

		// Try to place in any empty quad, to avoid splitting even further.
		BarnesHut* empty_quad = children->get_quad<&BarnesHut::is_empty>();
		if (empty_quad) { // empty == is a leaf.
			empty_quad->add_body(center, mass);
		}
		else { // if none are empty, which should be even rarer, just add to the upper left quad.
			children->UL().add_body(center, mass);
		}

	}

}

bool BarnesHut::contains(Vector2 point) const
{
	return Physics::point_in_rect(point, dimensions);
}

bool BarnesHut::is_leaf() const
{
	return children == nullptr;
}

bool BarnesHut::is_empty() const
{
	return mass_sum == 0l;
}

void BarnesHut::update(std::span<const Body> bodies)
{
	concatenate();
	center_of_mass = { 0,0 };
	mass_sum = 0l;

	for (const Body& body : bodies) {
		add_body(body.pos(), body.get_mass());
	}

}

void BarnesHut::split()
{
	float x = dimensions.x;
	float y = dimensions.y;

	children = quad_pool.get(x, y, dimensions.width);

	// add current body to correct quad
	// We are a leaf, and leaves can only have 1 body.
	// Therefore, our com and mass sum are the com and mass of the 1 body.
	add_to_child(center_of_mass, mass_sum);

}

void BarnesHut::concatenate()
{
	children.reset();
}

Vector2 BarnesHut::force_applied_to(const Body& body) const
{
	if (is_leaf())
	{
		// Use center of mass and mass sum as an approximate grav pull.
		// This is an approximation of a grav pull on the body by the group of bodies in child nodes.
		if (!is_empty())
		{
			return body.force_applied_by(center_of_mass, mass_sum);
		}
		else
		{
			return { 0, 0 };
		}
	}
	else if (sufficiently_far(body))
	{
		return body.force_applied_by(center_of_mass, mass_sum);
	}
	else
	{
		// Not a leaf, and not sufficiently far away from this body.
		Vector2 cur_force = { 0, 0 };

		for (BarnesHut& child : *children)
		{
			cur_force = Vector2Add(cur_force, child.force_applied_to(body));
		}

		return cur_force;
	}

}
