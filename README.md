# PacBird

## About the Application

* The game is built in C++ while using the openGL platform.
* The game is by structure top-down along with fixed frame dungeon rooms.

## Description of Project

* The game world consists of 3 rooms, where each room has a door at the edge. The player's objective is to reach the final gate without being destroyed by the enemies.
* Every room contains of randomly generated walls (apart from edge walls).
* Every room contains of randomly spawned coins and enemies.
* Player's points increase on collecting coins whereas the plyer loses on coming in contact with the moving/ static enemies.
* Difficulty of the game increases with progress.
* 'L' key can be used to toggle the lights.  When the lights are on, only some area surrounding the player will be visible while the rest of the world will go dark. However, playing in dark will give more points to the player.

## Running the Game

* `mkdir build`
* `cd build`
* `cmake ..`
* `make -j`
* `./app`

## Controls of the Game

* 'Enter' to start the Game.
* 'W', 'A', 'S', 'D' for player movement.
* L' to toggle lights ON/ OFF
