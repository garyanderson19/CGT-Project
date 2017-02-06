#include "GameObjects.h"
#include <iostream>
using namespace std;

#define DEG_TO_RADIAN 0.017453293

glm::vec3 moveF(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle*DEG_TO_RADIAN));
}

glm::vec3 moveR(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
}

//GameObjects methods
GameObjects::GameObjects() {
}

GameObjects::~GameObjects() {
	
}

glm::vec3 GameObjects::getPosition() {
	return position;
}

void GameObjects::setPosition(glm::vec3 newPos) {
	position = newPos;
}



//Cube methods, inherits from GameObjects
void Cube::init(GLuint texID) {
	objectTexture = texID; // change to wall texture when we have one
}

void Cube::init(GLuint texID, GLuint mIC, GLuint mesh) {
	meshIndexCount = mIC;
	objectTexture = texID; // change to wall texture when we have one
	objectMesh = mesh;
}

//Ground methods, inherits from Cube
Ground::Ground() {
	name = "Ground";
}

void Ground::render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview) {
	// draw a cube for ground plane
	glBindTexture(GL_TEXTURE_2D, objectTexture);
	modelview.push(modelview.top());
	modelview.top() = glm::translate(modelview.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
	modelview.top() = glm::scale(modelview.top(), glm::vec3(20.0f, 0.1f, 20.0f));
	rt3d::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(modelview.top()));
	rt3d::setMaterial(shader, material);
	rt3d::drawIndexedMesh(objectMesh, meshIndexCount, GL_TRIANGLES);
	modelview.pop();

}


//Wall methods, inherits from Cube
Wall::Wall() {
	name = "Wall";
	position = { -30.0f, 5.0f, -10.0f };
}

glm::vec3 Wall::getWallVecs(int vec) {
	if (vec == 1)
		return WV.rotation;
	else if (vec == 2)
		return WV.scaling;
}

void Wall::setWall(glm::vec3 tran, glm::vec3 rot, glm::vec3 scale) {
	glm::vec3 emptyVec(0.0f);
	if (tran != emptyVec) {
		WV.translation = tran;
		setPosition(tran);
	}
	if (rot != emptyVec)
		WV.rotation = rot;
	if (scale != emptyVec)
		WV.scaling = scale;
}

void Wall::render(rt3d::materialStruct material, GLuint &shader, stack <glm::mat4> &modelview) {

	//draw walls
	glBindTexture(GL_TEXTURE_2D, objectTexture);

	modelview.push(modelview.top());
	modelview.top() = glm::translate(modelview.top(), WV.translation);
	if (WV.rotation != glm::vec3(0.0f, 0.0f, 0.0f))
		modelview.top() = glm::rotate(modelview.top(), float(90*DEG_TO_RADIAN), WV.rotation);
	modelview.top() = glm::scale(modelview.top(), WV.scaling);
	rt3d::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(modelview.top()));
	rt3d::setMaterial(shader, material);
	rt3d::drawIndexedMesh(objectMesh, meshIndexCount, GL_TRIANGLES);
	modelview.pop();

}


//Leg methods, inherits from GameObjects
Leg::Leg() {
	name = "Leg";
}

void Leg::init(GLuint texID) {

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj(model, verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	objectTexture = texID;
	objectMesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());
}

void Leg::setModel(int m) {
	if (m == 0)
		model = "legv4left2.obj";
	else if (m == 1)
		model = "legv4right2.obj";
}

void Leg::setXRotAngle(float rot) {
	xRotAngle = rot;
}

float Leg::getXRotAngle() {
	return xRotAngle;
}

void Leg::setZRotAngle(float rot) {
	zRotAngle = rot;
}

float Leg::getZRotAngle() {
	return zRotAngle;
}

