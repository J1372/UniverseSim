#include "pch.h"

#include "Physics.h"
#include "Circle.h"
#include "raylib.h"
#include "raymath.h"

TEST(Physics, PointInCircle)
{
	Vector2 pt{ 0, 0 };
	Circle circle{ 0,0,10.0f };

	EXPECT_TRUE(Physics::point_in_circle(pt, circle));
}
TEST(Physics, PointInCircleEdges)
{
	Circle circle{ 0,0,10.0f };
	Vector2 l{ circle.left(), 0 };
	Vector2 r{ circle.right(), 0 };
	Vector2 t{ 0.0f, circle.bottom() };
	Vector2 b{ 0.0f, circle.top() };


	EXPECT_TRUE(Physics::point_in_circle(l, circle));
	EXPECT_TRUE(Physics::point_in_circle(r, circle));
	EXPECT_TRUE(Physics::point_in_circle(t, circle));
	EXPECT_TRUE(Physics::point_in_circle(b, circle));
}

TEST(Physics, PointNotInCircle)
{
	Circle circle{ 0,0,10.0f };
	Vector2 pt{ circle.left(), circle.top() };

	EXPECT_FALSE(Physics::point_in_circle(pt, circle));
}

TEST(Physics, PointInRect)
{
	Rectangle rect{ 0, 0, 10, 10 };

	Vector2 pt{ 5, 5 };

	EXPECT_TRUE(Physics::point_in_rect(pt, rect));
}

TEST(Physics, PointNotInRect)
{
	Rectangle rect{ 0, 0, 10, 10 };

	Vector2 l{ -1, 5 };
	Vector2 r{ rect.width, 5 };
	Vector2 t{ 5, -1 };
	Vector2 b{ 5, rect.height };

	EXPECT_FALSE(Physics::point_in_rect(l, rect));
	EXPECT_FALSE(Physics::point_in_rect(r, rect));
	EXPECT_FALSE(Physics::point_in_rect(t, rect));
	EXPECT_FALSE(Physics::point_in_rect(b, rect));
}

TEST(Physics, CirclesIntersect)
{
	Circle c{ Vector2{0,0}, 10 };
	Circle l{ Vector2{-20,0}, 11 };
	Circle r{ Vector2{20,0}, 11 };
	Circle t{ Vector2{0,-20}, 11 };
	Circle b{ Vector2{0,20}, 11 };

	EXPECT_TRUE(Physics::circles_intersect(c, l));
	EXPECT_TRUE(Physics::circles_intersect(c, r));
	EXPECT_TRUE(Physics::circles_intersect(c, t));
	EXPECT_TRUE(Physics::circles_intersect(c, b));
}

TEST(Physics, CirclesDontIntersect)
{
	Circle c{ Vector2{0,0}, 10 };
	Circle l{ Vector2{-20,0}, 10 };
	Circle r{ Vector2{20,0}, 10 };
	Circle t{ Vector2{0,-20}, 10 };
	Circle b{ Vector2{0,20}, 10 };

	EXPECT_FALSE(Physics::circles_intersect(c, l));
	EXPECT_FALSE(Physics::circles_intersect(c, r));
	EXPECT_FALSE(Physics::circles_intersect(c, t));
	EXPECT_FALSE(Physics::circles_intersect(c, b));
}

TEST(Physics, CircleInsideRect)
{
	Circle c1{ Vector2{5,5}, 5 };
	Circle c2{ Vector2{5,5}, 6 };
	Circle c3{ Vector2{0,0}, 1 };
	Rectangle r{ 0, 0, 10, 10 };

	EXPECT_TRUE(Physics::circle_inside_rect(c1, r));
	EXPECT_FALSE(Physics::circle_inside_rect(c2, r));
	EXPECT_FALSE(Physics::circle_inside_rect(c3, r));
}

TEST(Physics, CircleIntersectsRect)
{
	Rectangle r{ 0, 0, 10, 10 };

	Circle c1{ Vector2{-1,5}, 1 };
	Circle c2{ Vector2{5,5}, 5 };
	Circle c3{ Vector2{5,5}, 6 };
	Circle c4{ Vector2{0,0}, 1 };
	Circle c5{ Vector2{11,11}, 1 };
	Circle c6{ Vector2{11,5}, 1 };

	EXPECT_TRUE(Physics::circle_intersects_rect(c1, r));
	EXPECT_TRUE(Physics::circle_intersects_rect(c2, r));
	EXPECT_TRUE(Physics::circle_intersects_rect(c3, r));
	EXPECT_TRUE(Physics::circle_intersects_rect(c4, r));
	EXPECT_FALSE(Physics::circle_intersects_rect(c5, r));
	EXPECT_FALSE(Physics::circle_intersects_rect(c6, r));
}


TEST(Physics, Dist)
{
	Vector2 p1{ 5, 5 };
	Vector2 p2{ 10, 5 };
	Vector2 p3{ 10, 10 };

	EXPECT_EQ(5, Physics::dist(p1, p2));
	EXPECT_EQ(5, Physics::dist(p2, p3));
	EXPECT_FLOAT_EQ(std::sqrt(50), Physics::dist(p1, p3));
}

