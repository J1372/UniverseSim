#include "Dropdown.h"

int Dropdown::translate_click() const
{
	int diff_y = GetMouseY() - rect.y;

	int click_selection = diff_y / rect.height;

	if (click_selection == 0) { // Clicked the base box.
		return selected; // return previous selection.
	}

	return click_selection - 1;
}

std::string Dropdown::get_selected() const
{
	if (has_selected()) {
		return choices[selected];
	}
	else {
		return "";
	}
}

void Dropdown::click()
{
	if (active) {
		selected = translate_click();
		deactivate();
	}
}

void Dropdown::render() const
{

	int start_x = rect.x + rect.width * (width_padding / 2);
	int top_y = rect.y + edge_width; // y coord of the bottom of the top edge.
	int full_width = rect.height - 2 * edge_width; // distance from bottom of top edge to top of bottom edge.
	int start_y = top_y + full_width / 3;

	Rectangle render_rect = rect;

	if (active) {
		// set render rectangle height to accomodate all the choices.
		render_rect.height = rect.height * choices.size();
	}

	// draw background
	DrawRectangleRec(render_rect, background_color);
	DrawRectangleLinesEx(render_rect, edge_width, edge_color);

	if (active) { // render choices list
		constexpr float SEPARATOR_THICKNESS = 3.0f;

		// draw separator lines
		Vector2 sep_left = { render_rect.x, 0 };
		Vector2 sep_right = { render_rect.x + render_rect.width, 0 };

		for (int i = 1; i < choices.size(); i++) {
			sep_left.y = render_rect.y + rect.height * i;
			sep_right.y = sep_left.y;
			DrawLineEx(sep_left, sep_right, SEPARATOR_THICKNESS, GRAY);
		}

		// draw choices.
		for (int i = 0; i < choices.size(); i++) {
			const std::string& choice = choices[i];

			DrawText(choice.c_str(), start_x, start_y + rect.height * i, font_size, LIGHTGRAY);
		}
	}
	

	if (has_selected()) { // if selected an option, draw current choice in the base box.
		DrawText(get_selected().c_str(), start_x, start_y, font_size, WHITE);
	}


	
}
