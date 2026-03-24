/*

	Authors:
		Alexandra Coscuig - App.cpp
		Griffin Shea

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <algorithm>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

//constants not to be touched
//object IDs for the grid
extern const int EID_NONE = 0;
extern const int EID_ANT = 1;
extern const int EID_FOOD = 2;
extern const int EID_HIVE = 3;

//train instead
//extern const std::string loadfilename = "";

//load file for 2 mutations per generation, mutation method 2, weights -2 to 2, all weights start random
//extern const std::string loadfilename = "../saved_generations/1774312453_genomes_at_generation_9.txt";
//extern const std::string loadfilename = "../saved_generations/1774312467_genomes_at_generation_59.txt";
extern const std::string loadfilename = "../saved_generations/1774312475_genomes_at_generation_89.txt";

//simulation run parametres
extern const int MAX_GENERATIONS = 10000;
extern const int SAVE_GENERATION = 10;

//height and width of world
extern const int WORLD_SIZE = 100;
//ant population is enough to line one wall of the world
extern const int POPULATION = WORLD_SIZE;
//four foods per ant
extern const int FOOD_COUNT = POPULATION * 4;
//enough time for an ant to walk the perimeter of the world
extern const int SIMULATION_ITERATIONS = WORLD_SIZE * 8;
//number of mutations to the genome
extern const int MUTATION_RATE = 4;	//MUTATION RATE SETUP

#include "World.h"
#include "Ant.h"
#include "App.h"

void populateNewGeneration(Ant* ants[]) {
	//sort ants by fitness score
	std::sort(ants, ants + POPULATION, Ant::antCompare);

	//select the best half of the population's genomes for reproduction
	Genome bestGenomes[POPULATION / 2];
	for (int j = 0; j < POPULATION / 2; j++) {
		bestGenomes[j] = ants[j]->getGenome();
	}

	//deallocate generation, create new generation with best genomes
	for (int j = 0; j < POPULATION / 2; j++) {
		delete ants[j];
		ants[j] = new Ant(bestGenomes[j]);

		delete ants[j + POPULATION / 2];
		ants[j + POPULATION / 2] = new Ant(bestGenomes[j]);
	}

	//shuffle ants
	std::random_shuffle(ants, ants + POPULATION);
	return;
}

int getCumulativeFitness(Ant* ants[]) {
	int sum = 0;
	for (int i = 0; i < POPULATION; i++) {
		sum += ants[i]->getFitness();
	}
	return sum;
}

void writeGenerationToFile(Ant* ants[], int genNum, int fitnessScores[]) {
	std::ofstream file;
	std::stringstream filename;
	int cumulativeFitness;

	//open a new file
	filename << "../saved_generations/" << std::to_string(time(0));
	filename << "_genomes_at_generation_" << std::to_string(genNum);
	filename << ".txt";
	file.open(filename.str());

	//write world size, hive size, population, food count
	file << WORLD_SIZE << "," << POPULATION << ",";
	file << FOOD_COUNT << "," << SIMULATION_ITERATIONS;
	file << "," << MUTATION_RATE << std::endl;

	//write the fitness scores for each generation
	for (int i = 0; i < SAVE_GENERATION; i++) {
		file << fitnessScores[i] << ",";
	}
	file << std::endl;

	//write each ant's genome
	for (int i = 0; i < POPULATION; i++) {
		file << ants[i]->getGenomeString().str() << std::endl;
	}

	//close file
	file.close();

	std::cout << std::endl;
	std::cout << "Wrote genomes of generation " << genNum;
	std::cout << " to file: \n\t" << filename.str() << std::endl << std::endl;
	return;
}

int readGenerationFromFile(Ant* ants[], std::string filename) {
	ifstream file(filename);
	string line;

	if (file.is_open()) {
		getline(file, line);
		getline(file, line);
		for (int i = 0; i < POPULATION; i++) {
			if (getline(file, line)) {
				Genome genome;
				for (int j = 0; j < 80; j++) genome.inputWeights[j] = line[j] - 'a';
				for (int j = 0; j < 56; j++) genome.hiddenWeights[j] = line[j + 80] - 'a';
				ants[i] = new Ant(genome);
			}

		}
	}

	for (int i = filename.length() - 1; i >= 0; i--) {
		if (filename[i] == '_') {
			return stoi(filename.substr(i + 1, filename.length() - 4));
		}
	}

	return 0;
}

int main(int argc, char* argv[]) {
	std::cout << "HI";
	srand(time(NULL));

	Ant* ants[POPULATION];
	World* world;
	int fitnessScores[SAVE_GENERATION];
	int initialGeneration = 0;

	//if no loadfile, generate initial generation with random genome constructor
	if (loadfilename == "") {
		std::cout << "No sourcefile: generating " << POPULATION << " random ants..." << std::endl;
		for (int i = 0; i < POPULATION; i++) {
			ants[i] = new Ant();
		}

		std::cout << "Begining simulation (each \".\" represents a generation):" << std::endl;
		for (int i = initialGeneration; i <= MAX_GENERATIONS; i++) {

			//create the world, simulate for SIMULATION_ITERATIONS steps, then deallocate
			world = new World(WORLD_SIZE, POPULATION, ants, FOOD_COUNT);
			for (int j = 0; j < SIMULATION_ITERATIONS; j++) {
				world->simulateStep(j);
			}
			delete world;

			int fitness = getCumulativeFitness(ants);
			std::cout << fitness << ".";
			fitnessScores[i % SAVE_GENERATION] = fitness;

			//save best genomes to file every so often
			if (i % SAVE_GENERATION == SAVE_GENERATION - 1) {
				writeGenerationToFile(ants, i, fitnessScores);
			}

			//select best ants by fitness score and create a new generation
			populateNewGeneration(ants);
		}
	}

	else {
		std::cout << "Loading genomes from " << loadfilename << ":" << __FILE__ << std::endl;
		initialGeneration = readGenerationFromFile(ants, loadfilename);
		world = new World(WORLD_SIZE, POPULATION, ants, FOOD_COUNT);
		
		App app;
		app.Init(argc, argv, world, WORLD_SIZE);
		for (int i = 0; i < SIMULATION_ITERATIONS; i++) {
			world->simulateStep(i);
			RenderCallback();
			Sleep(1000 / 30);
		}
		std::cout << getCumulativeFitness(ants);
		delete world;
	}

	return 0;
}
