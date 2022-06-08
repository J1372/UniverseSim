#include <random>
#include "my_random.h"
#include <numbers>

static std::default_random_engine engine;

float Rand::real()
{
    static std::uniform_real_distribution<> real_dist(0, 1);
    return real_dist(engine);
}

float Rand::real(float min, float max)
{
    return min + Rand::real() * (max - min);
}

float Rand::radian()
{
    return Rand::real() * 2 * std::numbers::pi;
}

int Rand::num(int min, int max)
{
    //static std::random_device dev;
    //static std::mt19937 e;
    //std::uniform_int_distribution<std::mt19937::result_type> dist(min, max); // distribution in range [1, 6]

    int dif = max - min; // TODO: FIX THIS TO BE AN INT GENERATOR INDEPENDENT OF RANDF.
    return min + (int)(Rand::real() * dif);
}

bool Rand::chance(float possibility)
{
    return Rand::real() < possibility;
}

void Rand::set_seed(int number)
{
    engine.seed(number);
}
