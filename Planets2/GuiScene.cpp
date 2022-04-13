#include "GUIScene.h"
#include "raylib.h" // for Vector2

Scene* GuiScene::update()
{
	gui.render();
	return return_scene;
}
