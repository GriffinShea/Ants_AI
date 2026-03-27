#pragma once

//GENOME CONSTANTS
const int G_SN = 15;			//SENSORY NEURONS
const int G_HN = 20;			//HIDDEN NEURONS
const int G_AN = 7;				//ACTUATOR NEURONS

const int G_IW = G_SN * G_HN;	//INPUT WEIGHTS
const int G_HW = G_HN * G_AN;	//OUTPUT WEIGHTS

struct Genome {
	char inputWeights[G_IW];
	char hiddenWeights[G_HW];
};
