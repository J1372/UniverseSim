#pragma once

class Body;

class BodyObserver
{
public:

	virtual void notify_collision(Body& body1, Body& body2) {};
	virtual void notify_body_no_longer_exists(const Body* absorbed_by) {};

};
