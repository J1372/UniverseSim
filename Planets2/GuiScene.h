#pragma once
#include "Scene.h"
#include "GuiComponentList.h"
#include "raylib.h" // for Color


// A pure gui scene, all it has to update and render are ui elements. Implements the virtual Scene::update.
// 
// A scene with gui elements, but also other items, shouldn't inherit from this class
// since they will need to implement their own update method.
class GuiScene : public Scene {
protected:

	// The list of gui elements in the scene.
	GuiComponentList gui;

	// Scene to return at the end of update.
	Scene* return_scene = this;

	// Gui background color.
	Color background_color = RAYWHITE;

public:

	// Does a standard GUI update handling of user clicks and keypresses.
	Scene* update();

	virtual ~GuiScene() = default;

};

