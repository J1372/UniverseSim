#pragma once
class UIElement
{
	int x;
	int y;
	bool hidden = false;

public:
	UIElement(int x, int y) : x(x), y(y) {}


	bool hide() { hidden = true; }
	bool show() { hidden = false; }
	bool is_hidden() const { return hidden; }

	virtual void click() = 0;
	virtual bool contains_point(Vector2 point) const = 0;
	virtual void render() = 0;
};

