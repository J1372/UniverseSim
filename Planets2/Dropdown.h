#pragma once
#include "UIElement.h"
#include <raylib.h>
#include <vector>
#include <string>
#include <functional>
#include <string_view>

class Dropdown : public UIElement
{

	// Could be a template pair string,enum
	std::vector<std::string> choices;
	int selected = -1;


	int font_size;
	float width_padding = 0.1f;

	Rectangle rect;

	Color background_color = BLACK;
	Color edge_color = DARKGRAY;

	static constexpr int edge_width = 10;

	// Whether the dropdown should display its options or not.
	bool should_render_options = false;

	std::function<void(const std::string& text)> callback = nullptr;

	// Returns the selection which the user has clicked on.
	int translate_click() const;

	// Returns the height of the dropdown box.
	float get_render_height() const;

public:

	Dropdown(float x, float y, int font_size);

	void add_choice(const std::string& choice);

	void set_selected(int number);
	void set_selected(std::string_view choice);
	void deselect();

	// Returns if any option has been selected.
	bool has_selected() const;

	std::string get_selected() const;

	void click();

	bool contains_point(Vector2 point) const;

	void render() const override;

	void set_on_selection(std::function<void(const std::string& text)> to_set);

	void deactivate() override;

};

