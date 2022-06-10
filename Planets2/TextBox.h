#pragma once

#include "UIElement.h"
#include <string>
#include <raylib.h>

// A gui textbox that the user can type characters into.
class TextBox : public UIElement
{
	// The current text contained in the textbox.
	std::string entered_text;

	// Text to be shown when entered_text is empty.
	std::string prompt_text;
	
	// Index to insert / delete characters on keypress.
	int cursor_pos = 0;

	// Font size of entered and prompt text.
	int font_size = 12;

	// Padding between text and the border.
	float width_padding = 0.1f;

	// The textbox's area.
	Rectangle rect;

	// Color of textbox background, over which text is displayed.
	Color background_color = BLACK;

	// Color of textbox edge.
	Color edge_color = DARKGRAY;

	// Thickness of the textbox's edge.
	static constexpr int edge_width = 10;

	// Returns the x coord of where to begin drawing the currently entered text.
	int get_start_x_text() const;

public:

	TextBox(float x, float y, float width);
	TextBox(const std::string& start_text, float x, float y, float width);

	// Sets the textbox cursor position based on the mouse's current x-coordinate.
	void click();

	// Returns true if a point overlaps with the textbox, else false.
	bool contains_point(Vector2 point) const;

	// Renders the textbox.
	void render() const;

	// Reacts to a keypress.
	// Returns true if textbox consumed the input, else false.
	bool send_keypress(int key_code);

	// Sets the entered text.
	void set_text(std::string& to_set);

	// Sets the entered text.
	void set_text(std::string&& to_set);

	// Returns the currently entered text.
	const std::string& get_text() const;

	//Returns true if the text in this textbox is just a number, else false.
	bool is_number() const;

	// Returns the currently entered text as an integer.
	int get_int() const;

	// Returns the currently entered text as a float.
	float get_float() const;

	// Returns the currently entered text as a double.
	double get_double() const;

	// Sets the text to be shown when no text has been entered.
	void set_prompt_text(const std::string& text);
};
