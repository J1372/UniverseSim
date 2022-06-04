#include "GUIScene.h"
#include "raylib.h" // for Vector2

Scene* GuiScene::update()
{
	// send clicks to gui components
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		gui.send_click(GetMousePosition());
	}
	else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		gui.notify_drag();
	}

	// send keypresses to gui components
	// GetKeyPressed returns from a queue. Returns 0 when empty.
	int key_code = GetKeyPressed(); 
	while (key_code) {
		gui.send_keypress(key_code);
		key_code = GetKeyPressed();
	}

	BeginDrawing();
	ClearBackground(background_color);
	gui.render();
	EndDrawing();

	return return_scene;
}
