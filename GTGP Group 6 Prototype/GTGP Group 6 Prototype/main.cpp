#include "GameEngine.h"
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>


#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#define DEG_TO_RADIAN 0.017453293


// Globals
GLuint shaderProgram;

GLfloat r = 0.0f, eyeRotAngle = 0.0f, atAng = -45.0f;

glm::vec3 eye(2.0f, 70.0f, 0.0f);
glm::vec3 at(2.0f, 1.0f, -1.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

stack<glm::mat4> mvStack;

// TEXTURE STUFF
GLuint textures[3];

rt3d::lightStruct light0 = {
	{ 0.3f, 0.3f, 0.3f, 1.0f }, // ambient
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
	{ -7.0f, 10.0f, -18.0f, 1.0f }  // position
};
glm::vec4 lightPos(-7.0f, 10.0f, -18.0f, 1.0f); //light position

rt3d::materialStruct material0 = {
	{ 0.2f, 0.4f, 0.2f, 1.0f }, // ambient
	{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
	{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
	2.0f  // shininess
};
rt3d::materialStruct material1 = {
	{ 0.4f, 0.4f, 1.0f, 1.0f }, // ambient
	{ 0.8f, 0.8f, 1.0f, 1.0f }, // diffuse
	{ 0.8f, 0.8f, 0.8f, 1.0f }, // specular
	1.0f  // shininess
};
rt3d::materialStruct material2 = {
	{ 0.2f, 0.4f, 0.2f, 0.1f }, // ambient
	{ 0.5f, 1.0f, 0.5f, 0.1f }, // diffuse
	{ 0.0f, 0.1f, 0.0f, 0.1f }, // specular
	2.0f  // shininess
};
rt3d::attenuationStruct att0 = {
	1.0f,	//constant
	0.02f,	//linear
	0.01f	//quadratic
};


GameEngine GE;

// Set up rendering context
SDL_Window * setupRC(SDL_GLContext &context) {
	SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

													   // Create 800x600 window
	window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return window;
}

// A simple texture loading function
// lots of room for improvement - and better error checking!
GLuint loadBitmap(char *fname) {
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID

							  // load file - using core SDL library
	SDL_Surface *tmpSurface;
	tmpSurface = SDL_LoadBMP(fname);
	if (!tmpSurface) {
		std::cout << "Error loading bitmap" << std::endl;
	}

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SDL_PixelFormat *format = tmpSurface->format;

	GLuint externalFormat, internalFormat;
	if (format->Amask) {
		internalFormat = GL_RGBA;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
	}
	else {
		internalFormat = GL_RGB;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0,
		externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
	return texID;	// return value of texture ID
}

GLuint meshIndexCount, objectMesh;




void init(void) {

	shaderProgram = rt3d::initShaders("phong-tex.vert", "phong-tex.frag");
	rt3d::setLight(shaderProgram, light0);
	rt3d::setMaterial(shaderProgram, material0);
	rt3d::setAtt(shaderProgram, att0);
	
	vector<GLfloat> verts, norms, tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	objectMesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());

	textures[0] = loadBitmap("carpet1.bmp");
	GE.ground.init(textures[0], meshIndexCount, objectMesh);

	textures[1] = loadBitmap("wall2.bmp");
	textures[2] = loadBitmap("wall.bmp");

	GE.lWall.init(textures[1], meshIndexCount, objectMesh);
	glm::vec3 trans(0.0f), rot(0.0f), scale(0.0f);
	GE.lWall.setWall(trans, rot, scale);

	GE.rWall.init(textures[1], meshIndexCount, objectMesh);
	trans = { 10.0f, 5.0f, -10.0f };
	GE.rWall.setWall(trans, rot, scale);
	
	GE.fWall.init(textures[1], meshIndexCount, objectMesh);
	trans[0] = -10.0f; trans[2] = -30.0f; rot[1] += 1.0f;
	GE.fWall.setWall(trans, rot, scale);
	
	GE.bWall.init(textures[1], meshIndexCount, objectMesh);
	trans[2] = 10.0f; rot[1] -= 2.0f;
	GE.bWall.setWall(trans, rot, scale);

	for (int i = 0; i < 5; i++)
		GE.corridor[i].init(textures[2], meshIndexCount, objectMesh);


	trans = { -20.0f, 3.0f, -10.0f }; rot[1] = 0.0f; scale = { 0.2f, 7.0f, 15.0f };
	GE.corridor[0].setWall(trans, rot, scale);

	trans[0] += 5.0f; trans[2] = 0.0f; scale[2] = 5.0f;
	GE.corridor[1].setWall(trans, rot, scale);

	trans[2] = -20.0f;
	GE.corridor[2].setWall(trans, rot, scale);

	trans[0] += 7.0f; trans[2] += 5.0f; rot[1] = 1.0f; scale[2] = 7.0f;
	GE.corridor[3].setWall(trans, rot, scale);

	trans[2] += 10.0f;
	GE.corridor[4].setWall(trans, rot, scale);

	//initialising player-related stuff
	GE.player.init(textures);
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle*DEG_TO_RADIAN));
}

glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
}

