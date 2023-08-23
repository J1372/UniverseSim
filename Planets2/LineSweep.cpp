#include "LineSweep.h"
#include "raylib.h"

#include "Collision.h"
#include "Body.h"

#include "Physics.h"
#include "DebugInfo.h"
#include <algorithm>

LineSweep::LineSweep::SweepEvent LineSweep::get_next_event(int entries_processed, int leaves_processed) const
{
    if (entries_processed == entry_events.size()) {
        // we have no entries, but we may have leave events.
        // Nevertheless, return end event, since we only do collision checking on entry events.

        return SweepEvent::END;

    }

    // We have entry events.

    const Body& next_entry = *entry_events[entries_processed];

    // Don't need to check if we have leaves.
    // no way to have entries and not have leaves.

    const Body& next_leave = *leave_events[leaves_processed];

    // Return whichever event comes first.
    if (next_entry.left() <= next_leave.right()) {
        return SweepEvent::ENTRY;
    }
    else {
        return SweepEvent::LEAVE;

    }

    return SweepEvent::END;
}

void LineSweep::add_body(Body& body)
{
    auto it = std::upper_bound(entry_events.cbegin(), entry_events.cend(), &body, left_less_than);
    entry_events.insert(it, &body);

    it = std::upper_bound(leave_events.cbegin(), leave_events.cend(), &body, right_less_than);
    leave_events.insert(it, &body);
}

void LineSweep::add_bodies(std::span<Body*> bodies)
{
    for (Body* body : bodies) {
        entry_events.push_back(body);
        leave_events.push_back(body);
    }

    sort_events();
}

void LineSweep::rem_body(const Body& body)
{
    auto entry_event_it = get_entry_it(body);
    auto leave_event_it = get_leave_it(body);

    entry_events.erase(entry_event_it);
    leave_events.erase(leave_event_it);

}

void LineSweep::notify_move(const Body* from, Body* to)
{
    auto entry_event_it = get_entry_it(*from);
    auto leave_event_it = get_leave_it(*from);

    *entry_event_it = to;
    *leave_event_it = to;

}

void LineSweep::update()
{
    sort_events();
}

Body* LineSweep::find_body(Vector2 point) const
{
    auto it = std::find_if(entry_events.begin(), entry_events.end(), [point](const Body* body) {
        return body->contains_point(point);
    });

    if (it != entry_events.end()) {
        return *it;
    }

    return nullptr;
}

std::vector<Rectangle> LineSweep::get_representation() const
{
    // Return entry and exit lines. From top universe to bottom.
    std::vector<Rectangle> rep;

    for (Body* body : entry_events) {
        rep.emplace_back(body->left(), body->top(), 2.0f, body->diameter());
    }

    for (Body* body : leave_events) {
        rep.emplace_back(body->right(), body->top(), 2.0f, body->diameter());
    }

    return rep;
}

void LineSweep::get_info(const Body& body, DebugInfo& info) const
{
    // entry index
    // exit index

    auto entry_event_it = get_entry_it(body);
    auto leave_event_it = get_leave_it(body);

    int entry_index = std::distance(entry_events.begin(), entry_event_it);
    int leave_index = std::distance(leave_events.begin(), leave_event_it);

    info.add("Entry event index: " + std::to_string(entry_index));
    info.add("Leave event index: " + std::to_string(leave_index));
}

