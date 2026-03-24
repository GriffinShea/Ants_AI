#pragma once
#include "Ant.h"
#include "Random.h"
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;
using namespace rand_funcs;

const int HEAT_MAP_RADIUS = 20;
const int MIN_TEMP = -20;
const int MAX_TEMP = 50;
const int TYPE_HIVE = 1;
const int TYPE_FOOD = 2;
const char directions[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

class World
{
public:
	class GridSquare {
	public:
		//int temperature;
		Ant* ant;

		//bool obstacle;
		bool hasFood;
		bool isHive;
		bool hasAnt;

		bool hasSignalA;
		float signalA;
		bool hasSignalB;
		float signalB;

		bool lastSignalIsA;

		int signalACount; //cumulative 
		int signalBCount; //cumulative

		GridSquare();
	};

	//create world according to given parameters, put ants in the world
	World(int, int, Ant**, int);

	//used in main to run one step of the simulation
	void simulateStep(int, int);

	//used by an ant to attempt to move itself
	bool moveAnt(Ant*);
	//used by an ant to drop an A signal at location
	void dropSignalA(int, int, float);
	//used by an ant to drop an B signal at location
	void dropSignalB(int, int, float);


	//methods for ants to use to sense the world
	bool checkFood(int x, int y);
	bool checkHive(int x, int y);
	bool checkSignalA(int x, int y);
	float readSignalA(int x, int y);
	bool checkSignalB(int x, int y);
	float readSignalB(int x, int y);
	bool checkBlocked(int x, int y);
	bool containsAnt(int x, int y);
	inline Ant* getAnt(int x, int y) { return grid[x][y].ant; }
	inline bool showSignalA(int x, int y) { return grid[x][y].lastSignalIsA; }
	void printGrid();
private:
	//world size and GridSquare array representing the world
	int worldSize;
	vector<vector<GridSquare>> grid;
	//ant population and ant pointer array
	int population;
	Ant** ants;

	//place the ants in the world and setup the hive
	void setupHive();
	//adds a food supply to the world
	void addFood(int);

	//methods to generate the world grid
	void generateArea(int size, int AreaType);
	//void generateHeatMap();
};

