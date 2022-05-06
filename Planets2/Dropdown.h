#pragma once
#include "UIElement.h"
#include <raylib.h>
#include <vector>
#include <string>

class Dropdown : public UIElement
{

	std::vector<std::string> choices;
	int selected = -1;


	int font_size = 12;
	int text_length_pixels = 0;
	float width_padding = 0.1;

	Rectangle rect;

	Color background_color = BLACK;
	Color edge_color = DARKGRAY;

	static constexpr int edge_width = 10;

	//std::function<void(const std::string& text)> callback = nullptr;

	// Returns the selection which the user has clicked on.
	int translate_click() const;

public:

	Dropdown() = default;

	void add_choice(const std::string& choice) { choices.push_back(choice); }

	void set_selected(int number) { selected = number; }
	void deselect() { selected = -1; }
	bool has_selected() const { return selected >= 0 and selected < choices.size(); }

	std::string get_selected() const;

	void click();

	bool contains_point(Vector2 point) const {
		return point.x >= rect.x and point.x < rect.x + rect.width
			and point.y >= rect.y and point.y < rect.y + rect.height;
	}

	void render() const override;
};

