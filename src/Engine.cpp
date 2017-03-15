#include "QuestiaEng/Engine.h"

Engine::Engine(std::string windowName, int tickRate):
	window(sf::VideoMode(1920, 1080), windowName, sf::Style::Default)
	//managers
	, resourceManager()
	, guiManager(window, resourceManager)
	, tileEngine(window, resourceManager)
	//variables
	, timePerFrame(sf::seconds(1.f/std::max(1, tickRate)))
{	
	//TODO load from file
	window.setVerticalSyncEnabled(true);
	window.setView(sf::View(sf::Vector2f(window.getSize().x/2, window.getSize().y/2), sf::Vector2f(window.getSize())));
	
	//TODO remove - test TileEngine
	//tileEngine.loadMap("Demo_1", TileEng::TextureMode::Map, TileEng::TileMode::Batch);
	//tileEngine.setPosition(100,100);
}

bool Engine::run()
{
	return !toTerminate && window.isOpen();
}

bool Engine::tick()
{
	sf::Time elapsedTime = clock.restart();
	timeSinceLastTick += elapsedTime;
	return (timeSinceLastTick > timePerFrame) ? (timeSinceLastTick -= timePerFrame, true) : false;
}

void Engine::processInput()
{
	sf::Event event;

	while(window.pollEvent(event))
	{
		switch(event.type)
		{
		case sf::Event::TextEntered:
			{
				inputBuffer.enqueue(event.text.unicode);
			}
			break;
		case sf::Event::KeyPressed:
			break;
		case sf::Event::KeyReleased:
			break;
		case sf::Event::MouseWheelMoved:
			break;
		case sf::Event::Resized:
			break;
		case sf::Event::MouseButtonReleased:
			break;
		case sf::Event::MouseButtonPressed:
			break;
		case sf::Event::Closed:
			window.close();
			break;
		}
	}
}

void Engine::render()
{
	window.display();
	window.clear(sf::Color::White);
}
