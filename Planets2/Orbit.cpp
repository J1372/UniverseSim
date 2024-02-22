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
    float rel_speed_sq = Vector2LengthSqr(rel_vel);

    float std_grav_param = grav_const * (orbited.get_mass() + orbiter.get_mass());

    float orb_energy = rel_speed_sq / 2.0f - std_grav_param / rel_dist;
    bool in_elliptical_orbit = orb_energy < 0.0f;

    if (in_elliptical_orbit)
    {
        // e is orb energy
        // a is semi-major axis
        // 
        // e = -std_grav_param / 2a
        // a * e = -std_grav_param / 2
        // a = -std_grav_param / 2e
        float semi_major_axis = -std_grav_param / (2.0f * orb_energy);

        Vector2 ecc_vec_temp_l = Vector2Scale(rel_pos, ((rel_speed_sq / std_grav_param) - 1.0f / rel_dist));
        Vector2 ecc_vec_temp_r = Vector2Scale(rel_vel, Vector2DotProduct(rel_pos, rel_vel) / std_grav_param);
        Vector2 ecc_vec = Vector2Subtract(ecc_vec_temp_l, ecc_vec_temp_r);
        eccentricity = Vector2Length(ecc_vec);
        periapsis = semi_major_axis * (1.0f - eccentricity);
        periapsis_angle = std::atan2(ecc_vec.y, ecc_vec.x);

        float angle_to_pos = std::atan2(rel_pos.y, rel_pos.x);
        Vector2 v_rot = Vector2Rotate(rel_vel, -angle_to_pos);

        float v_tan = -v_rot.y; // negative = clockwise tangential movement (to central body, not ellipse center)
        // auto v_rad = v_rot.x; // negative = moving towards parent body. positive = moving away
        
        // use angular velocity to determine clockwise or counterclockwise orbit
        // since rel_dist is always positive, and only care about sign, can just use v_tan.
        
        bool clockwise = v_tan < 0.0f; // negative ang vel = clockwise
        prograde = !clockwise;
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

float Orbit::semi_minor_axis() const
{
    return semi_major_axis() * std::sqrt(1 - eccentricity * eccentricity);
}

float Orbit::translate_point(float point) const
{
    if (!prograde) {
        // Since retrograde orbit velocities are inverted, we should also invert this point.
        // Otherwise, the in between points (0, 0.5) (0.5, 1.0) will have different meanings depending on whether retrograde or prograde.

        point = 1 - point;
    }

    // Map point [0,1] in orbit to actual radian degree [0, 2pi].
    float radians = (-2.0 * std::numbers::pi) * point;

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

Vector2 Orbit::get_ellipse_center() const
{
    float opp_per_angle = periapsis_angle - std::numbers::pi;
    float c_length = semi_major_axis() - periapsis;
    return Vector2Scale({ std::cos(opp_per_angle), std::sin(opp_per_angle) }, c_length); // rel to central body
}

float Orbit::speed_at(float point) const
{
    float dist = dist_at(point);

    // Get the squared velocity at a point in its orbit using equation:
    // v2 = GM(2/r - 1/a), r == dist and a == semi major axis.
    float speed_sq = grav_const * orbited->get_mass() * (2.0f / dist - (1.0f / semi_major_axis()));

    float speed = std::sqrt(speed_sq);
    return speed;
}

Vector2 Orbit::vel_at(float point) const
{
    float semi_maj = semi_major_axis();

    // ellipse center = {0,0}. periapsis is on line in dir pos x
    Vector2 foci_from_center{ semi_maj - periapsis, 0.0f };
    float true_anomaly = translate_point(point);
    float cur_dist = semi_maj * (1 - std::pow(eccentricity, 2)) / (1 + eccentricity * std::cos(true_anomaly));

    Vector2 cur_from_foci{ cur_dist * std::cos(true_anomaly), cur_dist * std::sin(true_anomaly) };
    Vector2 cur_from_center = Vector2Add(cur_from_foci, foci_from_center); // point on an ellipse

    float speed_sq = grav_const * orbited->get_mass() * (2.0f / cur_dist - (1.0f / semi_maj));
    float speed = std::sqrt(speed_sq);

    if (prograde)
    {
        speed = -speed;
    }

    // Avoid division by 0.
    if (cur_from_center.y == 0.0f)
    {
        // either at lowest or heighest.
        // lowest is in pos x direction from center ( 0,0 )
        if (cur_from_center.x > 0.0f)
        {
            return Vector2Rotate({ 0.0f, speed }, periapsis_angle);
        }
        else
        {
            return Vector2Rotate({ 0.0f, -speed }, periapsis_angle);
        }
    }

    // ellipse
    // dy/dx = -xb^2 / ya^2
    float dy_dx =
        -(cur_from_center.x * std::pow(semi_minor_axis(), 2))
        /
        (cur_from_center.y * std::pow(semi_maj, 2));

    float h = speed / std::sqrt(dy_dx * dy_dx + 1);

    if (cur_from_center.y > 0.0f)
    {
        h = -h;
    }

    Vector2 vel
    {
        h,
        dy_dx * h
    };

    return Vector2Rotate(vel, periapsis_angle);
}

float Orbit::orbital_period() const
{
    float numerator = 4 * std::pow(std::numbers::pi, 2) * std::pow(semi_major_axis(), 3);
    float denominator = grav_const * orbited->get_mass();
    return std::sqrt(numerator / denominator);
}
