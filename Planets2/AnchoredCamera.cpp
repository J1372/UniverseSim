#include "AnchoredCamera.h"
#include "FreeCamera.h"
#include "Body.h"
#include "universe.h"

void AnchoredCamera::init(const AdvCamera& starting_config)
{
    camera = starting_config;

    // restrict panning so anchored body's center never goes off screen.
    camera.set_offset_bounds(0, 0, GetScreenWidth(), GetScreenHeight()); 

    if (anchored_to) {
        switch_to(nullptr);
    }
}

void AnchoredCamera::goto_body(Body& body)
{
    switch_to(body);
    snap_camera_to_target();
}

void AnchoredCamera::snap_camera_to_target()
{
    camera.set_target(anchored_to->pos());
}

CameraState* AnchoredCamera::goto_free_camera()
{
    FreeCamera& transition_to = CameraState::free_camera;
    transition_to.enter(camera);
    return &transition_to;
}

void AnchoredCamera::switch_to(Body* anchor_to)
{
    if (anchored_to) {
        // Remove camera as an observer to the anchored body's remove event.
        anchored_to->removal_event().rem_observer(listener_id);
    }

    anchored_to = anchor_to;

    if (anchor_to) {
        // Add camera as an observer to the new anchored body's remove event.
        listener_id = anchored_to->removal_event().add_observer(on_body_removal);
    }
}

void AnchoredCamera::switch_to(Body& anchor_to)
{
    if (anchored_to) {
        // Remove camera as an observer to the anchored body's remove event.
        anchored_to->removal_event().rem_observer(listener_id);
    }

    anchored_to = &anchor_to;

    // Add camera as an observer to the new anchored body's remove event.
    listener_id = anchored_to->removal_event().add_observer(on_body_removal);

}

CameraState* AnchoredCamera::update(const Universe& universe)
{
    // Handle user switching to different bodies.
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 screen_point = GetMousePosition();
        Vector2 universe_point = GetScreenToWorld2D(screen_point, camera.get_raylib_camera());

        Body* body = universe.get_body(universe_point);

        switch_to(body);
    }
    
    // Handle state transitions
    // anchored_to == nullptr if user right clicked on nothing or body deleted and was not absorbed by another body.
    // in any case, no body to anchor to, so return to a free camera state.
    if (!anchored_to) {
        return goto_free_camera();
    }

    // Will remain in an anchored camera state, so update camera and handle other user camera input.

    // Update camera to follow the body it is anchored to.
    snap_camera_to_target();

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

void AnchoredCamera::enter(const AdvCamera& prev_camera, Body& anchor_to)
{
    switch_to(anchor_to);

    const Camera2D& ray_cam = prev_camera.get_raylib_camera();

    // to keep camera still instead of snapping target to center, set
    // offset to the targets original screen_pos

    camera.set_offset(ray_cam.offset);
    camera.set_target(anchored_to->pos());
    camera.set_zoom(ray_cam.zoom);

}