void Leg::render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle) {
	
	glBindTexture(GL_TEXTURE_2D, objectTexture);
	modelview.push(modelview.top());
	modelview.top() = glm::translate(modelview.top(), position);
	
	modelview.top() = glm::rotate(modelview.top(), float(rotAngle*DEG_TO_RADIAN), glm::vec3(0.0f, -1.0f, 0.0f));
	
	modelview.top() = glm::rotate(modelview.top(), float(xRotAngle*DEG_TO_RADIAN), glm::vec3(-0.3f, 0.0f, 0.0f));
	modelview.top() = glm::rotate(modelview.top(), float(glm::sin(zRotAngle*DEG_TO_RADIAN)), glm::vec3(0.0f, 0.3f, -0.3f));

	modelview.top() = glm::scale(modelview.top(), glm::vec3(0.35f, 0.3f, 0.3f));
	rt3d::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(modelview.top()));
	rt3d::setMaterial(shader, material);
	rt3d::drawIndexedMesh(objectMesh, meshIndexCount, GL_TRIANGLES);
	modelview.pop();

}

//Foot methods, inherits from GameObjects
Foot::Foot() {
	name = "Foot";
}

void Foot::init(GLuint texID) {

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj(model, verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	objectTexture = texID;
	objectMesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());
}

void Foot::setModel(int m) {
	if (m == 0)
		model = "feetv2left.obj";
	else if (m == 1)
		model = "feetv1right.obj";
}

void Foot::setXRotAngle(float rot) {
	xRotAngle = rot;
}

float Foot::getXRotAngle() {
	return xRotAngle;
}

void Foot::setZRotAngle(float rot) {
	zRotAngle = rot;
}

float Foot::getZRotAngle() {
	return zRotAngle;
}

void Foot::render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle) {
	glBindTexture(GL_TEXTURE_2D, objectTexture);
	modelview.push(modelview.top());
	modelview.top() = glm::translate(modelview.top(), position);

	modelview.top() = glm::rotate(modelview.top(), float(rotAngle*DEG_TO_RADIAN), glm::vec3(0.0f, -0.3f, 0.0f));

	modelview.top() = glm::rotate(modelview.top(), float(xRotAngle*DEG_TO_RADIAN), glm::vec3(-0.3f, 0.0f, 0.0f));
	modelview.top() = glm::rotate(modelview.top(), float(glm::sin(zRotAngle*DEG_TO_RADIAN)), glm::vec3(0.0f, 0.3f, -0.3f));
	
	
	modelview.top() = glm::scale(modelview.top(), glm::vec3(0.34f));
	rt3d::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(modelview.top()));
	rt3d::setMaterial(shader, material);
	rt3d::drawIndexedMesh(objectMesh, meshIndexCount, GL_TRIANGLES);
	modelview.pop();
}




//Body methods, inherits from GameObjects
Body::Body() {
	name = "Body";
	position = { 2.0f, 2.6f, -10.0f };
}

void Body::init(GLuint texID) {
	
	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj("model3smoothv2.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	objectTexture = texID;
	objectMesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());
}

void Body::render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle) {
	glBindTexture(GL_TEXTURE_2D, objectTexture);
	modelview.push(modelview.top());
	modelview.top() = glm::translate(modelview.top(), position);

	modelview.top() = glm::rotate(modelview.top(), float(90.0f*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));

	modelview.top() = glm::rotate(modelview.top(), float(rotAngle*DEG_TO_RADIAN), glm::vec3(0.0f, -1.0f, 0.0f));

	modelview.top() = glm::scale(modelview.top(), glm::vec3(0.35f));
	rt3d::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(modelview.top()));
	rt3d::setMaterial(shader, material);
	rt3d::drawIndexedMesh(objectMesh, meshIndexCount, GL_TRIANGLES);
	modelview.pop();
}


//Chair methods, inherits from GameObjects
Chair::Chair() {
	name = "Chair";
}

void Chair::init(GLuint texID) {

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj("chairv6.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	objectTexture = texID;
	objectMesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());
}

