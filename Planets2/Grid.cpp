#include "Grid.h"
#include "Body.h"

#include "Collision.h"
#include <algorithm>

Grid::Grid(float grid_size, int nodes_per_row) : grid_size(grid_size), node_size(grid_size / nodes_per_row), nodes_per_row(nodes_per_row)
{
    nodes.reserve(nodes_per_row * nodes_per_row);

    for (int i = 0; i < nodes_per_row * nodes_per_row; i++) {
        int row = i % nodes_per_row;
        int col = i / nodes_per_row;

        float x = col * node_size - grid_size / 2;
        float y = row * node_size - grid_size / 2;

        nodes.emplace_back(x, y, node_size, i);
    }
}

void Grid::notify_move(const Body* from, Body* to)
{
    std::vector<GridNode*> nodes = get_all_nodes(*from);

    for (GridNode* node : nodes) {
        node->notify_move(from, to);
    }

    // may be better to not store bodies vector.
    auto it = std::find(bodies.begin(), bodies.end(), from);
    *it = to;
}

int Grid::get_index(int pos) const
{
    return std::clamp(static_cast<int>((pos + grid_size / 2) / node_size), 0, nodes_per_row - 1);
}

GridNode& Grid::get_node(Vector2 pos)
{
    int row = get_index(pos.y);
    int col = get_index(pos.x);

    int grid_num = row * nodes_per_row + col;

    return nodes[grid_num];

}

const GridNode& Grid::get_node(Vector2 pos) const
{
    int row = get_index(pos.y);
    int col = get_index(pos.x);

    int grid_num = row * nodes_per_row + col;

    return nodes[grid_num];
}

std::vector<GridNode*> Grid::get_all_nodes(const Body& body)
{
    std::vector<GridNode*> in_nodes;

    int first_row = get_index(body.top());
    int first_col = get_index(body.left());

    int last_row = get_index(body.bottom());
    int last_col = get_index(body.right());

    for (int i = first_row; i <= last_row; i++)
    {
        for (int j = first_col; j <= last_col; j++)
        {
            int grid_num = i * nodes_per_row + j;
            in_nodes.push_back(&nodes[grid_num]);
        }
    }

    return in_nodes;
}

void Grid::add_body(Body& body)
{
    bodies.push_back(&body);
    std::vector<GridNode*> nodes = get_all_nodes(body);

    for (GridNode* node : nodes) {
        node->add(body);
    }
}

void Grid::update()
{
    for (GridNode& node : nodes) {
        node.clear();
    }

    for (Body* body : bodies) {
        std::vector<GridNode*> nodes = get_all_nodes(*body);

        for (GridNode* node : nodes) {
            node->add(*body);
        }
    }

}

void Grid::rem_body(const Body& body)
{
    auto it = std::find(bodies.begin(), bodies.end(), &body);
    bodies.erase(it);

    std::vector<GridNode*> nodes = get_all_nodes(body);
    for (GridNode* node : nodes) {
        node->rem(body);
    }
}

Body* Grid::find_body(Vector2 point) const
{
    const GridNode& node = get_node(point);
    return node.find_body(point);
}

std::vector<Rectangle> Grid::get_representation() const
{
    std::vector<Rectangle> rep;
    rep.reserve(nodes.size());

    for (const GridNode& node : nodes) {
        rep.push_back(node.get_representation());
    }

    return rep;
}

void Grid::get_info(const Body& body, DebugInfo& info) const
{
    const GridNode& node = get_node(body.pos());
    node.get_info(body, info);

    // No general grid debug text to attach. Just node-specific.
}

std::vector<Collision> Grid::get_collisions_impl()
{
    std::vector<Collision> collisions;

    for (const GridNode& node : nodes) {
        num_collision_checks_tick += node.get_collisions(collisions);
    }

    return collisions;
}
