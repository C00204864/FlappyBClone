#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"

// Game Includes
#include "Bird.h"
#include "PipePair.h"

// Neuroevolution
#include "ANN.h"
#include "Pool.h"

class Game
{
public:
	// Public Methods
	Game();
	~Game();
	void run();
private:
	// Private Methods
	void processEvents();
	void update(sf::Time t_deltaTime);
	void render();
	void collisionCheck();
	void reset();
	void setupANN();
	PipePair getClosestPipePair();

	// Private Members
	sf::RenderWindow m_window; // main SFML window
	bool m_exitGame; // control exiting game
	sf::Time timePerFrame;

	const unsigned int SCREEN_WIDTH = 1280;
	const unsigned int SCREEN_HEIGHT = 720;

	Bird m_bird;
	std::vector<PipePair> m_pipePairs;

	bool m_trainAi;
	ANN * m_ann;
	Pool * m_pool;
	int m_memberNo;
	int m_score;
	int m_gens = 0;
};

#endif // !GAME_H