#ifndef GAME_H
#define GAME_H

#include <string>
#include <sstream>

#include <iostream>
#include <GL/glew.h>
#include <GL/wglew.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Debug.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GameObject.h>

using namespace std;
using namespace sf;
using namespace glm;

const int MAX_CUBES = 5;

class Game
{
public:
	Game();
	Game(sf::ContextSettings settings);
	~Game();
	void changeCamera();
	void moveObstacles();
	void run();
	void collision();
 private:
	GameObject* game_object[MAX_CUBES];
	RenderWindow window;
	Clock clock;
	Time time;
	bool cameraView = false;
	bool animate = false;
	vec3 animation = vec3(0.0f);
	float rotation = 0.0f;
	bool isRunning = false;
	void initialize();
	void update();
	void render();
	void unload();

	int points = 0;
};

#endif  // ! GAME_H