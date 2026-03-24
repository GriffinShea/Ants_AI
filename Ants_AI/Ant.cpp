#include "Ant.h"
#include "World.h"	//here to avoid circular linking
#include <cstdlib>

extern const int MUTATION_RATE;

Ant::Ant() : Ant(generateRandomGenome()) { return; }

Ant::Ant(Genome g) {
	//mutate genome then set neural network weights from genome
	genome = mutateGenome(g);
	//WEIGHT SETUP
	for (int i = 0; i < 80; i++) inputWeights[i] = (float)genome.inputWeights[i] / 4 - 2;
	for (int i = 0; i < 56; i++) hiddenWeights[i] = (float)genome.hiddenWeights[i] / 4 - 2;
	//for (int i = 0; i < 80; i++) inputWeights[i] = (float)genome.inputWeights[i] / 8 - 1;
	//for (int i = 0; i < 56; i++) hiddenWeights[i] = (float)genome.hiddenWeights[i] / 8 - 1;
	//initialize values for the simulation
	dir = randIntInRange(0, 8);
	hasFood = false;
	pos[0] = 0;
	pos[1] = 0;
	fitness = 0;

	return;
}

std::stringstream Ant::getGenomeString() {
	stringstream ss;
	for (int i = 0; i < 80; i++) ss << (char)('a' + genome.inputWeights[i]);
	for (int i = 0; i < 56; i++) ss << (char)('a' + genome.hiddenWeights[i]);
	return ss;
}

void Ant::sense(World* world, float step, float osc) {
	//fill input nodes with data from world and self
	int* forwardPos = getForwardPos();
	input[0] = (world->checkSignalA(pos[0], pos[1])) ? -1.0f : 1.0f;
	input[1] = world->readSignalA(pos[0], pos[1]);
	input[2] = (world->checkSignalB(pos[0], pos[1])) ? -1.0f : 1.0f;
	input[3] = world->readSignalB(pos[0], pos[1]);
	input[4] = (world->checkBlocked(forwardPos[0], forwardPos[1])) ? -1.0f : 1.0f;
	input[6] = ((float)dir / 7) * 2 - 1;
	input[7] = randFloat();
	input[8] = step;
	input[9] = osc;
	return;
}

void Ant::think() {
	//feed input to hidden layer
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 10; j++) {
			hidden[i] += input[j] * inputWeights[j * 8 + i] / 16;
		}
		hidden[i] = tanh(hidden[i]);
	}

	//feed hidden layer to output
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 8; j++) {
			output[i] += hidden[j] * hiddenWeights[j * 7 + i] / 16;
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
	if (output[4] > 0.0f && ++dir == 8) dir = 0;

	//turn counterclockwise
	if (output[5] > 0.0f && --dir == -1) dir = 7;

	//move forward
	if (output[6] > 0.0f) {
		int* forwardPos = getForwardPos();
		world->moveAnt(this);
	}

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
	return;
}

int* Ant::getForwardPos() {
	int forwardPos[2] = { pos[0], pos[1] };

	if (dir < 3)					forwardPos[0]++;
	else if (dir > 3 && dir < 7)	forwardPos[0]--;

	if (dir > 5 || dir == 0)		forwardPos[1]++;
	else if (dir > 1 && dir < 5)	forwardPos[1]--;

	return forwardPos;
}

Genome Ant::generateRandomGenome() {
	Genome g;
	//INITIAL GENOME SETUP
	//for (int i = 0; i < 80; i++) g.inputWeights[i] = randCharInRange(0, 16);
	//for (int i = 0; i < 56; i++) g.hiddenWeights[i] = randCharInRange(0, 16);
	for (int i = 0; i < 80; i++) g.inputWeights[i] = 8;
	for (int i = 0; i < 56; i++) g.hiddenWeights[i] = 8;
	return g;
}

Genome Ant::mutateGenome(Genome g) {
	//MUTATION METHOD SETUP
	//there are two methods here to choose from

	//change MUTATION_RATE random chars in the genome randomly
	int index;
	for (int i = 0; i < MUTATION_RATE; i++) {
		index = randIntInRange(0, 135);
		if (index < 80) g.inputWeights[index]		= randCharInRange(0, 16);
		else			g.hiddenWeights[index - 80] = randCharInRange(0, 16);
	}

	//increment or decrement MUTATION_RATE random chars
	/*int index;
	for (int i = 0; i < MUTATION_RATE; i++) {
		index = randIntInRange(0, 135);
		if (index < 80) {
			if (randFloat() > 0.5f) g.inputWeights[index] = min(16, g.inputWeights[index] + 1);
			else					g.inputWeights[index] = max(0, g.inputWeights[index] - 1);
		}
		else {
			if (randFloat() > 0.5f)	g.hiddenWeights[index - 80] = min(16, g.inputWeights[index - 80] + 1);
			else					g.hiddenWeights[index - 80] = max(0, g.inputWeights[index - 80] - 1);
		}
	}*/

	return g;
}

void Ant::printNet() {
	for (int i = 0; i < 10; i++) {
		cout << input[i] << ",";
	}
	cout << endl;
	for (int i = 0; i < 8; i++) {
		cout << hidden[i] << ",";
	}
	cout << endl;
	for (int i = 0; i < 7; i++) {
		cout << output[i] << ",";
	}
	cout << endl;
	cout << "current direction = " << (int)dir << endl;
	return;
}
