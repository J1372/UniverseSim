#include "GuiComponentList.h"
#include "raylib.h" // for Vector2

#include "UIElement.h"

void GuiComponentList::set_active(UIElement* element)
{
	// Deactivate current active element
	if (active_element) {
		active_element->deactivate();
	}

	active_element = element;

	// If element was not null, then activate new active element.
	if (element) {
		active_element->activate();
	}
}

void GuiComponentList::render()
{
	for (UIElement* element : elements) {
		element->render();
	}
}

UIElement* GuiComponentList::get_element(Vector2 point)
{
	// elements are rendered first->last. so last elements are on top. loop backwards.
	for (int i = elements.size() - 1; i >= 0; i--) {
		UIElement* element = elements[i];
		if (element->contains_point(point)) {
			return element;
		}
	}

	// did not click on any element.
	return nullptr;
}

void GuiComponentList::send_click(Vector2 point)
{
	UIElement* element = get_element(point);

	if (element) {
		element->click();
	}

	set_active(element);
}

bool GuiComponentList::send_keypress(int key_code)
{
	if (active_element) {
		bool active_responded = active_element->send_keypress(key_code);

		/*
		* 
		* if active element does not accept the key input, gui may respond here
		* 
		*/

		return active_responded;
	}
	else {
		// gui may respond here, returning true if it has responded to the key press.
	}


	return false;
}
