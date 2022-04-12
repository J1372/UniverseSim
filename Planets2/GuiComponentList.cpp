#include "GuiComponentList.h"
#include "raylib.h" // for Vector2

template<typename T, typename... ArgTypes>
T GuiComponentList::add(ArgTypes... args)
{
	T element{ args };

	elements.push_back(&element);

	return element;
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
