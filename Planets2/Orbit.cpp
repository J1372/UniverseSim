#include "Orbit.h"

#include "raylib.h"
#include "Body.h"
#include <numbers>

void Orbit::set_periapsis(float satellite_radius, float sat_dist)
{
    periapsis = sat_dist * (orbited.radius + satellite_radius);
}

Vector2 Orbit::periapsis_point() const
{
    return { periapsis * std::cos(periapsis_angle), periapsis * std::sin(periapsis_angle) };
}

float Orbit::apoapsis() const
{
    // ecc = 1.0 will divide by 0.
    return periapsis * (1 + eccentricity) / (1 - eccentricity);
}

float Orbit::semi_major_axis() const
{
    return (periapsis + apoapsis()) / 2;
}

float Orbit::velocity_periapsis(const Body& orbiter) const
{
    // all velocity at periapsis and apoapsis is tangental to body. 90 degrees = pi/2
    float num = (1 + eccentricity) * grav_const * (orbited.mass + orbiter.mass);
    float den = (1 - eccentricity) * semi_major_axis();
    float velocity_periapsis = std::sqrt(num / den);

    if (prograde) {
        return velocity_periapsis;
    }
    else {
        return -velocity_periapsis;
    }
}

Vector2 Orbit::velocity_periapsis_vector(const Body& orbiter) const
{
    float velocity = velocity_periapsis(orbiter);
    float angle = velocity_periapsis_angle();
    return { velocity * std::sin(angle), velocity * std::cos(angle) };
}

float Orbit::velocity_periapsis_angle() const
{
    return  2 * std::numbers::pi - periapsis_angle; // flip on y-axis
}
