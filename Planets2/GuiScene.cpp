#include "GUIScene.h"
#include "raylib.h" // for Vector2

Scene* GuiScene::update()
{
	// send clicks to gui components
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		gui.send_click(GetMousePosition());
	}

	// send keypresses to gui components
	// GetKeyPressed returns from a queue. Returns 0 when empty.
	int key_code = GetKeyPressed(); 
	while (key_code) {
		gui.send_keypress(key_code);
		key_code = GetKeyPressed();
	}

	gui.render();

	return return_scene;
}
