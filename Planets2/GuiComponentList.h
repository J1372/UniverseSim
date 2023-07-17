#pragma once
#include <vector>
#include <memory>
#include <type_traits>

#include "UIElement.h"

struct Vector2;

// A collection of gui elements that can be used for rendering itself,
// and delegating user input to its gui elements.
class GuiComponentList
{
	// The currentlt active element, that should be notified of user input.
	UIElement* active_element = nullptr;

	// All gui elements in this component list.
	std::vector<std::unique_ptr<UIElement>> elements;

	// All gui elements in this component list that are visible.
	std::vector<UIElement*> visible_elements;

public:

	// Adds an element of the given type to the gui.
	// Constructs the element with the given arguments.
	template <class T, class... ArgTypes>
	T& add(ArgTypes&&... args)
	{
		// Assert that UIElement is a base class of T.
		static_assert(std::is_convertible_v<T*, UIElement*>,
			"GuiComponentList: T must publicly inherit from UIElement.");

		// Assert that T has a constructor that takes the given arg types.
		static_assert(std::is_constructible_v<T, ArgTypes&&...>,
			"GuiComponentList: T does not have a constructor that takes the given argument types.");

		std::unique_ptr<T> obj_ptr = std::make_unique<T>(std::forward<ArgTypes>(args)...);
		T& element = *obj_ptr;

		visible_elements.push_back(obj_ptr.get());
		elements.emplace_back(std::move(obj_ptr));

		return element;
	}

	// Renders the gui.
	void render();

	// Returns the top-most element at a point on the screen.
	UIElement* get_element(Vector2 point);

	// Sends a click to the gui.
	// If the click was on a non-hidden element in the gui, clicks the element.
	void send_click(Vector2 point);

	// Sends a keypress to the gui.
	// If an element is active, sends the keypress to the element.
	bool send_keypress(int key_code);

	// Notifies the active element that the mouse is being dragged.
	void notify_drag();

	// Shows an element.
	void show(UIElement& element);

	// Hides an element.
	void hide(UIElement& element);

	// Toggles element's visibility.
	void toggle_visibility(UIElement& element);
};

