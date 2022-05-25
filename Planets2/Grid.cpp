#include "Grid.h"
#include "Body.h"

#include "Collision.h"

Grid::Grid(int grid_size, int nodes_per_row) : grid_size(grid_size), node_size(grid_size / nodes_per_row), nodes_per_row(nodes_per_row)
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

int Grid::get_index(int pos) const
{
    return (pos + grid_size / 2) / node_size;
}

GridNode& Grid::get_node(int x, float y)
{
    int row = get_index(y);
    int col = get_index(x);

    int grid_num = row * nodes_per_row + col;

    return nodes[grid_num];

}

const GridNode& Grid::get_node(int x, float y) const
{
    int row = get_index(y);
    int col = get_index(x);

    int grid_num = row * nodes_per_row + col;

    return nodes[grid_num];
}

std::vector<GridNode*> Grid::get_all_nodes(const Body& body)
{
    std::vector<GridNode*> in_nodes;

    Rectangle dimensions = body.get_bounding_box();


    int first_row = get_index(dimensions.y);
    int first_col = get_index(dimensions.x);

    int last_row = get_index(dimensions.y + dimensions.height);
    int last_col = get_index(dimensions.x + dimensions.width);


    for (int i = first_row; i <= last_row; i++) {
        for (int j = first_col; j <= last_col; j++) {

            int grid_num = i * nodes_per_row + j;
            in_nodes.push_back(&nodes[grid_num]);
        }
    }


    return in_nodes;

}

void Grid::add_body(Body& body)
{
    bodies.push_back(&body);
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
    // dont think we need to removed from nodes, we'll just clear them in update().
    auto it = std::find(bodies.begin(), bodies.end(), &body);
    bodies.erase(it);
}

Body* Grid::find_body(Vector2 point) const
{
    const GridNode& node = get_node(point.x, point.y);
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

void Grid::attach_debug_text(Body& body) const
{
    const GridNode& node = get_node(body.x, body.y);
    node.attach_debug_text(body);

    // No general grid debug text to attach. Just node-specific.
}

std::vector<Collision> Grid::get_collisions() const
{
    std::vector<Collision> collisions;

    for (const GridNode& node : nodes) {
        node.get_collisions(collisions);
    }

    return collisions;
}
