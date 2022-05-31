#pragma once
#include <raylib.h>

struct Circle {
	Vector2 center;
	float radius;

	float left() const { return center.x - radius; }
	float right() const { return center.x + radius; }
	float top() const { return center.y - radius; }
	float bottom() const { return center.y + radius; }
	float diameter() const { return 2 * radius; }
};
