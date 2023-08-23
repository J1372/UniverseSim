#include "pch.h"
#include "PlanetType.h"
#include "raylib.h"

TEST(TypeChange, DontDecreaseEqMin)
{
	constexpr int T1_MIN = 6000;
	constexpr PlanetType T1 = { GOLD, 100, T1_MIN };
	constexpr PlanetType T2 = { GOLD, 100, 12000 };
	EXPECT_EQ(0, T1.get_change(T1_MIN, &T2));
}

TEST(TypeChange, DecreaseLessThanMin)
{
	constexpr long T_MIN = 12000;
	constexpr PlanetType T = { GOLD, 100, T_MIN };
	EXPECT_EQ(-1, T.get_change(T_MIN - 1, &T));
}

TEST(TypeChange, IncreaseEqNextMin)
{
	constexpr long T1_MIN = 6000;
	constexpr long T2_MIN = 12000;
	constexpr PlanetType T1 = { GOLD, 100, T1_MIN };
	constexpr PlanetType T2 = { GOLD, 100, T2_MIN };
	EXPECT_EQ(1, T1.get_change(T2_MIN, &T2));
}
