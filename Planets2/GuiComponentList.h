#pragma once
#include <vector>
#include <memory>

#include "UIElement.h"
struct Vector2;

class GuiComponentList
{
	UIElement* active_element = nullptr; // where to send keypresses to.
	// send keypress returns false if no response or true if it accepts the input.
	// if false, process input yourself (tab inserts a tab char in TextBox, tab switches to next element in our Scene for example).
	// must set_unactive() before switching elements.
	// not all uielements care to store whether they are active or not, but TextBox for example draws a cursor line if active. 
	std::vector<std::unique_ptr<UIElement>> elements;

	// A separate list of only bisible element pointers, so no need to poll in render() and get_element().
	// Could set a bool flag has_changed that is implicitly changed after send_click, show hide. And check first in render, so often no loop.
	// But would still need to poll in get_element without this vector. Which would only happen on any click. Might not be bad.
	std::vector<UIElement*> visible_elements;

	// if maintain order needed, could give each element an id and keep vectors sorted.

	void set_active(UIElement* element);

public:

	template <class T, class... ArgTypes>
	T& add(ArgTypes&&... args)
	{
		std::unique_ptr<T> obj_ptr = std::make_unique<T>(std::forward<ArgTypes>(args)...);
		T& element = *obj_ptr;

		visible_elements.push_back(obj_ptr.get());
		elements.emplace_back(std::move(obj_ptr));

		return element;

		//return *elements.back.get();
	}

	void render();

	UIElement* get_element(Vector2 point); // get top-most element at a point on the screen.

	void send_click(Vector2 point);
	bool send_keypress(int key_code);
	void notify_drag();

	void show(UIElement& element);
	void hide(UIElement& element);
};

