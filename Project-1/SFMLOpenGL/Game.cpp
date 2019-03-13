#include <Game.h>
#include <Cube.h>
#include <Easing.h>

// Helper to convert Number to String for HUD
template <typename T>
string toString(T number)
{
	ostringstream oss;
	oss << number;
	return oss.str();
}

GLuint	vsid,		// Vertex Shader ID
		fsid,		// Fragment Shader ID
		progID,		// Program ID
		vao = 0,	// Vertex Array ID
		vbo,		// Vertex Buffer ID
		vib,		// Vertex Index Buffer
		to[1];		// Texture ID
GLint	positionID,	// Position ID
		colorID,	// Color ID
		textureID,	// Texture ID
		uvID,		// UV ID
		mvpID,		// Model View Projection ID
		x_offsetID, // X offset ID
		y_offsetID,	// Y offset ID
		z_offsetID;	// Z offset ID

GLenum	error;		// OpenGL Error Code


//Please see .//Assets//Textures// for more textures
const string filename = ".//Assets//Textures//grid_wip.tga";

int width;						// Width of texture
int height;						// Height of texture
int comp_count;					// Component of texture

unsigned char* img_data;		// image data

mat4 mvp[MAX_CUBES], projection, 
		view, model[MAX_CUBES];			// Model View Projection

Font font;						// Game font

float x_offset, y_offset, z_offset; // offset on screen (Vertex Shader)

Game::Game() : 
	window(VideoMode(800, 600), 
	"Introduction to OpenGL Texturing")
{
}

Game::Game(sf::ContextSettings settings) : 
	window(VideoMode(800, 600), 
	"Introduction to OpenGL Texturing", 
	sf::Style::Default, 
	settings)
{
	game_object[0] = new GameObject();
	game_object[0]->setPosition(vec3(0.5f, 0.5f, 0.0f));

	game_object[1] = new GameObject();
	game_object[1]->setPosition(vec3(0.5f, 0.5f, -14.0f));

	game_object[2] = new GameObject();
	game_object[2]->setPosition(vec3(0.5f, 0.5f, -24.0f));

	game_object[3] = new GameObject();
	game_object[3]->setPosition(vec3(0.5f, 0.5f, -34.0f));

	game_object[4] = new GameObject();
	game_object[4]->setPosition(vec3(50.0f, 50.0f, 50.0f));
}

Game::~Game()
{
}

void Game::changeCamera()
{
	if (cameraView)
	{
		cameraView = false;
	}
	else if (!cameraView)
	{
		cameraView = true;
	}
}

void Game::moveObstacles()
{
	for (int i = 1; i < MAX_CUBES -1; i++)
	{
		if (game_object[i]->getPosition().z > 14.0)
		{
			if (i == 3)
			{
				points++;
			}
			game_object[i]->setPosition(vec3{ 0.5f, 0.5f, -30.0f });
		}
		else
		{
			game_object[i]->setPosition(game_object[i]->getPosition() + vec3{ 0.0,0.0,0.02 });
		}
		
	}
}


