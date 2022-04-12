#pragma once
#include "Scene.h"
#include "UIElement.h"
#include "GuiComponentList.h"


// pure gui scene. all it has are ui elements. Can take care of the virtual Scene:: update ourselves.
// 
// A scene with gui elements, but also other items, will need to implement own update method. 
// But they will find render loop and input loop nice to be able to call

class GuiScene : public Scene {

	GuiComponentList gui_elements;
	Scene* return_scene = this;

protected:

	GuiScene(int width, int height) : Scene{ width, height } {}

	template <typename T, typename... ArgTypes>
	T add_element(ArgTypes... args) { return gui_elements.add<T>(args); }

	UIElement* get_click_element(Vector2 point);

	void set_return(Scene* to_return) { return_scene = to_return; }


public:

	Scene* update();

	virtual ~GuiScene() = default;

};

