#include "World.h"
#include <glm/glm.hpp>

#define PIX2 6.2831853

World::GridSquare::GridSquare() {
	ant = NULL;
	//temperature = -200;
	//obstacle = false;
	hasFood = false;
	isHive = false;
	hasAnt = false;

	signalA = 0;
	signalB = 0;

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

void World::simulateStep(int step, int simIterations) {
	float stepFloat = ((float)step / simIterations) * 2 - 1;
	float osc1 = sin((float)(step % 4) / 4.0f * PIX2);
	float osc2 = sin((float)(step % 8) / 8.0f * PIX2);
	float osc3 = sin((float)(step % 16) / 16.0f * PIX2);

	for (int i = 0; i < population; i++) {
		ants[i]->sense(this, stepFloat, osc1, osc2, osc3);
		ants[i]->think();
		ants[i]->act(this);

		glm::vec2 pos = ants[i]->getPos();
		bool hasFood = ants[i]->getHasFood();

		//if an ant walks over food while not carrying anything, it picks up the food
		if (!hasFood && grid[pos.x][pos.y].hasFood) {
			ants[i]->giveFood();
			grid[pos.x][pos.y].hasFood = false;
		}

		//if an ant walks over the hive while carrying food, the food is delivered
		if (hasFood && grid[pos.x][pos.y].isHive) {
			ants[i]->takeFood();
		}

	}
	return;
}

void World::dropSignalA(int x, int y, float signal) {
	grid[x][y].signalA = signal;
	return;
}

void World::dropSignalB(int x, int y, float signal) {
	grid[x][y].signalB = signal;
	return;
}

bool World::moveAnt(Ant* ant) {
	glm::vec2 oldPos = ant->getPos();
	glm::vec2 newPos = ant->getForwardPos();

	if (!checkBlocked(newPos.x, newPos.y)) {
		grid[oldPos.x][oldPos.y].hasAnt = false;
		grid[oldPos.x][oldPos.y].ant = NULL;
		grid[newPos.x][newPos.y].hasAnt = true;
		grid[newPos.x][newPos.y].ant = ant;
		ant->changePos(newPos.x, newPos.y);
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

float World::readSignalA(int x, int y) {
	if (x < 0 || x >= worldSize || y < 0 || y >= worldSize) return 0;
	else return grid[x][y].signalA;
};

float World::readSignalB(int x, int y) {
	if (x < 0 || x >= worldSize || y < 0 || y >= worldSize) return 0;
	else return grid[x][y].signalB;
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


void World::setupHive() {
	int k = 0;
	int hiveSize = worldSize / 8;
	for (int i = (worldSize - hiveSize) / 2; i < (worldSize + hiveSize) / 2; i = i + 2) {
		grid[i][(worldSize - hiveSize) / 2 - 1].hasAnt = true;
		grid[i][(worldSize - hiveSize) / 2 - 1].ant = ants[k];
		ants[k++]->setup(i, (worldSize - hiveSize) / 2 - 1, 3);

		grid[i][(worldSize + hiveSize) / 2].hasAnt = true;
		grid[i][(worldSize + hiveSize) / 2].ant = ants[k];
		ants[k++]->setup(i, (worldSize + hiveSize) / 2, 1);

		grid[(worldSize - hiveSize) / 2 - 1][i].hasAnt = true;
		grid[(worldSize - hiveSize) / 2 - 1][i].ant = ants[k];
		ants[k++]->setup((worldSize - hiveSize) / 2 - 1, i, 2);

		grid[(worldSize + hiveSize) / 2][i].hasAnt = true;
		grid[(worldSize + hiveSize) / 2][i].ant = ants[k];
		ants[k++]->setup((worldSize + hiveSize) / 2, i, 0);
	}
	for (int i = (worldSize - hiveSize) / 2; i < (worldSize + hiveSize) / 2; i++)
		for (int j = (worldSize - hiveSize) / 2; j < (worldSize + hiveSize) / 2; j++)
			grid[i][j].isHive = true;

	return;
}

void World::addFood(int foodCount) {
	//pick the center around which to generate the food circle (somewhere along an edge)
	int foodPos[2];
	int angle = randIntInRange(25, 65);
	foodPos[1] = worldSize * (0.33 * glm::sin(glm::radians((float)angle)));
	foodPos[0] = worldSize * (0.33 * glm::cos(glm::radians((float)angle)));
	
	//foodPos[0] = randIntInRange(0, worldSize);
	//foodPos[1] = randIntInRange(0, worldSize);
	//bool clampedOnY = randFloat() > 0.5f;
	//if (clampedOnY)	foodPos[0] = (foodPos[0] > worldSize / 2) ? 0 : worldSize;
	//else			foodPos[1] = (foodPos[1] > worldSize / 2) ? 0 : worldSize;

	for (int k = -1; k < 2; k = k + 2) {
		for (int l = -1; l < 2; l = l + 2) {
			//add food to the grid in places within the circle
			float radiusSqared = (float(foodCount) / PIX2) / 2;
			int x, y, z = 0;
			for (int i = -int(radiusSqared); i < int(radiusSqared); i++) {
				x = k * foodPos[0] + worldSize / 2 + i;
				//x = foodPos[0] + worldSize / 2 + i;
				if (x >= worldSize)	x -= worldSize;
				else if (x < 0)		x += worldSize;

				for (int j = -int(radiusSqared); j < int(radiusSqared); j++) {
					y = l * foodPos[1] + worldSize / 2 + j;
					//y = foodPos[1] + worldSize / 2 + j;
					if (y >= worldSize)	y -= worldSize;
					else if (y < 0)		y += worldSize;

					if (i * i + j * j < radiusSqared) {
						grid[x][y].hasFood = true;
						z++;
					}
				}
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


