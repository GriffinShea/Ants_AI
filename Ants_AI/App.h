#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "World.h";


const int GRID_SIZE = 7;

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

class App {

public:
	int Init(int argc, char* argv[], int);
	void setWorld(World*, int);
	void stop();
	int windowSize;
	float gridSize;
};

static App* instance;
//callback functions
void RenderCallback();
void ResizeCallback(int, int);
