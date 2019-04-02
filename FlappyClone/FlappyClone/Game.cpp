#include "Game.h"

/// <summary>
/// Constructor for the game object
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ 1280, 720, 32 }, "Flappy Clone" },
	m_exitGame{ false }, // When true game will exit
	m_bird{ 250, 400 },
	m_trainAi{ true },
	m_memberNo{ 0 },
	m_score{ 0 }
{
	reset();
	if (m_trainAi)
	{
		setupANN();
	}
}

/// <summary>
/// Destructor for the game object
/// </summary>
Game::~Game() 
{
	if (m_trainAi)
	{
		delete m_ann;
		delete m_pool;
	}
}

/// <summary>
/// Run the Game
/// </summary>
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	timePerFrame = sf::seconds(1.f / 60000000000000000.f); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // As many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // At least 60 fps
			update(timePerFrame); // 60 fps
		}
		if (!m_trainAi || timePerFrame == sf::seconds(1 / 60.f))
		{
			render();
		}
		//render(); // As many as possible
	}
}

/// <summary>
/// Process the game events
/// </summary>
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (sf::Event::Closed == event.type) // window message
		{
			m_window.close();
		}
		if (sf::Event::KeyPressed == event.type) //user key press
		{
			if (sf::Keyboard::Escape == event.key.code)
			{
				m_exitGame = true;
			}
		}
	}
}

/// <summary>
/// Update loop for the game
/// </summary>
/// <param name="t_deltaTime">deltatime</param>
void Game::update(sf::Time t_deltaTime)
{
	PipePair pair = getClosestPipePair();
	if (m_exitGame)
	{
		m_window.close();
	}
	for (int i = 0; i < m_pipePairs.size(); ++i)
	{
		m_pipePairs.at(i).update();
	}
	collisionCheck();
	if (m_trainAi)
	{
		PipePair pair = getClosestPipePair();
		auto nodes = m_ann->getAllNodes();
		m_ann->reset();
		nodes["input1"]->setValue((pair.getGapY() + 100) / SCREEN_HEIGHT);
		nodes["input2"]->setValue((pair.getX() - 250.f) / 250.f);
		nodes["input3"]->setValue(m_bird.getY() / SCREEN_HEIGHT);
		m_bird.updateNN(nodes["output"]->getOutput() > 0.5f);
		m_score++;
		if (m_score > 10000)
		{
			timePerFrame = sf::seconds(1 / 60.f);
		}
	}
	else
	{
		m_bird.update();
	}
	if (!m_bird.isAlive())
	{
		if (m_trainAi)
		{
			auto & members = m_pool->getMembers();

			members.at(m_memberNo).fitness = m_score;
			//std::cout << m_score << std::endl;
			m_score = 0;
			++m_memberNo;
			if (m_memberNo < members.size())
			{
				srand(time(NULL));
				m_ann->reset();
				m_ann->applyChromosome(members.at(m_memberNo).chromosome);
				srand(0);
			}
			else
			{
				m_memberNo = 0;
				++m_gens;
				std::cout << m_gens << std::endl;
				int elite = m_gens / 25 + 1;
				srand(time(NULL));
				m_pool->runGeneration(2);
				srand(0);
				m_ann->reset();
				m_ann->applyChromosome(members.at(m_memberNo).chromosome);
			}
		}
		reset();
		m_bird.setAliveStatus(true);
	}
}

/// <summary>
/// Draw loop for the game
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::Black);
	for (int i = 0; i < m_pipePairs.size(); ++i)
	{
		m_pipePairs.at(i).render(m_window);
	}
	m_bird.render(m_window);
	m_window.display();
}

void Game::collisionCheck()
{
	sf::FloatRect birdRect = m_bird.getShape().getGlobalBounds();
	if (birdRect.top > SCREEN_HEIGHT || birdRect.top < -30)
	{
		m_bird.setAliveStatus(false);
		return;
	}
	for (PipePair pair : m_pipePairs)
	{
 		if (pair.checkCollision(birdRect))
		{
 			m_bird.setAliveStatus(false);
			return;
		}
	}
}

void Game::reset()
{
	m_bird.setPosition(sf::Vector2f(250, 400));
	m_pipePairs.clear();
	float startX = 700;
	float gap = 250;
	srand(time(NULL));
	for (int i = 0; i < 8; ++i)
	{
		PipePair newPair(startX + gap * i, SCREEN_HEIGHT, 8, gap);
		m_pipePairs.push_back(newPair);
	}
}

void Game::setupANN()
{
	m_ann = new ANN(0.5f, 1.f, false);
	m_ann->createInputNode("input1");
	m_ann->createInputNode("input2");
	m_ann->createInputNode("input3");

	m_ann->createNode("hidden1");
	m_ann->createNode("hidden2");
	m_ann->createNode("hidden3");
	m_ann->createNode("hidden4");
	m_ann->createNode("hidden5");

	m_ann->createOutputNode("output");

	m_ann->createConnection("input1", "hidden1");
	m_ann->createConnection("input1", "hidden2");
	m_ann->createConnection("input1", "hidden3");
	m_ann->createConnection("input1", "hidden4");
	m_ann->createConnection("input1", "hidden5");

	m_ann->createConnection("input2", "hidden1");
	m_ann->createConnection("input2", "hidden2");
	m_ann->createConnection("input2", "hidden3");
	m_ann->createConnection("input2", "hidden4");
	m_ann->createConnection("input2", "hidden5");

	m_ann->createConnection("input3", "hidden1");
	m_ann->createConnection("input3", "hidden2");
	m_ann->createConnection("input3", "hidden3");
	m_ann->createConnection("input3", "hidden4");
	m_ann->createConnection("input3", "hidden5");

	m_ann->createConnection("hidden1", "output");
	m_ann->createConnection("hidden2", "output");
	m_ann->createConnection("hidden3", "output");
	m_ann->createConnection("hidden4", "output");
	m_ann->createConnection("hidden5", "output");

	m_pool = new Pool(m_ann->genChromosome(), 200, 0.1f, -1.5f, 1.5f);
	m_ann->applyChromosome(m_pool->getMembers().at(0).chromosome);
}

PipePair Game::getClosestPipePair()
{
	int closestIndex = 0;
	float closestDist = INT_MAX;
	for (int i = 0; i < m_pipePairs.size(); ++i)
	{
		if (m_pipePairs.at(i).getX() <= closestDist && m_pipePairs.at(i).getX() > 250)
		{
			closestDist = m_pipePairs.at(i).getX();
			closestIndex = i;
		}
	}
	return m_pipePairs.at(closestIndex);
}
