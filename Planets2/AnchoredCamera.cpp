#include "AnchoredCamera.h"
#include "FreeCamera.h"
#include "Body.h"
#include "Universe.h"

AnchoredCamera::AnchoredCamera(const AdvCamera& starting_config, Universe& universe)
{
    camera = starting_config;

    // restrict panning so anchored body's center never goes off screen.
    camera.set_offset_bounds(0, 0, GetScreenWidth(), GetScreenHeight());

}
void AnchoredCamera::goto_body(Body& body)
{
    switch_to(body.get_id());
    snap_camera_to_target(body);
}
void AnchoredCamera::snap_camera_to_target(const Body& target)
{
    camera.set_target(target.pos());
}

CameraState* AnchoredCamera::goto_free_camera()
{
    FreeCamera& transition_to = CameraState::free_camera;
    transition_to.enter(camera);
    return &transition_to;
}

void AnchoredCamera::unanchor()
{
    anchored_to = -1;
}

bool AnchoredCamera::is_anchored() const
{
    return anchored_to != -1;
}

void AnchoredCamera::switch_to(int anchor_to)
{
    anchored_to = anchor_to;
}

CameraState* AnchoredCamera::update(Universe& universe)
{
    // Handle user switching to different bodies.
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 screen_point = GetMousePosition();
        Vector2 universe_point = GetScreenToWorld2D(screen_point, camera.get_raylib_camera());

        const Body* body = universe.get_body(universe_point);

        if (body)
        {
            switch_to(body->get_id());
        }
        else
        {
            unanchor();
        }
    }
    
    // Handle state transitions
    // anchored_to == nullptr if user right clicked on nothing or body deleted and was not absorbed by another body.
    // in any case, no body to anchor to, so return to a free camera state.
    if (!is_anchored()) {
        exit(universe);
        return goto_free_camera();
    }

    // Will remain in an anchored camera state, so update camera and handle other user camera input.

    // Update camera to follow the body it is anchored to.
    const Body& body = *universe.get_body(anchored_to); // Guaranteed valid id.
    snap_camera_to_target(body);

    // Camera movement, offset to the target body
    // The center of the anchored body is never able to go off screen.

    if (IsKeyDown(KEY_W)) {
        camera.move_offset(Direction::DOWN);
    }
    if (IsKeyDown(KEY_A)) {
        camera.move_offset(Direction::RIGHT);
    }
    if (IsKeyDown(KEY_S)) {
        camera.move_offset(Direction::UP);
    }
    if (IsKeyDown(KEY_D)) {
        camera.move_offset(Direction::LEFT);
    }

    // Camera speed

    if (IsKeyPressed(KEY_MINUS)) {
        camera.decrease_speed_offset();
    }
    else if (IsKeyPressed(KEY_EQUAL)) {
        camera.increase_speed_offset();
    }

    // Camera zoom for keys and mousewheel

    float wheel_move = GetMouseWheelMove();
    if (wheel_move < 0 or IsKeyPressed(KEY_COMMA)) {
        camera.zoom_out();
    }
    else if (wheel_move > 0 or IsKeyPressed(KEY_PERIOD)) {
        camera.zoom_in();
    }

    // We are still in an anchored camera state, so return this.
    return this;
}

void AnchoredCamera::enter(const AdvCamera& prev_camera, const Body& anchor_to, Universe& universe)
{
    // Add a listener, so that we update when the planet we are attached to
    // is removed.
    listener_id = universe.removal_event().add_observer(
        [this](Removal remove_event)
        {
            if (anchored_to == remove_event.removed)
            {
                // Since absorbed_by == -1 if just deleted, could switch to it without check.
                // since -1 will transition out of anchored camera state on update.

                if (remove_event.was_absorbed())
                {
                    this->switch_to(remove_event.absorbed_by);
                }
                else
                {
                    unanchor();
                }
            }
        });

    switch_to(anchor_to.get_id());

    const Camera2D& ray_cam = prev_camera.get_raylib_camera();

    // note if wanted:
    // to keep camera still instead of snapping target to center, set
    // offset to the targets original screen_pos

    camera.set_offset(ray_cam.offset);
    camera.set_target(anchor_to.pos());
    camera.set_zoom(ray_cam.zoom);

    // update offset bounds in case screen was resized while another camera was active.
    camera.set_offset_bounds(0, 0, GetScreenWidth(), GetScreenHeight());

}

void AnchoredCamera::exit(Universe& universe)
{
    if (listener_id.has_value()) {
        universe.removal_event().rem_observer(listener_id.value());
        listener_id.reset();
    }
}

void AnchoredCamera::notify_resize(int width, int height)
{
    camera.set_offset_bounds(0, 0, width, height);
}
