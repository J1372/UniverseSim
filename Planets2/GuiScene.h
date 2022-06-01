#pragma once
#include "Scene.h"
#include "GuiComponentList.h"
#include "raylib.h" // for Color


// pure gui scene. all it has are ui elements. Can take care of the virtual Scene:: update ourselves.
// 
// A scene with gui elements, but also other items, will need to implement own update method. 
// But they will find render loop and input loop nice to be able to call

class GuiScene : public Scene {
protected:

	GuiComponentList gui;
	Scene* return_scene = this;
	Color background_color = RAYWHITE;

public:

	Scene* update();

	virtual ~GuiScene() = default;

};

