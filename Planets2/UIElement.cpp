#include "UIElement.h"
#include "raylib.h"

bool UIElement::contains_point(Vector2 point) const
{
	return false;
}

void UIElement::hide()
{
	hidden = true;
}

void UIElement::show()
{
	hidden = false;
}

void UIElement::toggle_hide()
{
	hidden = !hidden;
}

bool UIElement::is_hidden() const
{
	return hidden;
}

bool UIElement::is_visible() const
{
	return !hidden;
}

void UIElement::activate()
{
	active = true;
}

void UIElement::deactivate()
{
	on_deactivation();
	active = false;
}

bool UIElement::send_keypress(int key_code)
{
	return false;
}
