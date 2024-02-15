#pragma once

class Body;
struct Vector2;
struct Color;

namespace RenderUtil
{
	// Render the body.
	void render_body(const Body& body);

	// Draw text near the given body.
	void render_near_body(const Body& body, const char* text);

	// Render a scaled vector centered around a body.
	void render_body_vector(const Body& body, Vector2 vec, float dist_scale, float thick_scale, Color color);

	// Render a default force vector centered on the body.
	void render_force(const Body& body);

	// Render a default velocity vector centered on the body.
	void render_velocity(const Body& body);

};

