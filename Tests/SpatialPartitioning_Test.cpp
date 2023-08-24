#include "pch.h"

#include "SpatialPartitioning.h"
#include "QuadTree.h"
#include "Grid.h"
#include "LineSweep.h"
#include "Body.h"
#include <Collision.h>

using SPFactory = SpatialPartitioning*(*)();

class SPTestFixture : public testing::TestWithParam<SPFactory>
{
public:

	~SPTestFixture()
	{
		delete partitioning;
	}

	void SetUp()
	{ 
		partitioning = GetParam()(); 
	};

	void TearDown()
	{
		delete partitioning;
		partitioning = nullptr;
	};

protected:

	SpatialPartitioning* partitioning;

};

template<float size, int max_bodies, int max_depth>
SpatialPartitioning* CreateQuadTree()
{
	return new QuadTree{ size, max_bodies, max_depth };
}

template<float grid_size, int nodes_per_row>
SpatialPartitioning* CreateGrid()
{
	return new Grid{ grid_size, nodes_per_row };
}

SpatialPartitioning* CreateLineSweep()
{
	return new LineSweep;
}

TEST_P(SPTestFixture, CollisionsEmpty)
{
	partitioning->update();
	auto collisions = partitioning->get_collisions();
	EXPECT_EQ(collisions.size(), 0);
}

TEST_P(SPTestFixture, CollisionsOne)
{
	auto b = Body{ 0, 0, 0 };
	partitioning->add_body(b);
	partitioning->update();
	auto collisions = partitioning->get_collisions();
	EXPECT_EQ(collisions.size(), 0);
}

TEST_P(SPTestFixture, CollisionsMultiple)
{
	constexpr int mass = 500;
	auto radius = Body{ 0,0, mass }.get_radius();

	std::vector<Body> bodies
	{
		{ 0, 0, mass },
		{ 2 * radius, 0, mass },
		{ 2 * -radius, 0, mass },
		{ 0, 2 * -radius, mass },
		{ 0, 2 * radius, mass },
		{ 4 * radius, 0, mass },
		{ 4 * -radius, 0, mass },
		{ 0, 4 * -radius, mass },
		{ 0, 4 * radius, mass },
		{ 10 * radius, 10 * radius, mass },
	};

	for (int i = 0; i < bodies.size(); ++i)
	{
		auto& b1 = bodies[i];
		b1.set_id(i);
		partitioning->add_body(b1);
	}

	std::vector<Collision> expected;

	for (auto it1 = bodies.begin(); it1 != bodies.end() - 1; it1++)
	{
		Body& body1 = *it1;
		for (auto it2 = it1 + 1; it2 != bodies.end(); it2++)
		{
			Body& body2 = *it2;

			if (body1.collided_with(body2))
			{
				expected.emplace_back(Body::get_sorted_pair(body1, body2));
			}
		}
	}

	partitioning->update();
	auto collisions = partitioning->get_collisions();

	EXPECT_EQ(collisions.size(), expected.size());
	for (const auto& c : collisions)
	{
		auto it = std::find(expected.begin(), expected.end(), c);
		EXPECT_TRUE(it != expected.end()) << "Did not find: " << c.bigger.get_id() << ", " << c.smaller.get_id() << '\n';
	}
}

TEST_P(SPTestFixture, FindBody)
{
	constexpr int mass = 500;
	auto radius = Body{ 0,0, mass }.get_radius();

	std::vector<Body> bodies
	{
		{ 0, 0, mass },
		{ 2 * radius, 0, mass },
		{ 2 * -radius, 0, mass },
		{ 0, 2 * -radius, mass },
		{ 0, 2 * radius, mass },
		{ 4 * radius, 0, mass },
		{ 4 * -radius, 0, mass },
		{ 0, 4 * -radius, mass },
		{ 0, 4 * radius, mass },
		{ 10 * radius, 10 * radius, mass },
	};

	std::vector<Vector2> points;

	for (Body& b : bodies)
	{
		auto pos = b.pos();
		partitioning->add_body(b);
		points.emplace_back(pos.x + radius / 5, pos.y);
	}

	for (int i = 0; i < points.size(); ++i)
	{
		EXPECT_EQ(partitioning->find_body(points[i]), &bodies[i]);
	}

	EXPECT_EQ(partitioning->find_body(Vector2{ -5 * radius, -5 * radius }), nullptr);

}

INSTANTIATE_TEST_CASE_P(SpatialPartitioningInterface, SPTestFixture,
	testing::Values(&CreateQuadTree<2000.0f, 10, 10>,
		&CreateGrid<2000.0f, 10>,
		&CreateLineSweep));
