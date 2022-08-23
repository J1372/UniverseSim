#pragma once

#include "UIElement.h"
#include <string>
#include <raylib.h>
#include <memory>
#include "TextValidator.h"

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

	// Whether the textbox should display a cursor line or not.
	bool should_render_cursor = false;

	// A validator that checks input into the textbox and potentially modifies it on deactivation.
	std::unique_ptr<TextValidator> validator = nullptr;

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
	// If a validator was set, this input will go through the validator.
	void set_text(std::string_view to_set);

	// Returns the currently entered text.
	const std::string& get_text() const;

	// Returns the currently entered text as an integer.
	int get_int() const;

	// Returns the currently entered text as a float.
	float get_float() const;

	// Returns the currently entered text as a double.
	double get_double() const;

	// Sets the text to be shown when no text has been entered.
	void set_prompt_text(const std::string& text);

	// Sets a validator for user input into this text box.
	void set_validator(std::unique_ptr<TextValidator>&& to_set);

	// Sets the textbox to active rendering.
	void activate() override;

	// If a validator was set, allows it to modify the final-result input.
	void deactivate() override;
};