void Game::run()
{

	initialize();

	Event event;

	float rotation = 0.01f;
	float start_value = 0.0f;
	float end_value = 1.0f;

	while (isRunning){

#if (DEBUG >= 2)
		DEBUG_MSG("Game running...");
#endif

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				isRunning = false;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				// Set Model Rotation
				if (!animate)
				{
					animate = true;
					if (rotation < 0)
						rotation *= -1; // Set Positive
					animation = glm::vec3(0, 1, 0); //Rotate Y
				}
			}

			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				// Set Model Rotation
				if (!animate)
				{
					animate = true;
					if (rotation >= 0)
						rotation *= -1; // Set Negative
					animation = glm::vec3(0, 1, 0); //Rotate Y
				}

				// https://www.sfml-dev.org/documentation/2.0/classsf_1_1Clock.php
				// https://github.com/acron0/Easings
				// http://robotacid.com/documents/code/Easing.cs
				// http://st33d.tumblr.com/post/94243475686/easing-equations-for-unity-c
				// http://easings.net/
				// http://upshots.org/actionscript/jsas-understanding-easing
				// https://www.kirupa.com/html5/animating_with_easing_functions_in_javascript.htm
				// https://medium.com/motion-in-interaction/animation-principles-in-ui-design-understanding-easing-bea05243fe3#.svh4gczav
				// http://thednp.github.io/kute.js/easing.html
				// http://gizma.com/easing/#quad1
				// https://github.com/warrenm/AHEasing

				// VR
				// https://www.sfml-dev.org/documentation/2.4.2/classsf_1_1Sensor.php
				// http://en.sfml-dev.org/forums/index.php?topic=9412.msg65594
				// https://github.com/SFML/SFML/wiki/Tutorial:-Building-SFML-for-Android-on-Windows
				// https://github.com/SFML/SFML/wiki/Tutorial:-Building-SFML-for-Android
				// https://www.youtube.com/watch?v=n_JSi6ihDFs
				// http://en.sfml-dev.org/forums/index.php?topic=8010.0
				// 
			}
			
			if (animate)
			{
				rotation += (1.0f * rotation) + 0.05f;
				model[0] = rotate(model[0], 0.01f, animation); // Rotate
				rotation = 0.0f;
				animate = false;
			}
		}

		if (!gameOver)
		{
			update();
		}
		else
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
					resetGame();
			}
		}
		
		render();
	}

#if (DEBUG >= 2)
	DEBUG_MSG("Calling Cleanup...");
#endif
	unload();

}

void Game::collision()
{
	if (!invincible)
	{
		for (int i = 1; i < MAX_CUBES - 1; i++)
		{
			if (game_object[0]->getPosition().z - 2 < game_object[i]->getPosition().z &&
				game_object[0]->getPosition().z > game_object[i]->getPosition().z - 2 &&
				game_object[0]->getPosition().y - 2 < game_object[i]->getPosition().y &&
				game_object[0]->getPosition().y > game_object[i]->getPosition().y - 2
				)
			{
				health--;
				invincible = true;
			}
		}
	}
	else
	{
		invincibilityTimer += 0.001f;
		if (invincibilityTimer >= 1)
		{
			invincible = false;
			invincibilityTimer = 0.0f;
		}
	}
}

void Game::movePlayer()
{
	if (jumping)
	{
		if (game_object[0]->getPosition().y < 7)
		{
			game_object[0]->setPosition(game_object[0]->getPosition() + vec3{ 0.0f,0.03f,0.0f });
		}
		else
		{
			jumping = false;
			falling = true;
		}
	}
	
	if (falling)
	{
		if (game_object[0]->getPosition().y > 0.5)
		{
			game_object[0]->setPosition(game_object[0]->getPosition() - vec3{ 0.0f,0.03f,0.0f });
		}
		else
		{
			falling = false;
		}
	}
}

void Game::resetGame()
{
	game_object[0] = new GameObject();
	game_object[0]->setPosition(vec3(0.5f, 0.5f, 0.0f));

	game_object[1] = new GameObject();
	game_object[1]->setPosition(vec3(0.5f, 0.5f, -14.0f));

	game_object[2] = new GameObject();
	game_object[2]->setPosition(vec3(0.5f, 0.5f, -24.0f));

	game_object[3] = new GameObject();
	game_object[3]->setPosition(vec3(0.5f, 0.5f, -34.0f));

	game_object[4] = new GameObject();
	game_object[4]->setPosition(vec3(50.0f, 50.0f, 50.0f));

	health = 5;
	points = 0;
	jumping = false;
	falling = false;
	gameOver = false;
}

