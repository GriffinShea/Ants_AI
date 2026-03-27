#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

namespace rand_funcs {
	int randIntInRange(int min, int max) {
		return (rand() % (max + 1)) + min;
	}
	char randCharInRange(char min, char max) {
		return (rand() % (max + 1)) + min;
	}
	float randFloat() {
		return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2 - 1;
	}
}