void Chair::render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle) {
	glBindTexture(GL_TEXTURE_2D, objectTexture);
	modelview.push(modelview.top());
	
	modelview.top() = glm::translate(modelview.top(), position);

	modelview.top() = glm::rotate(modelview.top(), float(90.0f*DEG_TO_RADIAN), glm::vec3(0.0f, -1.0f, 0.0f));
	modelview.top() = glm::rotate(modelview.top(), float(rotAngle*DEG_TO_RADIAN), glm::vec3(0.0f, -1.0f, 0.0f));
	
	modelview.top() = glm::scale(modelview.top(), glm::vec3(0.75f));
	rt3d::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(modelview.top()));
	rt3d::setMaterial(shader, material);
	rt3d::drawIndexedMesh(objectMesh, meshIndexCount, GL_TRIANGLES);
	modelview.pop();
}


//Player methods, contains two Legs, a Body, a Chair and two Feet
Player::Player() {
	
}

void Player::init(GLuint texID[]) {
	body.init(texID[0]);
	
	lLeg.setModel(0);
	lLeg.init(texID[0]);
	rLeg.setModel(1);
	rLeg.init(texID[0]);
	lFoot.setModel(0);
	lFoot.init(texID[0]);
	rFoot.setModel(1);
	rFoot.init(texID[0]);

	chair.init(texID[0]);

	glm::vec3 temp = body.getPosition();
	float move = 0.45f, footYTrans = 2.35f;

	temp[0] -= move; temp[2] -= 1.75f;
	lLeg.setPosition(temp);
	temp[1] -= footYTrans; temp[2] -= 0.19f;
	lFoot.setPosition(temp);
	
	temp = lLeg.getPosition();	temp[0] += 2 * move;
	rLeg.setPosition(temp);
	temp = lFoot.getPosition(); temp[0] += 0.92f; temp[2] -= 0.04f;
	rFoot.setPosition(temp);

	temp = body.getPosition(); temp[1] += 0.35f; temp[2] -= 0.65f;
	chair.setPosition(temp);
}

void Player::moveLegs(const Uint8 keys[]) {

	if (keys[SDL_SCANCODE_I]) { 
		if (lLeg.getXRotAngle() > -63.0f) {
			lLeg.setXRotAngle(lLeg.getXRotAngle() - 3.0f); rLeg.setXRotAngle(rLeg.getXRotAngle() - 3.0f);
			lFoot.setXRotAngle(lFoot.getXRotAngle() - 3.0f); rFoot.setXRotAngle(rFoot.getXRotAngle() - 3.0f);
		}
		
	}

	if (keys[SDL_SCANCODE_K]) { 
		if (lLeg.getXRotAngle() < 9.0f) {
			lLeg.setXRotAngle(lLeg.getXRotAngle() + 3.0f); rLeg.setXRotAngle(rLeg.getXRotAngle() + 3.0f);
			lFoot.setXRotAngle(lFoot.getXRotAngle() + 3.0f); rFoot.setXRotAngle(rFoot.getXRotAngle() + 3.0f);
		}
		
	}

}

glm::vec3 Player::getPosition() {
	return body.getPosition();
}

