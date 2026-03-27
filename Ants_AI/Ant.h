#pragma once
#include "Random.h"
#include "Genome.h"
#include <sstream>
#include <glm/glm.hpp>

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
	inline glm::vec2 getPos() { return pos; }
	inline bool getHasFood() { return hasFood; }

	//used in main for fitness sorting
	inline static bool antCompare(Ant* a1, Ant* a2) { return (a1->fitness > a2->fitness); }
	//used in main to get genome as a string to write to file
	std::stringstream getGenomeString();

	//uses the world to fill sensory neurons with current values
	void sense(World*, float, float, float, float);
	//feeds data through the neural network from input neurons
	void think();
	//ant takes actions determined by actuator nodes in the world
	void act(World*);

	//used by the world to set ant location and direction
	void setup(int, int, char);
	//used by the world to set an ants location
	void changePos(int, int);
	//uses current position and direction to figure out the position infront of the ant
	glm::vec2 getForwardPos();
	glm::vec2 getLeftPos();
	glm::vec2 getBackPos();
	glm::vec2 getRightPos();
	//used by the world to give food to the ant and increase fitness score
	void giveFood();
	//used by the world to take food from the ant and increase fitness score
	void takeFood();

	void printNet();
private:
	//genome contains information for creating the ant's neural network
	Genome genome;

	//ant simulation properties
	int dir;
	glm::vec2 pos;
	bool hasFood;
	int fitness;

	//neural network
	float input[G_SN];
	float inputWeights[G_IW];
	float hidden[G_HN];
	float hiddenWeights[G_HW];
	float output[G_AN];

	//generates a random genome
	static Genome generateRandomGenome();
	//mutates genome by changing a small portion of it randomly
	Genome mutateGenome(Genome);
};