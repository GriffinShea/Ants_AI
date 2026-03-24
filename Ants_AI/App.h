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


const int GRID_SIZE = 6;

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
	glm::vec3 normal;
};

class App {

public:
	int Init(int argc, char* argv[], World* w, int size);
	void stop();
	int window_width, window_height;
};

static App* instance;
//callback functions
void RenderCallback();
void ResizeCallback(int width, int height);
