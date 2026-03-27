/*

	Authors of Initial Commit:
		Alexandra Coscuig
		Griffin Shea

	Author of Subsequent Commits:
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

//train instead
extern const std::string loadfilename = "";

//load file for 4 mutations per generation, mutation method 2, weights -2 to 2, all weights start at 0
//extern const std::string loadfilename = "../saved_generations/1774312453_genomes_at_generation_9.txt";
//extern const std::string loadfilename = "../saved_generations/1774312467_genomes_at_generation_59.txt";
//extern const std::string loadfilename = "../saved_generations/1774392602_genomes_at_generation_0.txt";
//extern const std::string loadfilename = "../saved_generations/1774392602_genomes_at_generation_1.txt";
//extern const std::string loadfilename = "../saved_generations/1774392602_genomes_at_generation_5.txt";
//extern const std::string loadfilename = "../saved_generations/1774392602_genomes_at_generation_10.txt";
// 
//extern const std::string loadfilename = "../saved_generations/1774392846_genomes_at_generation_9.txt";
//extern const std::string loadfilename = "../saved_generations/1774392846_genomes_at_generation_39.txt";
//extern const std::string loadfilename = "../saved_generations/1774392846_genomes_at_generation_69.txt";
//extern const std::string loadfilename = "../saved_generations/1774401047_genomes_at_generation_999.txt";
//
//extern const std::string loadfilename = "../saved_generations/1774402423_genomes_at_generation_99.txt";
//extern const std::string loadfilename = "../saved_generations/1774402423_genomes_at_generation_299.txt";
//extern const std::string loadfilename = "../saved_generations/1774417943_genomes_at_generation_99.txt";
// 
//extern const std::string loadfilename = "../saved_generations/1774417943_genomes_at_generation_99.txt";
//extern const std::string loadfilename = "../saved_generations/1774417943_genomes_at_generation_99.txt";
//extern const std::string loadfilename = "../saved_generations/1774416535_genomes_at_generation_90.txt";





//simulation run parametres
extern const int MAX_GENERATIONS = 10000;
extern const int SAVE_GENERATION = 10;

extern const int WINDOW_SIZE = 800;

extern const int WORLD_SIZE = 128;
extern const int SIMULATION_ITERATIONS = WORLD_SIZE * 8;
extern const int MUTATION_RATE = 1;

extern const int POPULATION = 32;
extern const int HIVE_SPACING = 2;
extern const int HIVE_SIZE = (POPULATION + HIVE_SPACING) / 4;
extern const int FOOD_COUNT = POPULATION * 4;



#include "World.h"
#include "Ant.h"
#include "App.h"
#include "Genome.h"

struct simulationSettings {
	int worldSize;
	int population;
	int foodCount;
	int simIterations;
};

void populateNewGeneration(Ant* ants[]) {
	//sort ants by fitness score
	std::random_shuffle(ants, ants + POPULATION);
	std::sort(ants, ants + POPULATION, Ant::antCompare);

	//select the best half of the population's genomes for reproduction
	Genome bestGenomes[POPULATION / 2];
	for (int j = 0; j < POPULATION / 2; j++) {
		bestGenomes[j] = ants[j]->getGenome();
	}

	//create new generation with best genomes
	//(mutation takes place in the Ant constructor)
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

std::string writeGenerationToFile(Ant* ants[], int genNum, int fitnessScores[], unsigned int seed) {
	std::ofstream file;
	std::stringstream filename;
	int cumulativeFitness;

	//open a new file
	filename << "../saved_generations/" << seed;
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
	return filename.str();
}

simulationSettings readGenerationFromFile(Ant* ants[], std::string filename) {
	ifstream file(filename);
	string line;
	simulationSettings ss;
	ss.worldSize = 0;

	if (file.is_open()) {
		getline(file, line);

		int p = line.find(",");
		ss.worldSize = std::stoi(line.substr(0, p));
		line.erase(0, p + 1);
		p = line.find(",");
		ss.population = std::stoi(line.substr(0, p));
		line.erase(0, p + 1);
		p = line.find(",");
		ss.foodCount= std::stoi(line.substr(0, p));
		line.erase(0, p + 1);
		p = line.find(",");
		ss.simIterations = std::stoi(line.substr(0, p));
		line.erase(0, p + 1);
		

		getline(file, line);
		for (int i = 0; i < POPULATION; i++) {
			if (getline(file, line)) {
				Genome genome;
				for (int j = 0; j < G_IW; j++) genome.inputWeights[j] = line[j] - 'a';
				for (int j = 0; j < G_HW; j++) genome.hiddenWeights[j] = line[j + G_IW] - 'a';
				ants[i] = new Ant(genome);
			}

		}
	}

	return ss;
}

int displayGeneration(int argc, char* argv[], std::string filename, App app) {
	std::cout << "Loading genomes from " << filename << ":" << std::endl;
	Ant* ants[POPULATION];
	simulationSettings ss = readGenerationFromFile(ants, filename);
	if (ss.worldSize == 0) {
		std::cout << "ERROR: The file " << filename << " could not be opened or read, exiting." << std::endl;
		return 0;
	}
	World *world = new World(ss.worldSize, ss.population, ants, ss.foodCount);

	app.setWorld(world, ss.worldSize);
	for (int i = 0; i < ss.simIterations; i++) {
		world->simulateStep(i, ss.simIterations);
		RenderCallback();
		Sleep(1000 / 60);
	}
	std::cout << getCumulativeFitness(ants);
	delete world;
	return 0;
}

int main(int argc, char* argv[]) {
	//save seed from system time to seed the random number generator
	unsigned int seed = time(NULL);
	srand(seed);

	App app;
	app.Init(argc, argv, WORLD_SIZE);
	int fitnessScores[SAVE_GENERATION];

	//if loadfile, display the generation and close
	if (loadfilename != "") displayGeneration(argc, argv, loadfilename, app);
	//if no loadfile, begin the evolutionary algorithm by populating with random ants
	else {
		std::cout << "No sourcefile: generating " << POPULATION << " random ants..." << std::endl;

		

		std::cout << "Begining simulation (each \".\" represents a generation, the number is the generation's cumulative fitness score):" << std::endl;
		
		//create a set of ants with random or all 0 genomes
		Ant* ants[POPULATION];
		for (int i = 0; i < POPULATION; i++)
			ants[i] = new Ant();
		
		//run the evolutionary algorithm
		for (int i = 0; i <= MAX_GENERATIONS; i++) {
			//create the world, simulate for SIMULATION_ITERATIONS steps
			//(each ant keeps track of its own fitness score)
			World *world = new World(WORLD_SIZE, POPULATION, ants, FOOD_COUNT);
			for (int j = 0; j < SIMULATION_ITERATIONS; j++)
				world->simulateStep(j, SIMULATION_ITERATIONS);
			delete world;

			int fitness = getCumulativeFitness(ants);
			std::cout << fitness << ".";
			fitnessScores[i % SAVE_GENERATION] = fitness;

			//save best genomes to file every so often
			if (i % SAVE_GENERATION == 9) {
				std::string filename = writeGenerationToFile(ants, i, fitnessScores, seed);
				displayGeneration(argc, argv, filename, app);
			}

			//select best ants by fitness score and create a new generation
			populateNewGeneration(ants);
		}
	}


	return 0;
}
