#pragma once
#include "UIElement.h"
#include <vector>

class GuiComponentList
{
	UIElement* active_element = nullptr; // where to send keypresses to.
	// send keypress returns false if no response or true if it accepts the input.
	// if false, process input yourself (tab inserts a tab char in TextBox, tab switches to next element in our Scene for example).
	// must set_unactive() before switching elements.
	// not all uielements care to store whether they are active or not, but TextBox for example draws a cursor line if active. 
	std::vector<UIElement*> elements;

	void set_active(UIElement* element);

public:

	template <typename T, typename... ArgTypes>
	T add(ArgTypes... args);

	void render();

	UIElement* get_element(Vector2 point); // get top-most element at a point on the screen.

	void send_click(Vector2 point);
	bool send_keypress(int key_code);
};

