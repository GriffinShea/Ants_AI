#include "Ant.h"
#include "World.h"	//here to avoid circular linking
#include <cstdlib>

extern const int MUTATION_RATE;

Ant::Ant() : Ant(generateRandomGenome()) { return; }

Ant::Ant(Genome g) {
	//save mutated genome
	genome = mutateGenome(g);
	
	//translate genome (char from 0 to 16) into weights (float from -4 to 4)
	for (int i = 0; i < G_IW; i++)
		inputWeights[i] = (float) (genome.inputWeights[i] - 8) / 2;
	for (int i = 0; i < G_HW; i++)
		hiddenWeights[i] = (float) (genome.hiddenWeights[i] - 8) / 2;
	
	//initialize values for the simulation
	hasFood = false;
	fitness = 0;

	return;
}

std::stringstream Ant::getGenomeString() {
	stringstream ss;
	for (int i = 0; i < G_IW; i++) ss << (char)('a' + genome.inputWeights[i]);
	for (int i = 0; i < G_HW; i++) ss << (char)('a' + genome.hiddenWeights[i]);
	return ss;
}

void Ant::sense(World* world, float step, float osc1, float osc2, float osc3) {
	//fill input nodes with data from world and self
	glm::vec2 forwardPos = getForwardPos();
	glm::vec2 leftPos = getLeftPos();
	glm::vec2 backPos = getBackPos();
	glm::vec2 rightPos = getRightPos();
	input[0] = world->readSignalA(pos[0], pos[1]);
	input[1] = world->readSignalB(pos[0], pos[1]);
	input[2] = world->readSignalA(forwardPos[0], forwardPos[1]);
	input[3] = world->readSignalB(forwardPos[0], forwardPos[1]);
	input[4] = world->readSignalA(leftPos[0], leftPos[1]);
	input[5] = world->readSignalB(leftPos[0], leftPos[1]);
	input[6] = world->readSignalA(backPos[0], backPos[1]);
	input[7] = world->readSignalB(backPos[0], backPos[1]);
	input[8] = world->readSignalA(rightPos[0], rightPos[1]);
	input[9] = world->readSignalB(rightPos[0], rightPos[1]);
	input[10] = (world->checkBlocked(forwardPos[0], forwardPos[1])) ? -1.0f : 1.0f;
	//if (dir < 3)					input[11] = 1;
	//else if (dir > 3 && dir < 7)	input[11] = -1;

	//if (dir > 5 || dir == 0)		input[12] = 1;
	//else if (dir > 1 && dir < 5)	input[12] = -1;
	input[11] = randFloat();
	//input[30] = step;
	input[12] = osc1;
	input[13] = osc2;
	input[14] = osc3;
	return;
}

void Ant::think() {
	//feed input to hidden layer
	for (int i = 0; i < G_HN; i++) {
		hidden[i] = 0;
		for (int j = 0; j < G_SN; j++) {
			hidden[i] += input[j] * inputWeights[j * G_HN + i];
		}
		hidden[i] = tanh(hidden[i]);
	}

	//feed hidden layer to output
	for (int i = 0; i < G_AN; i++) {
		output[i] = 0;
		for (int j = 0; j < G_HN; j++) {
			output[i] += hidden[j] * hiddenWeights[j * G_AN + i];
		}
		output[i] = tanh(output[i]);
	}

	return;
}

void Ant::act(World* world) {
	//make some actions dependant on actuator nodes

	//drop signal A
	if (output[0] > 0.0f) world->dropSignalA(pos[0], pos[1], output[1]);
	//drop signal B
	if (output[2] > 0.0f) world->dropSignalB(pos[0], pos[1], output[3]);
	//turn clockwise
	//output[4] = 0;
	if (output[4] > 0.0f && ++dir == 4) dir = 0;
	//turn counterclockwise
	//output[5] = 0;
	if (output[5] > 0.0f && --dir == -1) dir = 3;
	//move forward
	//output[6] = 1;
	if (output[6] > 0.0f) { world->moveAnt(this); }

	return;
}

void Ant::setup(int xPos, int yPos, char d) {
	changePos(xPos, yPos);
	dir = d;
	return;
}

