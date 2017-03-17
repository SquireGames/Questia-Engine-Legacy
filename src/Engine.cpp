#include "QuestiaEng/Engine.h"

Engine::Engine(std::string windowName, int tickRate):
	//options
	saveFile()
	//window
	, window(sf::VideoMode(1920, 1080), windowName, saveFile.getWindowMode() ? sf::Style::Default : sf::Style::Fullscreen)
	//managers
	, resourceManager()
	, inputBuffer()
	, guiManager(window, resourceManager)
	, guiLoader()
	, tileEngine(window, resourceManager)
	, stateManager(*this)
	//variables
	, timePerFrame(sf::seconds(1.f/std::max(1, tickRate)))
{
	//load render refresh rate
	switch(saveFile.getFps())
	{
	case 0:
		window.setVerticalSyncEnabled(true);
		break;
	case -1:
		window.setFramerateLimit(100000);
	default:
		window.setVerticalSyncEnabled(false);
		break;
	}
	//center view
	window.setView(sf::View(sf::Vector2f(window.getSize().x/2, window.getSize().y/2), sf::Vector2f(window.getSize())));
	
	//TODO remove, print options for debugging
	std::cout << "------------------" << std::endl;
	std::cout << "LAUNCH OPTIONS " << std::endl;
	std::cout << "------------------" << std::endl;
	std::cout << "Window mode: " << (saveFile.getWindowMode() ? "Default" : "Fullscreen") << std::endl;
	std::cout << "------------------" << std::endl;
	
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
