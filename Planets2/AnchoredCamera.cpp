#include "AnchoredCamera.h"
#include "FreeCamera.h"
#include "Body.h"
#include "universe.h"
#include <utility>

AnchoredCamera::AnchoredCamera(Camera2D&& camera, int cam_speed_multiplier, const Body& anchor_to) : CameraState{std::move(camera), cam_speed_multiplier}, anchored_to(&anchor_to)
{
}

CameraState* AnchoredCamera::update(const Universe& universe)
{
    // Update camera to follow the body it is anchored to.
    camera.target.x = anchored_to->x;
    camera.target.y = anchored_to->y;

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 screen_point = GetMousePosition();
        Vector2 universe_point = GetScreenToWorld2D(screen_point, camera);

        Body* body = universe.get_body(universe_point);

        if (body) {
            std::cout << body->id << '\n';
            anchored_to = body;
        }
        else {
            ret_state = new FreeCamera{ std::move(camera), cam_speed_multiplier };
        }
    }

    // Camera zoom for keys and mousewheel

    float wheel_move = GetMouseWheelMove();
    if (wheel_move < 0 or IsKeyPressed(KEY_COMMA)) {
        zoom_out();
        cam_speed = 5 * cam_speed_multiplier / camera.zoom;
    }
    else if (wheel_move > 0 or IsKeyPressed(KEY_PERIOD)) {
        zoom_in();
        cam_speed = 5 * cam_speed_multiplier / camera.zoom;
    }

    return ret_state;
}

void AnchoredCamera::resize(float width, float height)
{
}

void AnchoredCamera::notify_body_no_longer_exists(const Body* absorbed_by)
{
    if (absorbed_by) {
        anchored_to = absorbed_by;
    }
    else {
        ret_state = new FreeCamera{ std::move(camera), cam_speed_multiplier };
    }
}
