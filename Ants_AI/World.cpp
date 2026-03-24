#include "World.h"

#define PIX2 6.2831853

extern const int SIMULATION_ITERATIONS;

World::GridSquare::GridSquare() {
	ant = NULL;
	//temperature = -200;
	//obstacle = false;
	hasFood = false;
	isHive = false;
	hasAnt = false;

	hasSignalA = false;
	hasSignalB = false;

	lastSignalIsA = false;

	signalACount = 0;
	signalBCount = 0;

	return;
}

World::World(int w, int p, Ant** a, int foodCount) {
	population = p;
	ants = a;
	worldSize = w;

	vector<vector<GridSquare> > setGrid(worldSize, vector<GridSquare>(worldSize));
	grid = setGrid;

	setupHive();
	addFood(foodCount);
	return;
}

void World::simulateStep(int step) {
	float stepFloat = ((float)step / SIMULATION_ITERATIONS) * 2 - 1;
	float oscFloat = sin((float)(step % 20) / 20.0f * PIX2);

	for (int i = 0; i < population; i++) {
		ants[i]->sense(this, stepFloat, oscFloat);
		ants[i]->think();
		ants[i]->act(this);

		int* pos = ants[i]->getPos();
		bool hasFood = ants[i]->getHasFood();

		//if an ant walks over food while not carrying anything, it picks up the food
		if (!hasFood && grid[pos[0]][pos[1]].hasFood) {
			ants[i]->giveFood();
			grid[pos[0]][pos[1]].hasFood = false;
		}

		//if an ant walks over the hive while carrying food, the food is delivered
		if (hasFood && grid[pos[0]][pos[1]].isHive) {
			ants[i]->takeFood();
		}

	}
	return;
}

void World::dropSignalA(int x, int y, float signal) {
	grid[x][y].signalA = signal;
	grid[x][y].hasSignalA = true;
	grid[x][y].signalACount++;
	grid[x][y].lastSignalIsA = true;
	return;
}

void World::dropSignalB(int x, int y, float signal) {
	grid[x][y].signalB = signal;
	grid[x][y].hasSignalB = true;
	grid[x][y].signalBCount++;
	grid[x][y].lastSignalIsA = false;
	return;
}

bool World::moveAnt(Ant* ant) {
	int* oldPos = ant->getPos();
	int* newPos = ant->getForwardPos();
	int newPosX = newPos[0];
	int newPosY = newPos[1];

	if (!checkBlocked(newPosX, newPosY)) {
		grid[oldPos[0]][oldPos[1]].hasAnt = false;
		grid[oldPos[0]][oldPos[1]].ant = NULL;
		grid[newPosX][newPosY].hasAnt = true;
		grid[newPosX][newPosY].ant = ant;
		ant->changePos(newPosX, newPosY);
		return true;
	}
	else return false;
}

bool World::checkFood(int x, int y) {
	return grid[x][y].hasFood;
};

bool World::checkHive(int x, int y) {
	return grid[x][y].isHive;
};

bool World::checkSignalA(int x, int y) {
	return grid[x][y].hasSignalA;
};

float World::readSignalA(int x, int y) {
	return grid[x][y].signalA;
};

bool World::checkSignalB(int x, int y) {
	return grid[x][y].hasSignalB;
};

float World::readSignalB(int x, int y) {
	return grid[x][y].signalB;
};

bool World::checkBlocked(int x, int y) {
	return x < 0 || x >= worldSize || y < 0 || y >= worldSize || grid[x][y].hasAnt;
};

bool World::containsAnt(int x, int y) {
	return grid[x][y].hasAnt;
}

void World::printGrid() {
	string s;
	for (int i = 0; i < worldSize; ++i)
	{
		for (int j = 0; j < worldSize; ++j)
		{
			s = grid[i][j].hasFood ? "true" : "false";
			cout << s << " ";
		}
		cout << i << endl;
	}
}

void World::generateArea(int size, int areaType) {
	int x = randIntInRange(size, worldSize - size);
	int y = randIntInRange(size, worldSize - size);
	int radiusSQ = (size * size) / 4;
	int radius = (size * 2) + 1;
	int threshold, d;
	for (int i = 0; i < worldSize; ++i) {
		threshold = radiusSQ - ((i - y) * (i - y));
		for (int j = 0; j < worldSize; ++j)
		{
			d = j - x;
			switch (areaType) {
			case TYPE_HIVE:
				if ((d * d) > threshold) {
					grid[i][j].isHive = false;
				}
				else {
					grid[i][j].isHive = true;
				}
				break;
			case TYPE_FOOD:
				if ((d * d) > threshold) {
					grid[i][j].hasFood = false;
				}
				else {
					grid[i][j].hasFood = true;
				}
				break;
			default:
				break;
			}
		}
	}
}

