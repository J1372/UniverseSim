#pragma once
struct Vector2;

class UIElement
{
protected:

	bool hidden = false;
	bool active = false; // Some elements render differently when active.

public:

	bool hide() { hidden = true; }
	bool show() { hidden = false; }
	bool is_hidden() const { return hidden; }

	void activate() { active = true; }
	void deactivate() { active = false; }


	virtual void click() = 0;
	virtual bool send_keypress(int key_code) = 0;
	virtual bool contains_point(Vector2 point) const = 0;
	virtual void render() const = 0;


};

