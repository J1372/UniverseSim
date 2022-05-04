#include "Grid.h"
#include "Body.h"

GridNode& Grid::get_node(int x, float y)
{
    int row = y / node_size;
    int col = x / node_size;

    int grid_num = row * row_width + col;

    return nodes[grid_num];

}

Grid::Grid(int grid_size, int node_size) : grid_size(grid_size), node_size(node_size), row_width(grid_size / node_size)
{
    nodes.reserve(row_width * row_width);

    for (int i = 0; i < row_width * row_width; i++) {
        int row = i % row_width;
        int col = i / row_width;

        float x = col * node_size;
        float y = row * node_size;

        nodes.emplace_back(x, y, node_size);
    }
}

void Grid::add_body(Body& body)
{
    GridNode& node = get_node(body.x, body.y);
    node.add(body);
}

void Grid::collision_check(std::vector<Body*>& to_remove)
{
    for (GridNode& node : nodes) {
        node.collision_check(to_remove);
    }
}

void Grid::update()
{
    for (GridNode& node : nodes) {
        node.update();
    }
}

void Grid::draw_debug(const Camera2D& camera) const
{
    for (const GridNode& node : nodes) {
        node.draw_debug();
    }
}