void Ant::changePos(int xPos, int yPos) {
	pos[0] = xPos;
	pos[1] = yPos;
	return;
}

void Ant::giveFood() {
	hasFood = true;
	fitness++;
	return;
}

void Ant::takeFood() {
	hasFood = false;
	fitness++;
	fitness++;
	return;
}

glm::vec2 Ant::getForwardPos() {
	glm::vec2 fpos = glm::vec2(pos[0], pos[1]);

	switch (dir) {
	case 0:
		fpos.x++;
		break;
	case 1:
		fpos.y++;
		break;
	case 2:
		fpos.x--;
		break;
	case 3:
		fpos.y--;
		break;
	}

	//std::cout << pos[0] << "," << pos[1] << " " << fpos.x << "," << fpos.y << " " << dir << std::endl;

	return fpos;
}
glm::vec2 Ant::getLeftPos() {
	glm::vec2 lpos = glm::vec2(pos.x, pos.y);

	switch (dir) {
	case 0:
		lpos.y++;
		break;
	case 1:
		lpos.x--;
		break;
	case 2:
		lpos.y--;
		break;
	case 3:
		lpos.x++;
		break;
	}

	return lpos;
}
glm::vec2 Ant::getBackPos() {
	glm::vec2 bpos = glm::vec2(pos.x, pos.y);

	switch (dir) {
	case 0:
		bpos.x--;
		break;
	case 1:
		bpos.y--;
		break;
	case 2:
		bpos.x++;
		break;
	case 3:
		bpos.y++;
		break;
	}

	return bpos;
}
glm::vec2 Ant::getRightPos() {
	glm::vec2 rpos = glm::vec2(pos.x, pos.y);
	switch (dir) {
	case 0:
		rpos.y--;
		break;
	case 1:
		rpos.x++;
		break;
	case 2:
		rpos.y++;
		break;
	case 3:
		rpos.x--;
		break;
	}

	return rpos;
}

Genome Ant::generateRandomGenome() {
	Genome g;
	//for (int i = 0; i < G_IW; i++) g.inputWeights[i] = randCharInRange(0, 16);
	//for (int i = 0; i < G_HW; i++) g.hiddenWeights[i] = randCharInRange(0, 16);
	for (int i = 0; i < G_IW; i++) g.inputWeights[i] = 8;
	for (int i = 0; i < G_HW; i++) g.hiddenWeights[i] = 8;
	return g;
}

Genome Ant::mutateGenome(Genome g) {
	//change MUTATION_RATE random chars in the genome randomly
	int index;
	for (int i = 0; i < MUTATION_RATE; i++) {
		index = randIntInRange(0, G_IW + G_HW - 1);
		if (index < G_IW)	g.inputWeights[index]			= randCharInRange(0, 16);
		else				g.hiddenWeights[index - G_IW]	= randCharInRange(0, 16);
	}

	//increment or decrement MUTATION_RATE random chars
	/*int index;
	for (int i = 0; i < MUTATION_RATE; i++) {
		index = randIntInRange(0, G_IW + G_HW - 1);
		if (index < G_IW) {
			if (randFloat() > 0.5f) g.inputWeights[index] = min(16, g.inputWeights[index] + 1);
			else					g.inputWeights[index] = max(0, g.inputWeights[index] - 1);
		}
		else {
			if (randFloat() > 0.5f)	g.hiddenWeights[index - G_IW] = min(16, g.inputWeights[index - G_IW] + 1);
			else					g.hiddenWeights[index - G_IW] = max(0, g.inputWeights[index - G_IW] - 1);
		}
	}*/

	return g;
}

void Ant::printNet() {
	for (int i = 0; i < G_SN; i++) {
		cout << input[i] << ",";
	}
	cout << endl;
	for (int i = 0; i < G_HN; i++) {
		cout << hidden[i] << ",";
	}
	cout << endl;
	for (int i = 0; i < G_AN; i++) {
		cout << output[i] << ",";
	}
	cout << endl;
	cout << "current direction = " << (int)dir << endl;
	return;
}
