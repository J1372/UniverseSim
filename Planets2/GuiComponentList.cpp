#include "GuiComponentList.h"
#include "raylib.h" // for Vector2

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
	for (auto&& element : visible_elements) {
		element->render();
	}
}

UIElement* GuiComponentList::get_element(Vector2 point)
{
	// elements are rendered first->last. so last elements are on top. loop backwards.
	for (int i = visible_elements.size() - 1; i >= 0; i--) {
		auto&& element = visible_elements[i];
		if (element->contains_point(point)) {
			return element;
		}
	}

	// did not click on any visible element.
	return nullptr;
}

void GuiComponentList::send_click(Vector2 point)
{
	UIElement* clicked_on = get_element(point);

	if (active_element and active_element != clicked_on) {
		active_element->deactivate();
	}

	if (clicked_on) {
		clicked_on->click();
		clicked_on->activate();
	}

	active_element = clicked_on;
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

void GuiComponentList::notify_drag()
{
	if (active_element) {
		active_element->notify_drag();
	}
}

void GuiComponentList::show(UIElement& element)
{
	if (element.is_hidden()) {
		element.show();
		visible_elements.push_back(&element);
	}
}

void GuiComponentList::hide(UIElement& element)
{
	if (!element.is_hidden()) {
		element.hide();
		auto it = std::find(visible_elements.begin(), visible_elements.end(), &element);
		visible_elements.erase(it);
	}
}