void World::setupHive() {
	int k = 0;
	int hiveSize = population / 4;
	for (int i = (worldSize - hiveSize) / 2; i < (worldSize + hiveSize) / 2; i++) {\
		grid[i][(worldSize - hiveSize) / 2 - 1].hasAnt = true;
		grid[i][(worldSize - hiveSize) / 2 - 1].ant = ants[k];
		ants[k++]->setup(i, (worldSize - hiveSize) / 2 - 1, 3);

		grid[i][(worldSize + hiveSize) / 2].hasAnt = true;
		grid[i][(worldSize + hiveSize) / 2].ant = ants[k];
		ants[k++]->setup(i, (worldSize + hiveSize) / 2, 7);

		grid[(worldSize - hiveSize) / 2 - 1][i].hasAnt = true;
		grid[(worldSize - hiveSize) / 2 - 1][i].ant = ants[k];
		ants[k++]->setup((worldSize - hiveSize) / 2 - 1, i, 5);

		grid[(worldSize + hiveSize) / 2][i].hasAnt = true;
		grid[(worldSize + hiveSize) / 2][i].ant = ants[k];
		ants[k++]->setup((worldSize + hiveSize) / 2, i, 1);

		for (int j = (worldSize - hiveSize) / 2; j < (worldSize + hiveSize) / 2; j++) {
			grid[i][j].isHive = true;
		}
	}

	return;
}

void World::addFood(int foodCount) {
	//pick the center around which to generate the food circle (somewhere along an edge)
	int foodPos[2];
	foodPos[0] = randIntInRange(0, worldSize);
	foodPos[1] = randIntInRange(0, worldSize);
	bool clampedOnY = randFloat() > 0.5f;
	if (clampedOnY)	foodPos[0] = (foodPos[0] > worldSize / 2) ? 0 : worldSize;
	else			foodPos[1] = (foodPos[1] > worldSize / 2) ? 0 : worldSize;

	//add food to the grid in places within the circle
	float radiusSqared = (float(foodCount * 4) / PIX2) / 2;
	int x, y, k = 0;
	for (int i = -int(radiusSqared); i < int(radiusSqared); i++) {
		x = foodPos[0] + i;
		if (x >= worldSize)	x -= worldSize;
		else if (x < 0)		x += worldSize;

		for (int j = -int(radiusSqared); j < int(radiusSqared); j++) {
			y = foodPos[1] + j;
			if (y >= worldSize)	y -= worldSize;
			else if (y < 0)		y += worldSize;

			if (i * i + j * j < radiusSqared) {
				grid[x][y].hasFood = true;
				k++;
			}
		}
	}

	return;
}

/*
void World::generateHeatMap() {
	int xArr[2];
	int yArr[2];
	int x, y, threshold, d, counter, temperature, radius, radiusSQ, tempInit;
	srand(time(0));
	for (int i = 0; i < 2; i++) {
		xArr[i] = rand() % (worldSize - 1);
	}
	for (int j = 0; j < 2; j++) {
		yArr[j] = rand() % (worldSize - 1);
	}
	while ((abs(xArr[0] - xArr[1]) <= 20) && (abs(yArr[0] == yArr[1]) <= 20)) {
		xArr[1] = rand() % (worldSize - 1);
		yArr[1] = rand() % (worldSize - 1);
	}

	for (int n = 0; n < 2; n++) {
		x = xArr[n];
		y = yArr[n];
		radius = HEAT_MAP_RADIUS;
		counter = 0;
		tempInit = (rand() % MAX_TEMP) + MIN_TEMP;
		temperature = tempInit;
		while (radius <= worldSize * 3) {
			radius = HEAT_MAP_RADIUS + 10 * counter;
			radius = (radius * 2) + 1;
			radiusSQ = (radius * radius) / 4;
			for (int i = 0; i < worldSize; i++) {
				threshold = radiusSQ - ((i - y) * (i - y));
				for (int j = 0; j < worldSize; j++)
				{
					d = j - x;
					if (grid[i][j].temperature != -500) {
						grid[i][j].temperature = ((d * d) > threshold) ? grid[i][j].temperature : (grid[i][j].temperature + temperature) / 2;
					}
					else {
						grid[i][j].temperature = ((d * d) > threshold) ? grid[i][j].temperature : temperature;
					}
				}
			}
			if (temperature < 0) {
				temperature = temperature + 5;
			}
			else {
				temperature = temperature - 5;
			}
			counter = counter + 1;
		}
	}
}
*/


