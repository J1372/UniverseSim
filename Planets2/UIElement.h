#pragma once
struct Vector2;

// An element of a gui that may be shown, hid, or sent input to in order for it to react.
class UIElement
{
	// Whether the element should be rendered.
	bool hidden = false;

public:

	// Marks the element so that it should not be rendered.
	void hide();

	// Marks the element so that it should be rendered.
	void show();

	// Toggles whether the element is hidden.
	void toggle_hide();

	// Returns true if the element is hidden.
	bool is_hidden() const;

	// Returns true if the element is not hidden.
	bool is_visible() const;

	// Notifies the element that it has been activated.
	virtual void activate() {};

	// Notifies the element that it has been deactivated.
	virtual void deactivate() {};

	// Notifies the element that it has been clicked on by the left mouse button.
	virtual void click() {};

	// Sends a key press to the element.
	// Returns true if the element responded to the key, else false.
	virtual bool send_keypress(int key_code);

	// Notifies the element that the left mouse button is being dragged.
	virtual void notify_drag() {};

	// Returns true if the element contains the point, else false.
	virtual bool contains_point(Vector2 point) const;

	// Renders the element using screen coordinates.
	virtual void render() const = 0;

	virtual ~UIElement() = default;


};

