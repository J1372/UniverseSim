#include "GUIScene.h"
#include "raylib.h" // for Vector2

UIElement* GuiScene::get_click_element(Vector2 point)
{
	return gui_elements.get_element(point);
}

Scene* GuiScene::update()
{
	gui_elements.render();
	return return_scene;
}