void Player::setBodyPos(glm::vec3 newPos, GLfloat rotAngle) {

	body.setPosition(newPos);
	
	glm::vec3 temp = body.getPosition();

	temp = moveF(temp, rotAngle, 1.75f);
	temp = moveR(temp, rotAngle, -0.45f);
	lLeg.setPosition(temp);
	
	temp[1] -= (2.1f*std::cos(lFoot.getXRotAngle()*DEG_TO_RADIAN))+(0.1*std::sin(lFoot.getXRotAngle()*DEG_TO_RADIAN));
	temp = moveF(temp, rotAngle, (0.19f+(2.15f*std::sin(-(lFoot.getXRotAngle())*DEG_TO_RADIAN)))); 
	lFoot.setPosition(temp);
	
	temp = lLeg.getPosition();
	temp = moveR(temp, rotAngle, 0.90f);
	rLeg.setPosition(temp);
	
	temp[1] -= (2.1f*std::cos(rFoot.getXRotAngle()*DEG_TO_RADIAN)) + (0.1*std::sin(rFoot.getXRotAngle()*DEG_TO_RADIAN));
	temp = moveF(temp, rotAngle, 0.23f+2.19f*std::sin(-(rFoot.getXRotAngle())*DEG_TO_RADIAN));
	
	temp = moveR(temp, rotAngle, 0.02f);
	rFoot.setPosition(temp);
	
	temp = body.getPosition();
	temp[1] += 0.35f;
	temp = moveF(temp, rotAngle, 0.65f);
	chair.setPosition(temp);

}

glm::vec3 Player::wallCollision(glm::vec3 pastPos, glm::vec3 currentPos, Wall walls[], GLfloat playerRot) {
		
	glm::vec3 returnPos = currentPos, realPos = moveF(currentPos, playerRot, 1.0f);
	for (int i = 0; i < 9; i++)
	if (walls[i].getWallVecs(1).y != 0.0f) {
		if (((((realPos.x - 1.6f) <= (walls[i].getPosition().x + walls[i].getWallVecs(2).z))
			&& ((realPos.x - 1.6f) >= (walls[i].getPosition().x - walls[i].getWallVecs(2).z)))
			|| ((realPos.x + 1.6f <= (walls[i].getPosition().x + walls[i].getWallVecs(2).z))
				&& (realPos.x + 1.6f >= (walls[i].getPosition().x - walls[i].getWallVecs(2).z))))

			&&

			(((realPos.z - 1.6f <= (walls[i].getPosition().z + walls[i].getWallVecs(2).x))
				&& (realPos.z - 1.6f >= (walls[i].getPosition().z - walls[i].getWallVecs(2).x)))
				|| ((realPos.z + 1.6f >= (walls[i].getPosition().z - walls[i].getWallVecs(2).x))
					&& (realPos.z + 1.6f <= (walls[i].getPosition().z + walls[i].getWallVecs(2).x))))) {
			returnPos = pastPos;
		}
	}
	else if (walls[i].getWallVecs(1).y == 0)
	{
		if (((((realPos.x - 1.6f) <= (walls[i].getPosition().x + walls[i].getWallVecs(2).x))
			&& ((realPos.x - 1.6f) >= (walls[i].getPosition().x - walls[i].getWallVecs(2).x)))
			|| ((realPos.x + 1.6f <= (walls[i].getPosition().x + walls[i].getWallVecs(2).x))
				&& (realPos.x + 1.6f >= (walls[i].getPosition().x - walls[i].getWallVecs(2).x))))

			&&

			(((realPos.z - 1.6f <= (walls[i].getPosition().z + walls[i].getWallVecs(2).z))
				&& (realPos.z - 1.6f >= (walls[i].getPosition().z - walls[i].getWallVecs(2).z)))
				|| ((realPos.z + 1.6f >= (walls[i].getPosition().z - walls[i].getWallVecs(2).z))
					&& (realPos.z + 1.6f <= (walls[i].getPosition().z + walls[i].getWallVecs(2).z))))) {
			returnPos = pastPos;
		}
	}

	return returnPos;
}

void Player::render(rt3d::materialStruct material, GLuint &shader, stack<glm::mat4> &modelview, GLfloat rotAngle) {
	
	body.render(material, shader, modelview, rotAngle);
	chair.render(material, shader, modelview, rotAngle);
	lLeg.render(material, shader, modelview, rotAngle);
	lFoot.render(material, shader, modelview, rotAngle);
	rLeg.render(material, shader, modelview, rotAngle);
	rFoot.render(material, shader, modelview, rotAngle);

}