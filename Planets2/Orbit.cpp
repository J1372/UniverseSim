#include "Orbit.h"

#include "raylib.h"
#include "Body.h"
#include <numbers>
#include <cmath>
#include <raymath.h>

Orbit::Orbit(const Body& orbited) 
    : orbited(&orbited)
{}

Orbit::Orbit(const Body& orbited, const Body& orbiter, float grav_const)
    : orbited(&orbited), grav_const(grav_const)
{
    Vector2 rel_pos = orbited.distv(orbiter);
    Vector2 rel_vel = orbiter.vel_relative(orbited);

    float rel_dist = Vector2Length(rel_pos);
    float rel_speed = Vector2Length(rel_vel);
    float rel_speed_sq = rel_speed * rel_speed;

    float std_grav_param = grav_const * (orbited.get_mass() + orbiter.get_mass());

    float orb_energy = rel_speed_sq / 2.0f - std_grav_param / rel_dist;
    bool in_elliptical_orbit = orb_energy < 0.0f;

    if (in_elliptical_orbit)
    {
        // e is orb energy
        // a is semi-major axis
        // 
        // e = -std_grav_param / 2a
        // 2a * e = -std_grav_param
        // 2a = -std_grav_param / e
        // a = -std_grav_param / 2e
        float semi_major_axis = -std_grav_param / (2.0f * orb_energy);

        Vector2 ecc_vec_temp_l = Vector2Scale(rel_pos, ((rel_speed_sq / std_grav_param) - 1.0f / rel_dist));
        Vector2 ecc_vec_temp_r = Vector2Scale(rel_vel, Vector2DotProduct(rel_pos, rel_vel) / std_grav_param);
        Vector2 ecc_vec = Vector2Subtract(ecc_vec_temp_l, ecc_vec_temp_r);
        eccentricity = Vector2Length(ecc_vec);
        periapsis = semi_major_axis * (1.0f - eccentricity);
        periapsis_angle = atan2(ecc_vec.y, ecc_vec.x);
    }
    else
    {
        //...
    }
}

void Orbit::set_periapsis(const Body& orbiter, float sat_dist)
{
    periapsis = sat_dist * (orbited->get_radius() + orbiter.get_radius());
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

float Orbit::speed_at(float point) const
{
    float dist = dist_at(point);

    // Get the squared velocity at a point in its orbit using equation:
    // v2 = GM(2/r - 1/a), r == dist and a == semi major axis.
    float speed_sq = grav_const * orbited->get_mass() * (2 / dist - (1 / semi_major_axis()));

    float speed = std::sqrt(speed_sq);
    return speed;
}

Vector2 Orbit::vel_at(float point) const
{
    float speed = speed_at(point);

    // Get the radian degree from periapsis of this point in the orbit.
    float true_anomaly = translate_point(point);

    // Get angle of point in the orbit from central body.
    float from_central = periapsis_angle + true_anomaly;

    if (!prograde) {
        speed = -speed;
    }

    return { speed * -std::sin(from_central), speed * std::cos(from_central) };
}

float Orbit::orbital_period() const
{
    float numerator = 4 * std::pow(std::numbers::pi, 2) * std::pow(semi_major_axis(), 3);
    float denominator = grav_const * orbited->get_mass();
    return std::sqrt(numerator / denominator);
}
