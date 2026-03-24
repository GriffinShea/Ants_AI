#include "App.h"

vector<Vertex> square;
int worldSize;
World* world;
void CreateSquare(void) {
	Vertex v;
	v.pos = glm::vec2(0, GRID_SIZE);
	square.push_back(v);
	v.pos = glm::vec2(GRID_SIZE, GRID_SIZE);
	square.push_back(v);
	v.pos = glm::vec2(GRID_SIZE, 0);
	square.push_back(v);
	v.pos = glm::vec2(0, 0);
	square.push_back(v);
}

void drawGrid() {
	glm::vec2 position;
	glm::vec2 v;
	glm::vec3 color;
	int window_size = worldSize * GRID_SIZE;
	for (int row = 0; row < worldSize; ++row) {
		for (int col = 0; col < worldSize; ++col) {
			position = glm::vec2(window_size - GRID_SIZE - GRID_SIZE * row, 0 + col * GRID_SIZE);

			if (world->containsAnt(row, col)) {
				if (world->getAnt(row, col)->getHasFood())
					color = glm::vec3(0.9, 0.9, 0.9);
				else
					color = glm::vec3(0.1, 0.1, 0.1);
			}
			else if (world->checkHive(row, col) && world->checkFood(row, col)) {
				color = glm::vec3(0.5, 0.25, 0.5);
			}
			else if (world->checkFood(row, col)) {
				color = glm::vec3(0.0, 1.0, 0.0);
			}
			else if (world->checkHive(row, col)) {
				color = glm::vec3(1.0, 0.0, 0.5);
			}
			else {
				if (world->showSignalA(row, col) && world->checkSignalA(row, col)) {
					color = glm::vec3(0, 0, 0.5);
				}
				else if (!world->showSignalA(row, col) && world->checkSignalB(row, col)) {
					color = glm::vec3(0.5, 0, 0);
				}
				else { color = glm::vec3(0.5, 0.5, 0.5); }
			}

			glBegin(GL_POLYGON);
			for (int i = 0; i < square.size(); i++) {
				v = square[i].pos + position;
				glColor3fv((GLfloat*)&color);
				glVertex2fv((GLfloat*)&v);
			}
			glEnd();
		}
	}
}


void RenderCallback() {
	glClearColor(0, 0, 0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	drawGrid();
	glFlush();
}


void ResizeCallback(int width, int height) {

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, instance->window_width, 0.0, instance->window_height);

}

void App::stop() {
}

int App::Init(int argc, char* argv[], World* w, int size) {
	world = w;
	worldSize = size;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	window_height = worldSize * GRID_SIZE;
	window_width = worldSize * GRID_SIZE;
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(120, 120);
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "Could not initialize the GLEW library: " << endl;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glutCreateWindow("ANT A.I. PROJECT");
	instance = this;
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ResizeCallback);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	CreateSquare();

	//glutKeyboardFunc(KeyboardInputCallback);
	//glutMainLoop();

	RenderCallback();
	ResizeCallback(worldSize * GRID_SIZE, worldSize * GRID_SIZE);
	return 0;
}
