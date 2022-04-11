#pragma once
class Vector2;

class UIElement
{
	bool hidden = false;

public:

	bool hide() { hidden = true; }
	bool show() { hidden = false; }
	bool is_hidden() const { return hidden; }

	virtual void click() = 0;
	virtual bool contains_point(Vector2 point) const = 0;
	virtual void render() const = 0;
};

