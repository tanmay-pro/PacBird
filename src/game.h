/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <bits/stdc++.h>
using namespace std;

#include "game_level.h"
#include "ball_object_collisions.h"

// Represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
	GAME_LOST
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(50.0f, 50.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(400.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(400.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 30.0f;
// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
	// game state
	GameState               State;
	bool                    Keys[1024];
	unsigned int            Width, Height;
	std::vector<GameLevel>  Levels;
	unsigned int            Level;
	bool                    change;
	bool                    light;
	time_t                  start_time;
	// constructor/destructor
	unsigned int Lives;
	Game(unsigned int width, unsigned int height);
	~Game();
	// initialize game state (load all shaders/textures/levels)
	void Init();
	// game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void DoCollisions();
	void NextLevel();
	bool enemyCollision(GameObject& enemy);
};

void make_map(int level);
void check_valid(vector<vector<int>> &mapping);

#endif