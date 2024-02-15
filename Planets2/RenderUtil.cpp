#include "RenderUtil.h"
#include <raylib.h>
#include "Body.h"
#include <raymath.h>

void RenderUtil::render_body(const Body& body)
{
	Vector2 pos = body.pos();
	Color planet_color = body.color();

	DrawCircle(pos.x, pos.y, body.get_radius(), planet_color);
}

void RenderUtil::render_near_body(const Body& body, const char* text)
{
	Vector2 pos = body.pos();
	float radius = body.get_radius();


	// Calculate font size with minor scaling based on body's radius.
	constexpr float FONT_START = 25;
	constexpr float FONT_SCALE = 0.05;
	int font_size = FONT_START + FONT_SCALE * radius;

	int text_x = pos.x + radius + 20;
	int text_y = pos.y + radius + 20;
	DrawText(text, text_x, text_y, font_size, body.color());
}

void RenderUtil::render_body_vector(const Body& body, Vector2 vec, float dist_scale, float thick_scale, Color color)
{
	Vector2 dir = Vector2Normalize(vec);
	Vector2 start_pos = Vector2Add(body.pos(), Vector2Scale(dir, body.get_radius() - 0.5f));
	Vector2 end_pos = Vector2Add(start_pos, Vector2Scale(dir, dist_scale));
	DrawLineEx(start_pos, end_pos, thick_scale, color);
}

void RenderUtil::render_force(const Body& body)
{
	float dist_scale = 50.0f;
	float thick_scale = std::max(3.0f, body.diameter() / 20);
	render_body_vector(body, body.get_forces(), dist_scale, thick_scale, RED);
}

void RenderUtil::render_velocity(const Body& body)
{
	float dist_scale = 50.0f;
	float thick_scale = std::max(3.0f, body.diameter() / 20);
	render_body_vector(body, body.vel(), dist_scale, thick_scale, SKYBLUE);
}