void update(void) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	glm::vec3 bodyPos = GE.player.getPosition();
	glm::vec3 pastPos = bodyPos;

	if (keys[SDL_SCANCODE_W]) bodyPos = moveForward(bodyPos, r, 0.1f);
	if (keys[SDL_SCANCODE_S]) bodyPos = moveForward(bodyPos, r, -0.1f);
	if (keys[SDL_SCANCODE_A]) r -= 1.0f;
	if (keys[SDL_SCANCODE_D]) r += 1.0f;

	if (keys[SDL_SCANCODE_X]) eye = moveForward(eye, eyeRotAngle, 0.1f);
	if (keys[SDL_SCANCODE_C]) eye = moveForward(eye, eyeRotAngle, -0.1f);
	if (keys[SDL_SCANCODE_Z]) eye = moveRight(eye, eyeRotAngle, -0.1f);
	if (keys[SDL_SCANCODE_V]) eye = moveRight(eye, eyeRotAngle, 0.1f);
	if (keys[SDL_SCANCODE_B]) eyeRotAngle -= 1.0f;
	if (keys[SDL_SCANCODE_N]) eyeRotAngle += 1.0f;
	if (keys[SDL_SCANCODE_R]) { if (eye[1] < 50.0f) eye[1] += 0.1f; }
	if (keys[SDL_SCANCODE_F]) { if (eye[1] > 0.7f) eye[1] -= 0.1f; }
	if (keys[SDL_SCANCODE_T]) { if (atAng < 50.0f) atAng += 0.1f; }
	if (keys[SDL_SCANCODE_G]) { if (atAng > -50.0f) atAng -= 0.1f; }


	Wall walls[] = { GE.fWall, GE.bWall, GE.lWall, GE.rWall, GE.corridor[0], GE.corridor[1], GE.corridor[2], GE.corridor[3], GE.corridor[4], GE.corridor[5] };
	
	bodyPos = GE.player.wallCollision(pastPos, bodyPos, walls, r);

	GE.player.moveLegs(keys);


	if (keys[SDL_SCANCODE_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	if (keys[SDL_SCANCODE_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
	
	GE.player.setBodyPos(bodyPos, r);

}


void draw(SDL_Window * window) {

	
	// clear the screen
	glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 150.0f);
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));


	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);
	
	at = moveForward(eye, eyeRotAngle, 7.0f);
	at[1] += atAng;
	
	mvStack.top() = glm::lookAt(eye, at, up);

	glDepthMask(GL_TRUE); // make sure depth test is on
	
	glUseProgram(shaderProgram);

	glm::vec4 tmp = mvStack.top()*lightPos;
	light0.position[0] = tmp.x;
	light0.position[1] = tmp.y;
	light0.position[2] = tmp.z;
	rt3d::setLightPos(shaderProgram, glm::value_ptr(tmp));

	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));
	
	GE.ground.render(material1, shaderProgram, mvStack);
	
	//rendering walls
	GE.lWall.render(material1, shaderProgram, mvStack);
	GE.rWall.render(material1, shaderProgram, mvStack);
	GE.fWall.render(material1, shaderProgram, mvStack);
	GE.bWall.render(material1, shaderProgram, mvStack);
	//

	//rendering corridor
	for (int i = 0; i < 5; i++) {
		GE.corridor[i].render(material1, shaderProgram, mvStack);
	}
	//
	
	GE.player.render(material1, shaderProgram, mvStack, r);
	
	// remember to use at least one pop operation per push...
	mvStack.pop(); // initial matrix
	glDepthMask(GL_TRUE);

	SDL_GL_SwapWindow(window); // swap buffers
	
}


// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) {
	SDL_Window * hWindow; // window handle
	SDL_GLContext glContext; // OpenGL context handle
	hWindow = setupRC(glContext); // Create window and render context 

								  // Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << endl;
		exit(1);
	}
	cout << glGetString(GL_VERSION) << endl;

	init();

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT)
				running = false;
		}
		update();
		draw(hWindow); // call the draw function
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	return 0;
}
