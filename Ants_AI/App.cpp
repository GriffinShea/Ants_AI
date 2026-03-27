#include "App.h"

vector<Vertex> square;
int worldSize;
World* world;

void CreateSquare(void) {
	
	return;
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
					color = glm::vec3(0, 1, 0);
				else
					color = glm::vec3(1, 1, 1);
			}
			else if (world->checkFood(row, col)) {
				color = glm::vec3(0.0, 0.75, 0.0);
			}
			else if (world->checkHive(row, col)) {
				color = glm::vec3(0.5, 0.0, 0.5);
			}
			else {
				color = glm::vec3(0.5);
				color.x = (world->readSignalA(row, col) + 1) / 2;
				color.z = (world->readSignalB(row, col) + 1) / 2;
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


void ResizeCallback(int w, int h) {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, w, 0.0, h);

}

void App::stop() {
}

void App::setWorld(World *w, int s) {
	world = w;
	worldSize = s;
	gridSize = windowSize / worldSize;
	Vertex v;
	v.pos = glm::vec2(0, gridSize);
	square.push_back(v);
	v.pos = glm::vec2(gridSize, gridSize);
	square.push_back(v);
	v.pos = glm::vec2(gridSize, 0);
	square.push_back(v);
	v.pos = glm::vec2(0, 0);
	square.push_back(v);
	return;
}

int App::Init(int argc, char* argv[], int ws) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	windowSize = ws * GRID_SIZE;
	glutInitWindowSize(windowSize, windowSize);
	glutInitWindowPosition(0, 0);
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
	//CreateSquare();

	//glutKeyboardFunc(KeyboardInputCallback);
	//glutMainLoop();

	RenderCallback();
	ResizeCallback(windowSize, windowSize);
	return 0;
}
