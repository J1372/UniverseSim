#include "pch.h"
#include "BarnesHut.h"
#include "Body.h"

TEST(BarnesHut, Empty)
{
	BarnesHut barnes{ 2000, 1 };
	barnes.update({});

	Body b{ 0,0,100 };
	Vector2 forces = barnes.force_applied_to(b);
	EXPECT_FLOAT_EQ(forces.x, 0.0f);
	EXPECT_FLOAT_EQ(forces.y, 0.0f);
}

TEST(BarnesHut, One)
{
	BarnesHut barnes{ 2000, 1 };
	Body b{ 0,0,100 };

	barnes.update({ &b, 1 });
	Vector2 forces = barnes.force_applied_to(b);

	EXPECT_FLOAT_EQ(forces.x, 0.0f);
	EXPECT_FLOAT_EQ(forces.y, 0.0f);
}

TEST(BarnesHut, TwoNoApprox)
{
	BarnesHut barnes{ 2000, 0 };

	std::vector<Body> bodies
	{
		{ 0,0,100 },
		{ 500,0,100 },
	};

	constexpr float expected_force = 0.04f;

	barnes.update(bodies);

	Vector2 forces1 = barnes.force_applied_to(bodies[0]);
	EXPECT_FLOAT_EQ(forces1.x, expected_force);
	EXPECT_FLOAT_EQ(forces1.y, 0.0f);

	Vector2 forces2 = barnes.force_applied_to(bodies[1]);
	EXPECT_FLOAT_EQ(forces2.x, -expected_force);
	EXPECT_FLOAT_EQ(forces2.y, 0.0f);
}
TEST(BarnesHut, MultApprox)
{
	BarnesHut barnes{ 2000, 1 };

	std::vector<Body> bodies
	{
		{ -1000,500,100 },
		{ 1,1,100 },
		{ 999,999,100 },
		{ 500,500,100 },
	};

	// expect bodies[0] too far away from UR com { 500, 500 } with 300 mass.
	constexpr float dist_com_squared = 1500 * 1500;
	constexpr float expected_force = (100 * 300) / dist_com_squared;

	barnes.update(bodies);
	Vector2 forces = barnes.force_applied_to(bodies[0]);

	EXPECT_FLOAT_EQ(forces.x, expected_force);
	EXPECT_FLOAT_EQ(forces.y, 0.0f);
}

TEST(BarnesHut, MultApproxSamePos)
{
	BarnesHut barnes{ 2000, 1 };

	std::vector<Body> bodies
	{
		{ -1000,500,100 },
		{ 1,1,100 },
		{ 999,999,100 },
		{ 500,500,100 },
		{ 500,500,100 }, // same position as prev body.
		{ 500,500,100 }, // same position as prev body.
		{ 500,500,100 }, // same position as prev body.
		{ 500,500,100 }, // same position as prev body.
		{ 500,500,100 }, // same position as prev body.
		{ 500,500,100 }, // same position as prev body.
	};

	// expect bodies[0] too far away from UR com { 500, 500 } with 400 mass.
	constexpr float dist_com_squared = 1500 * 1500;
	constexpr float expected_force = (100 * 900) / dist_com_squared;

	barnes.update(bodies);

	Vector2 forces = barnes.force_applied_to(bodies[0]);
	EXPECT_FLOAT_EQ(forces.x, expected_force);
	EXPECT_FLOAT_EQ(forces.y, 0.0f);
}

