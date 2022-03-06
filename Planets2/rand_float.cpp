#include <random>
#include "rand_float.h"

float randf()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(0, 1); // rage 0 - 1
    return dis(e);
}

int randi(int min, int max) {
    //static std::random_device dev;
    //static std::mt19937 e;
    //std::uniform_int_distribution<std::mt19937::result_type> dist(min, max); // distribution in range [1, 6]

    int dif = max - min; // TODO: FIX THIS TO BE AN INT GENERATOR INDEPENDENT OF RANDF.
    return min + (int)(randf() * dif);
}