void Game::initialize()
{
	text.setFont(font);
	healthText.setFont(font);
	isRunning = true;
	GLint isCompiled = 0;
	GLint isLinked = 0;

	if (!(!glewInit())) { DEBUG_MSG("glewInit() failed"); }

	// Copy UV's to all faces
	for (int i = 1; i < 6; i++)
		memcpy(&uvs[i * 4 * 2], &uvs[0], 2 * 4 * sizeof(GLfloat));

	DEBUG_MSG(glGetString(GL_VENDOR));
	DEBUG_MSG(glGetString(GL_RENDERER));
	DEBUG_MSG(glGetString(GL_VERSION));
	
	for (int i = 0; i < MAX_CUBES; i++)
	{


		// Vertex Array Buffer
		glGenBuffers(1, &vbo);		// Generate Vertex Buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Vertices (3) x,y,z , Colors (4) RGBA, UV/ST (2)

		int countVERTICES = game_object[i]->getVertexCount();
		int countCOLORS = game_object[i]->getColorCount();
		int countUVS = game_object[i]->getUVCount();

		glBufferData(GL_ARRAY_BUFFER, ((3 * countVERTICES) + (4 * countCOLORS) + (2 * countUVS)) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

		glGenBuffers(1, &vib); //Generate Vertex Index Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);


		int countINDICES = game_object[i]->getIndexCount();
		// Indices to be drawn
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * countINDICES * sizeof(GLuint), indices, GL_STATIC_DRAW);

		// NOTE: uniforms values must be used within Shader so that they 
		// can be retreived
		const char* vs_src =
			"#version 400\n\r"
			""
			"in vec3 sv_position;"
			"in vec4 sv_color;"
			"in vec2 sv_uv;"
			""
			"out vec4 color;"
			"out vec2 uv;"
			""
			"uniform mat4 sv_mvp;"
			"uniform float sv_x_offset;"
			"uniform float sv_y_offset;"
			"uniform float sv_z_offset;"
			""
			"void main() {"
			"	color = sv_color;"
			"	uv = sv_uv;"
			//"	gl_Position = vec4(sv_position, 1);"
			"	gl_Position = sv_mvp * vec4(sv_position.x + sv_x_offset, sv_position.y + sv_y_offset, sv_position.z + sv_z_offset, 1 );"
			"}"; //Vertex Shader Src

		DEBUG_MSG("Setting Up Vertex Shader");

		vsid = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vsid, 1, (const GLchar**)&vs_src, NULL);
		glCompileShader(vsid);

		// Check is Shader Compiled
		glGetShaderiv(vsid, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_TRUE) {
			DEBUG_MSG("Vertex Shader Compiled");
			isCompiled = GL_FALSE;
		}
		else
		{
			DEBUG_MSG("ERROR: Vertex Shader Compilation Error");
		}

		const char* fs_src =
			"#version 400\n\r"
			""
			"uniform sampler2D f_texture;"
			""
			"in vec4 color;"
			"in vec2 uv;"
			""
			"out vec4 fColor;"
			""
			"void main() {"
			"	fColor = color - texture2D(f_texture, uv);"
			""
			"}"; //Fragment Shader Src

		DEBUG_MSG("Setting Up Fragment Shader");

		fsid = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fsid, 1, (const GLchar**)&fs_src, NULL);
		glCompileShader(fsid);

		// Check is Shader Compiled
		glGetShaderiv(fsid, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_TRUE) {
			DEBUG_MSG("Fragment Shader Compiled");
			isCompiled = GL_FALSE;
		}
		else
		{
			DEBUG_MSG("ERROR: Fragment Shader Compilation Error");
		}

		DEBUG_MSG("Setting Up and Linking Shader");
		progID = glCreateProgram();
		glAttachShader(progID, vsid);
		glAttachShader(progID, fsid);
		glLinkProgram(progID);

		// Check is Shader Linked
		glGetProgramiv(progID, GL_LINK_STATUS, &isLinked);

		if (isLinked == 1) {
			DEBUG_MSG("Shader Linked");
		}
		else
		{
			DEBUG_MSG("ERROR: Shader Link Error");
		}

		// Set image data
		// https://github.com/nothings/stb/blob/master/stb_image.h
		img_data = stbi_load(filename.c_str(), &width, &height, &comp_count, 4);

		if (img_data == NULL)
		{
			DEBUG_MSG("ERROR: Texture not loaded");
		}

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &to[0]);
		glBindTexture(GL_TEXTURE_2D, to[0]);

		// Wrap around
		// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// Filtering
		// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Bind to OpenGL
		// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
		glTexImage2D(
			GL_TEXTURE_2D,			// 2D Texture Image
			0,						// Mipmapping Level 
			GL_RGBA,				// GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB, GL_BGR, GL_RGBA 
			width,					// Width
			height,					// Height
			0,						// Border
			GL_RGBA,				// Bitmap
			GL_UNSIGNED_BYTE,		// Specifies Data type of image data
			img_data				// Image Data
		);

		// Projection Matrix 
		projection = perspective(
			45.0f,					// Field of View 45 degrees
			4.0f / 3.0f,			// Aspect ratio
			5.0f,					// Display Range Min : 0.1f unit
			100.0f					// Display Range Max : 100.0f unit
		);

		// Camera Matrix
		view = lookAt(
			vec3(0.0f, 5.0f, 10.0f),	// Camera (x,y,z), in World Space
			vec3(0.0f, 0.0f, 0.0f),		// Camera looking at origin
			vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
		);

		// Model matrix
		model[i] = mat4(
			1.0f					// Identity Matrix
		);
	}
	// Enable Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// Load Font
	font.loadFromFile(".//Assets//Fonts//BBrick.ttf");
}

