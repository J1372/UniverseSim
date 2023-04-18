#pragma once

class Scene
{
public:

	// Updates the scene, and returns a pointer to the scene to transition to, which may be nullptr or the same scene.
	virtual Scene* update() = 0;

	// Notifies the scene that the window has been resized.
	virtual void notify_resize(int width, int height)
	{}

	virtual ~Scene() = default;
};