TEST(Physics, DistNegatives)
{
	Vector2 p1{ 5, 5 };
	Vector2 p2{ 10, 5 };
	Vector2 p3{ 10, 10 };
	Vector2 p4{ -10, -10 };

	EXPECT_FLOAT_EQ(std::sqrt(450), Physics::dist(p4, p1));
	EXPECT_FLOAT_EQ(std::sqrt(625), Physics::dist(p4, p2));
	EXPECT_FLOAT_EQ(std::sqrt(800), Physics::dist(p4, p3));
}

TEST(Physics, DistSquared)
{
	Vector2 p1{ 5, 5 };
	Vector2 p2{ 10, 5 };
	Vector2 p3{ 10, 10 };

	EXPECT_EQ(25, Physics::dist_squared(p1, p2));
	EXPECT_EQ(25, Physics::dist_squared(p2, p3));
	EXPECT_EQ(50, Physics::dist_squared(p1, p3));
}

TEST(Physics, DistSquaredNegatives)
{
	Vector2 p1{ 5, 5 };
	Vector2 p2{ 10, 5 };
	Vector2 p3{ 10, 10 };
	Vector2 p4{ -10, -10 };

	EXPECT_EQ(450, Physics::dist_squared(p4, p1));
	EXPECT_EQ(625, Physics::dist_squared(p4, p2));
	EXPECT_EQ(800, Physics::dist_squared(p4, p3));
}

bool Vector2Eq(Vector2 v1, Vector2 v2, float abs_range)
{
	Vector2 dif = Vector2Subtract(v2, v1);
	return std::abs(dif.x) < abs_range && std::abs(dif.y) < abs_range;
}

TEST(Physics, Distv)
{
	Vector2 p1{ 5, 5 };
	Vector2 p2{ 10, 5 };
	Vector2 p3{ 10, 10 };

	Vector2 d1{ 5, 0 };
	Vector2 d2{ 0, 5 };
	Vector2 d3{ 5, 5 };

	EXPECT_TRUE(Vector2Eq(d1, Physics::distv(p1, p2), 0.01f));
	EXPECT_TRUE(Vector2Eq(d2, Physics::distv(p2, p3), 0.01f));
	EXPECT_TRUE(Vector2Eq(d3, Physics::distv(p1, p3), 0.01f));
	EXPECT_TRUE(Vector2Eq(Vector2Negate(d1), Physics::distv(p2, p1), 0.01f));
	EXPECT_TRUE(Vector2Eq(Vector2Negate(d2), Physics::distv(p3, p2), 0.01f));
	EXPECT_TRUE(Vector2Eq(Vector2Negate(d3), Physics::distv(p3, p1), 0.01f));
}


TEST(Physics, DistvNegative)
{
	Vector2 p1{ 5, 5 };
	Vector2 p2{ 10, 5 };
	Vector2 p3{ 10, 10 };
	Vector2 p4{ -10, -10 };

	Vector2 d1{ 15, 15 };
	Vector2 d2{ 20, 15 };
	Vector2 d3{ 20, 20 };

	EXPECT_TRUE(Vector2Eq(d1, Physics::distv(p4, p1), 0.01f));
	EXPECT_TRUE(Vector2Eq(d2, Physics::distv(p4, p2), 0.01f));
	EXPECT_TRUE(Vector2Eq(d3, Physics::distv(p4, p3), 0.01f));

	EXPECT_TRUE(Vector2Eq(Vector2Negate(d1), Physics::distv(p1, p4), 0.01f));
	EXPECT_TRUE(Vector2Eq(Vector2Negate(d2), Physics::distv(p2, p4), 0.01f));
	EXPECT_TRUE(Vector2Eq(Vector2Negate(d3), Physics::distv(p3, p4), 0.01f));
}

TEST(Physics, NetForce)
{

	Vector2 p1{ 5, 5 };
	long m1 = 25;
	Vector2 p2{ 10, 5 };
	long m2 = 8;

	float force = 200.0f / 25;

	EXPECT_EQ(force, Physics::net_force(p1, m1, p2, m2));
	EXPECT_EQ(force, Physics::net_force(p2, m2, p1, m1));
}

TEST(Physics, NetForceSamePos)
{
	Vector2 p1{ 5, 5 };
	long m1 = 20;
	long m2 = 500000;
	EXPECT_EQ(0.0f, Physics::net_force(p1, m1, p1, m2));
}

TEST(Physics, GravForceAligned)
{
	Vector2 p1{ 5, 5 };
	Vector2 p2{ 10, 5 };
	long m1 = 25;
	long m2 = 8;

	float force = 200.0f / 25;

	Vector2 force_on_first{ force,0.0f };
	Vector2 force_on_second{ -force,0.0f };

	EXPECT_TRUE(Vector2Eq(force_on_first, Physics::grav_force(p1, m1, p2, m2), 0.01f));
	EXPECT_TRUE(Vector2Eq(force_on_second, Physics::grav_force(p2, m2, p1, m1), 0.01f));
}

TEST(Physics, GravForceSamePos)
{
	Vector2 p1{ 5, 5 };
	long m1 = 25;
	long m2 = 8;

	EXPECT_TRUE(Vector2Eq(Vector2Zero(), Physics::grav_force(p1, m1, p1, m2), 0.01f));
}

TEST(Physics, Moment)
{
	long mass = 50;
	Vector2 pos{ 10, 5 };
	Vector2 moment{ 500, 250 };

	EXPECT_TRUE(Vector2Eq(moment, Physics::moment(pos, mass), 0.01f));
}
