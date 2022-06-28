#ifndef MY_RANDOM_H
#define MY_RANDOM_H

// Methods for convenient uniform random number generation.
namespace Rand {

	// Returns a float in range [0, 1)
	float real();

	// Returns a float in range [min, max)
	float real(float min, float max);

	// Returns a float in range [0, 2*pi).
	float radian();
	
	// Returns an int in range [min, max)
	int num(int min, int max);

	// Returns true if an event with the given possibility occurred.
	bool chance(float possibility);

	// Sets the seed to be used for producing random numbers.
	void set_seed(int number);

}

#endif