std::vector<Collision> LineSweep::get_collisions_impl()
{
    std::vector<Collision> collisions;
    collisions.reserve(entry_events.size());

    // When entry event processed, new body scans collisions with these bodies.
    // Then the new body is added here and removed on its leave event encounter.
    // Sorted based on left(), like entry_events.
    std::vector<Body*> currently_active;
    currently_active.reserve(entry_events.size());

    // can just pop off back (scan right to left) and go until both empty.
    int entries_processed = 0;
    int leaves_processed = 0;

    // We can stop on last entry event.

    SweepEvent next_event = get_next_event(entries_processed, leaves_processed);
    while (next_event != SweepEvent::END) {
        // if next entry event's left() comes before next leave event, add to curr active and advance entries_processed.
        // if next leave event's right() comes before next entry event, remove from active and advance leaves_processed.


        if (next_event == SweepEvent::ENTRY) {
            // Get collisions between the body just entering, and all active bodies.
            // Then add to active bodies.

            Body& entry = *entry_events[entries_processed];

            num_collision_checks_tick += get_collisions(entry, currently_active, collisions);

            currently_active.push_back(&entry);

            entries_processed++;
        }
        else if (next_event == SweepEvent::LEAVE) {
            // Remove the body from the list of active bodies.

            Body& leave = *leave_events[leaves_processed];

            // Can do binary search, since currently_active is sorted.
            auto it = std::lower_bound(currently_active.begin(), currently_active.end(), &leave, left_less_than);
            currently_active.erase(it);

            leaves_processed++;
        }

        next_event = get_next_event(entries_processed, leaves_processed);
    }
    

    return collisions;
}

int LineSweep::get_collisions(Body& entry, std::span<Body*> currently_active, std::vector<Collision>& collisions) const
{
    for (Body* body : currently_active) {
        if (entry.collided_with(*body)) {
            collisions.emplace_back(Body::get_sorted_pair(entry, *body));
        }
    }

    // Body is checked with all currently active bodies.
    return currently_active.size();
}

void LineSweep::sort_events()
{
    std::sort(entry_events.begin(), entry_events.end(), left_less_than);
    std::sort(leave_events.begin(), leave_events.end(), right_less_than);
}

std::vector<Body*>::const_iterator LineSweep::get_entry_it(const Body& body) const
{
    auto it = std::lower_bound(entry_events.cbegin(), entry_events.cend(), &body, left_less_than);

    // it can be pointing at leftmost body whose leftmost point >= body's leftmost point. should be guaranteed == if in entry events.
    // might be multiple entry events with same point.
    while (it != entry_events.cend() and **it != body) { // if in events, shouldnt need to check for != cend().
        it++;
    }

    return it;
}

std::vector<Body*>::iterator LineSweep::get_entry_it(const Body& body)
{
    auto it = std::lower_bound(entry_events.begin(), entry_events.end(), &body, left_less_than);

    // it can be pointing at leftmost body whose leftmost point >= body's leftmost point. should be guaranteed == if in entry events.
    // might be multiple entry events with same point.
    while (it != entry_events.end() and **it != body) { // if in events, shouldnt need to check for != cend().
        it++;
    }

    return it;
}

std::vector<Body*>::const_iterator LineSweep::get_leave_it(const Body& body) const
{
    auto it = std::lower_bound(leave_events.cbegin(), leave_events.cend(), &body, right_less_than);

    // it is pointing at first body whose rightmost point >= body's rightmost point. if in events, should be first ==.
    while (it != leave_events.cend() and **it != body) { // if in events, shouldnt need to check for != cend().
        it++;
    }

    return it;
}

std::vector<Body*>::iterator LineSweep::get_leave_it(const Body& body)
{
    auto it = std::lower_bound(leave_events.begin(), leave_events.end(), &body, right_less_than);

    // it is pointing at first body whose rightmost point >= body's rightmost point. if in events, should be first ==.
    while (it != leave_events.end() and **it != body) { // if in events, shouldnt need to check for != cend().
        it++;
    }

    return it;
}

bool LineSweep::left_less_than_point(const Body* body, float entry_x)
{
    return body->left() < entry_x;
}

bool LineSweep::left_less_than(const Body* body1, const Body* body2)
{
    return body1->left() < body2->left();
}

bool LineSweep::right_less_than_point(const Body* body, float leave_x)
{
    return body->right() < leave_x;
}

bool LineSweep::right_less_than(const Body* body1, const Body* body2)
{
    return body1->right() < body2->right();
}
