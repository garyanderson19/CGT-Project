#pragma once
#include "GameObjects.h"

class GameEngine {

public:
	GameEngine();
	void render();

	Ground ground;
	Wall lWall, rWall, fWall, bWall, corridor[5];
	Player player;


};