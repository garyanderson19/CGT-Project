#pragma once
#include <string>
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "md2model.h"
#include <stack>
using namespace std;

class GameObjects {

protected:
	glm::vec3 position;
	string name;
	GLuint objectTexture;
	GLuint objectMesh, meshIndexCount;

public:
	GameObjects();
	~GameObjects();
	glm::vec3 getPosition();
	void setPosition(glm::vec3 newPos);
	virtual void init(GLuint texID) = 0;
	
};


//Cube and subclasses of Cube
class Cube : public GameObjects {
public:
	void init(GLuint texID);
	void init(GLuint texID, GLuint mIC, GLuint mesh);
	virtual void render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview) = 0;
};


class Ground : public Cube {
public:
	Ground();
	void render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview);
};

class Wall : public Cube {
private:
	struct WallVecs {
		glm::vec3 translation{ -30.0f, 5.0f, -10.0f };
		glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 scaling{ 0.2f, 12.0f, 20.0f };
	} WV; 
public:
	Wall();
	glm::vec3 getWallVecs(int vec);
	void setWall(glm::vec3 tran, glm::vec3 rot, glm::vec3 scale);
	void render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview);
};
//

//Player-related classes/objects
class Leg : public GameObjects {
private:
	float xRotAngle = 0.0f, zRotAngle = 0.0f;
	char *model;
public:
	Leg();
	void init(GLuint texID);
	void setModel(int m);
	void setXRotAngle(float rot);
	float getXRotAngle();
	void setZRotAngle(float rot);
	float getZRotAngle();
	void render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle);
};

class Body : public GameObjects {
public:
	Body();
	void init(GLuint texID);
	void render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle);
};

class Chair : public GameObjects {
public:
	Chair();
	void init(GLuint texID);
	void render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle);
};

class Foot : public GameObjects {
private:
	float xRotAngle = 0.0f, zRotAngle = 0.0f;
	char *model;
public:
	Foot();
	void init(GLuint texID);
	void setModel(int m);
	void setXRotAngle(float rot);
	float getXRotAngle();
	void setZRotAngle(float rot);
	float getZRotAngle();
	void render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle);
};

//player & body
class Player {
private:
	Body body;
	Leg lLeg, rLeg;
	Foot lFoot, rFoot;
	Chair chair;
public:
	Player();
	void init(GLuint texID[]);
	void moveLegs(const Uint8 keys[]);
	glm::vec3 getPosition();
	void setBodyPos(glm::vec3 newPos, GLfloat rotAngle);
	glm::vec3 wallCollision(glm::vec3 pastPos, glm::vec3 currentPos, Wall /*testW*/walls[], GLfloat playerRot);
	void render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle);
};