#pragma once
#include "UIElement.h"
#include <raylib.h>
#include <vector>
#include <string>

class Dropdown : public UIElement
{

	std::vector<std::string> choices;
	int selected = -1;


	int font_size;
	float width_padding = 0.1;

	Rectangle rect;

	Color background_color = BLACK;
	Color edge_color = DARKGRAY;

	static constexpr int edge_width = 10;

	//std::function<void(const std::string& text)> callback = nullptr;

	// Returns the selection which the user has clicked on.
	int translate_click() const;

	// Returns the height of the dropdown box.
	int get_render_height() const;

public:

	Dropdown(float x, float y, int font_size) :
		font_size(font_size),
		rect{ x, y, 100, 50 }
	{}

	void add_choice(const std::string& choice) { 
		choices.push_back(choice);
		int length_choice = MeasureText(choice.c_str(), font_size) * (1 + width_padding) + 2 * edge_width;

		if (length_choice > rect.width) {
			rect.width = length_choice;
		}
	}

	void set_selected(int number) { selected = number; }
	void deselect() { selected = -1; }
	bool has_selected() const { return selected >= 0 and selected < choices.size(); }

	std::string get_selected() const;

	void click();

	bool contains_point(Vector2 point) const;

	void render() const override;
};

