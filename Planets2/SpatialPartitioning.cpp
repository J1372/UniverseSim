#include "SpatialPartitioning.h"
#include "Collision.h"

std::vector<Collision> SpatialPartitioning::get_collisions()
{
	num_collision_checks_tick = 0;
	return get_collisions_impl();
}