#ifndef MY_RANDOM_H
#define MY_RANDOM_H

namespace Rand {

	// Returns a float in range [0, 1)
	float real();
	
	// Returns an int in range [min, max)
	int num(int min, int max);

	// Returns a float in range [0, 2*pi).
	float radian();

}

#endif