void Game::update()
{
#if (DEBUG >= 2)
	DEBUG_MSG("Updating...");
#endif


	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		if (!jumping && !falling)
		{
			jumping = true;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		changeCamera();
	}
	if (!cameraView)
	{
		view = lookAt(
			vec3(10.0f, 5.0f, 10.0f),	// Camera (x,y,z), in World Space
			vec3(0.0f, 0.0f, 0.0f),		// Camera looking at origin
			vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
		);
	}
	else if (cameraView)
	{
		
		view = lookAt(
			vec3(20.0f, 2.0f, 0.0f),	// Camera (x,y,z), in World Space
			vec3(0.0f, 2.0f, 0.0f),		// Camera looking at origin
			vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
		);
	}

	moveObstacles();
	movePlayer();
	// Update Model View Projection
	// For mutiple objects (cubes) create multiple models
	// To alter Camera modify view & projection
	for (int i = 0; i < MAX_CUBES; i++)
	{
		mvp[i] = projection * view * model[i];
	}
	collision();
	if (health <= 0)
	{
		gameOver = true;
	}
}

void Game::render()
{

#if (DEBUG >= 2)
	DEBUG_MSG("Render Loop...");
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save current OpenGL render states
	// https://www.sfml-dev.org/documentation/2.0/classsf_1_1RenderTarget.php#a8d1998464ccc54e789aaf990242b47f7
	window.pushGLStates();

	// Find mouse position using sf::Mouse
	int x = Mouse::getPosition(window).x;
	int y = Mouse::getPosition(window).y;

	text.setString("P O I N T S : [  " + std::to_string(points) + "  ]");
	healthText.setString("Hits remaining: [" + std::to_string(health) + "]");

	text.setFillColor(sf::Color::Magenta);
	text.setPosition(10.f, 40.f);
	if (invincible)
	{
		healthText.setFillColor(sf::Color::Red);
		healthText.setPosition(10.f, 70.0f);
	}
	else
	{
		healthText.setFillColor(sf::Color::Magenta);
		healthText.setPosition(10.f, 70.0f);
	}


	window.draw(text);
	window.draw(healthText);
	// Restore OpenGL render states
	// https://www.sfml-dev.org/documentation/2.0/classsf_1_1RenderTarget.php#a8d1998464ccc54e789aaf990242b47f7

	window.popGLStates();

	for (int i = 0; i < MAX_CUBES; i++)
	{

		// Rebind Buffers and then set SubData
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);

		// Use Progam on GPU
		glUseProgram(progID);

		// Find variables within the shader
		// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetAttribLocation.xml
		positionID = glGetAttribLocation(progID, "sv_position");
		if (positionID < 0) { DEBUG_MSG("positionID not found"); }

		colorID = glGetAttribLocation(progID, "sv_color");
		if (colorID < 0) { DEBUG_MSG("colorID not found"); }

		uvID = glGetAttribLocation(progID, "sv_uv");
		if (uvID < 0) { DEBUG_MSG("uvID not found"); }

		textureID = glGetUniformLocation(progID, "f_texture");
		if (textureID < 0) { DEBUG_MSG("textureID not found"); }

		mvpID = glGetUniformLocation(progID, "sv_mvp");
		if (mvpID < 0) { DEBUG_MSG("mvpID not found"); }

		x_offsetID = glGetUniformLocation(progID, "sv_x_offset");
		if (x_offsetID < 0) { DEBUG_MSG("x_offsetID not found"); }

		y_offsetID = glGetUniformLocation(progID, "sv_y_offset");
		if (y_offsetID < 0) { DEBUG_MSG("y_offsetID not found"); }

		z_offsetID = glGetUniformLocation(progID, "sv_z_offset");
		if (z_offsetID < 0) { DEBUG_MSG("z_offsetID not found"); };

		// VBO Data....vertices, colors and UV's appended
		// Add the Vertices for all your GameOjects, Colors and UVS

		glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), game_object[i]->getVertex());
		//glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 3 * VERTICES * sizeof(GLfloat), 4 * COLORS * sizeof(GLfloat), colors);
		glBufferSubData(GL_ARRAY_BUFFER, ((3 * VERTICES) + (4 * COLORS)) * sizeof(GLfloat), 2 * UVS * sizeof(GLfloat), uvs);

		// Send transformation to shader mvp uniform [0][0] is start of array
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[i][0][0]);

		// Set Active Texture .... 32 GL_TEXTURE0 .... GL_TEXTURE31
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(textureID, 0); // 0 .... 31

		// Set the X, Y and Z offset (this allows for multiple cubes via different shaders)
		// Experiment with these values to change screen positions

		glUniform1f(x_offsetID, game_object[i]->getPosition().x);
		glUniform1f(y_offsetID, game_object[i]->getPosition().y);
		glUniform1f(z_offsetID, game_object[i]->getPosition().z);

		/*glUniform1f(x_offsetID, 0.00f);
		glUniform1f(y_offsetID, 0.00f);
		glUniform1f(z_offsetID, 0.00f);*/

		// Set pointers for each parameter (with appropriate starting positions)
		// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glVertexAttribPointer.xml
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, (VOID*)(3 * VERTICES * sizeof(GLfloat)));
		glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, 0, (VOID*)(((3 * VERTICES) + (4 * COLORS)) * sizeof(GLfloat)));

		// Enable Arrays
		glEnableVertexAttribArray(positionID);
		glEnableVertexAttribArray(colorID);
		glEnableVertexAttribArray(uvID);

		// Draw Element Arrays
		glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
	}
	window.display();


	// Disable Arrays
	glDisableVertexAttribArray(positionID);
	glDisableVertexAttribArray(colorID);
	glDisableVertexAttribArray(uvID);

	// Unbind Buffers with 0 (Resets OpenGL States...important step)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Reset the Shader Program to Use
	glUseProgram(0);

	// Check for OpenGL Error code
	error = glGetError();
	if (error != GL_NO_ERROR) {
		DEBUG_MSG(error);
	}
}

void Game::unload()
{
#if (DEBUG >= 2)
	DEBUG_MSG("Cleaning up...");
#endif
	glDetachShader(progID, vsid);	// Shader could be used with more than one progID
	glDetachShader(progID, fsid);	// ..
	glDeleteShader(vsid);			// Delete Vertex Shader
	glDeleteShader(fsid);			// Delete Fragment Shader
	glDeleteProgram(progID);		// Delete Shader
	glDeleteBuffers(1, &vbo);		// Delete Vertex Buffer
	glDeleteBuffers(1, &vib);		// Delete Vertex Index Buffer
	stbi_image_free(img_data);		// Free image stbi_image_free(..)
}

