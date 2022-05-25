#include "Collision.h"
#include "Body.h"

Collision::Collision(std::pair<Body*, Body*> collided) : bigger(*collided.first), smaller(*collided.second)
{}

bool Collision::operator==(const Collision & other) const
{
    return bigger == other.bigger and smaller == other.smaller;
}
