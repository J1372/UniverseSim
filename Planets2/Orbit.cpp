#include "Orbit.h"

#include "raylib.h"
#include "Body.h"
#include <numbers>

void Orbit::set_periapsis(const Body& orbiter, float sat_dist)
{
    periapsis = sat_dist * (orbited.get_radius() + orbiter.get_radius());
}

float Orbit::apoapsis() const
{
    // I think there is another formula which handles eccentricity == 1 without an if check.

    if (eccentricity == 1.0f) { // would divide by 0.
        // Ecc == 1 means orbit is parabolic, apoapsis goes to infinity.
        return std::numeric_limits<float>::max();
    }

    return periapsis * (1.0f + eccentricity) / (1.0f - eccentricity);
}

float Orbit::semi_major_axis() const
{
    return (periapsis + apoapsis()) / 2;
}

float Orbit::translate_point(float point) const
{
    if (!prograde) {
        // Since retrograde orbit velocities are inverted, we should also invert this point.
        // Otherwise, the in between points (0, 0.5) (0.5, 1.0) will have different meanings depending on whether retrograde or prograde.

        point = 1 - point;
    }

    // Map point [0,1] in orbit to actual radian degree [0, 2pi].
    float radians = 2 * std::numbers::pi * point;

    return radians;
}

float Orbit::dist_at(float point) const
{
    // Get the radian degree from periapsis of this point in the orbit.
    float true_anomaly = translate_point(point);

    // Get the distance at the calculated true anomaly.
    float dist = semi_major_axis() * (1 - std::pow(eccentricity, 2)) / (1 + eccentricity * std::cos(true_anomaly));

    return dist;
}

Vector2 Orbit::pos_at(float point) const
{
    float dist = dist_at(point);

    // Get the radian degree from periapsis of this point in the orbit.
    float true_anomaly = translate_point(point);

    // Get angle of point in the orbit from central body.
    float from_central = periapsis_angle + true_anomaly;

    return { dist * std::cos(from_central), dist * std::sin(from_central) };

}

float Orbit::vel_at(float point) const
{
    float dist = dist_at(point);

    // Get the squared velocity at a point in its orbit using equation:
    // v2 = GM(2/r - 1/a), r == dist and a == semi major axis.
    float vel_sq = grav_const * orbited.get_mass() * (2 / dist - (1 / semi_major_axis()));

    float velocity = std::sqrt(vel_sq);

    if (prograde) {
        return velocity;
    }
    else {
        return -velocity;
    }
}

Vector2 Orbit::vel_vec_at(float point) const
{
    float velocity = vel_at(point);

    // Get the radian degree from periapsis of this point in the orbit.
    float true_anomaly = translate_point(point);

    // Get angle of point in the orbit from central body.
    float from_central = periapsis_angle + true_anomaly;

    // Flip on y-axis.
    float flipped = 2 * std::numbers::pi - from_central;

    return { velocity * std::sin(flipped), velocity * std::cos(flipped) };
}

float Orbit::orbital_period() const
{
    float numerator = 4 * std::pow(std::numbers::pi, 2) * std::pow(semi_major_axis(), 3);
    float denominator = grav_const * orbited.get_mass();
    return std::sqrt(numerator / denominator);
}
