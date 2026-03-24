#pragma once
#include "Random.h"
#include "Genome.h"
#include <sstream>

class World;	//needed to avoid circular linking
class Ant
{
public:
	//creates ant with random genome
	Ant();
	//creates ant with given genome of a previous generation (must be mutated)
	Ant(Genome);

	inline int getFitness() { return fitness; };
	inline Genome getGenome() { return genome; };
	inline int* getPos() { return pos; }
	inline bool getHasFood() { return hasFood; }

	//used in main for fitness sorting
	inline static bool antCompare(Ant* a1, Ant* a2) { return (a1->fitness > a2->fitness); }
	//used in main to get genome as a string to write to file
	std::stringstream getGenomeString();

	//uses the world to fill sensory neurons with current values
	void sense(World*, float, float);
	//feeds data through the neural network from input neurons
	void think();
	//ant takes actions determined by actuator nodes in the world
	void act(World*);

	//used by the world to set ant location and direction
	void setup(int, int, char);
	//used by the world to set an ants location
	void changePos(int, int);
	//uses current position and direction to figure out the position infront of the ant
	int* getForwardPos();
	//used by the world to give food to the ant and increase fitness score
	void giveFood();
	//used by the world to take food from the ant and increase fitness score
	void takeFood();

	void printNet();
private:
	//genome contains information for creating the ant's neural network
	Genome genome;

	//ant simulation properties
	char dir;
	int pos[2];
	bool hasFood;
	int fitness;

	//neural network
	float input[10];
	float inputWeights[80];
	float hidden[8];
	float hiddenWeights[56];
	float output[7];

	//generates a random genome
	static Genome generateRandomGenome();
	//mutates genome by changing a small portion of it randomly
	Genome mutateGenome(Genome);
};