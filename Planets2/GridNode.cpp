#include "GridNode.h"
#include "Body.h"
#include "Physics.h"
#include "Collision.h"

GridNode::GridNode(float x, float y, float node_size, int id) : dimensions{ x, y, node_size, node_size }, id(id)
{}

void GridNode::add(Body& body)
{
    bodies.push_back(&body);
}

void GridNode::rem(const Body& body)
{
    // Unlike quad tree, we can't be certain that the size of the bodies vector is small. Still, just loop through vector for now.
    auto it = std::find(bodies.begin(), bodies.end(), &body);
    bodies.erase(it);
}

void GridNode::clear()
{
    bodies.clear();
}

bool GridNode::contains_point(Vector2 point) const
{
    return Physics::point_in_rect(point, dimensions);
}

bool GridNode::contains_partially(const Body& body) const
{
    return Physics::body_intersects_rect(body, dimensions);
}

bool GridNode::contains_fully(const Body& body) const
{
    return Physics::body_inside_rect(body, dimensions);
}

Body* GridNode::find_body(Vector2 point) const
{
    for (Body* body : bodies) {
        if (body->contains_point(point)) {
            return body;
        }
    }

    return nullptr;
}

Rectangle GridNode::get_representation() const
{
    return dimensions;
}

void GridNode::attach_debug_text(Body& body) const
{
    body.add_debug_text("Grid id: " + std::to_string(id));
    body.add_debug_text("Grid bodies: " + std::to_string(bodies.size()));
}

void GridNode::get_collisions(std::vector<Collision>& collisions) const
{
    if (bodies.empty()) {
        return;
    }

    for (auto it1 = bodies.begin(); it1 != bodies.end() - 1; it1++) {
        Body& body1 = **it1;
        for (auto it2 = it1 + 1; it2 != bodies.end(); it2++) {
            Body& body2 = **it2;

            if (Physics::have_collided(body1, body2)) {
                Collision collision { Body::get_sorted_pair(body1, body2) };

                auto is_same_collision = [collision](Collision other) {
                    return collision == other;
                };

                if (!std::any_of(collisions.begin(), collisions.end(), is_same_collision)) {
                    collisions.emplace_back(Body::get_sorted_pair(body1, body2));
                }
            }

        }
    }
}
