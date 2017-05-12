#include "QuestiaEng/Engine.h"

Engine::Engine(std::string windowName, int tickRate):
	//options
	saveFile()
	//window
	, window(sf::VideoMode(1920, 1080), windowName, saveFile.getWindowMode() ? sf::Style::Default : sf::Style::Fullscreen)
	, size_real(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height)
	, size_scaled(window.getSize().x, window.getSize().y)
	, scaleFactor(1920.f / static_cast<float>(size_scaled.x), 1080.f / static_cast<float>(size_scaled.y))
	, mousePos(sf::Mouse::getPosition(window).x * scaleFactor.x, sf::Mouse::getPosition(window).y * scaleFactor.y)
	//input
	, mouseListener()
	//managers
	, resourceManager()
	, guiManager(window, resourceManager)
	, guiHandler(mouseListener)
	, guiLoader()
	, tileEngine(window, resourceManager)
	, tileEngineEditor(window, resourceManager, tileEngine)
	, stateManager(*this)
	//variables
	, font()
	, inputBuffer()
	, clock()
	, timePerFrame(sf::seconds(1.f/std::max(1, tickRate)))
	, timeSinceLastTick()

{
	//load render refresh rate
	switch(saveFile.getFps())
	{
	case 0:
		window.setVerticalSyncEnabled(true);
		break;
	case -1:
		window.setFramerateLimit(100000);
		window.setVerticalSyncEnabled(false);
		break;
	default:
		window.setFramerateLimit(saveFile.getFps());
		window.setVerticalSyncEnabled(false);
		break;
	}
	//center view
	window.setView(sf::View(sf::Vector2f(window.getSize().x/2, window.getSize().y/2), sf::Vector2f(window.getSize())));

	//set font
	if(!font.loadFromFile("Media/Fonts/" + saveFile.getFont()))
	{
		font.loadFromFile("Media/Fonts/acidstructure.ttf");
	}
	guiManager.setFont(font);
	
	//allocate space for inputBuffer
	inputBuffer.reserve(4);

	//TODO remove, print options for debugging
	std::cout << "------------------" << std::endl;
	std::cout << "LAUNCH OPTIONS " << std::endl;
	std::cout << "------------------" << std::endl;
	std::cout << "Window mode:  " << (saveFile.getWindowMode() ? "Default" : "Fullscreen") << std::endl;
	std::cout << "Window size:  " << size_real.x << ", " << size_real.y << std::endl;
	std::cout << "Scaled size:  " << size_scaled.x << ", " << size_scaled.y << std::endl;
	std::cout << "Scale factor: " << scaleFactor.x << ", " << scaleFactor.y << std::endl;
	std::cout << "------------------" << std::endl;

}

bool Engine::run()
{
	return !toTerminate && window.isOpen();
}

bool Engine::tick()
{
	//update scaled mouse position
	sf::Time elapsedTime = clock.restart();
	timeSinceLastTick += elapsedTime;
	if(timeSinceLastTick > timePerFrame)
	{
		timeSinceLastTick -= timePerFrame;
		mousePos = utl::Vector2f(sf::Mouse::getPosition(window).x * scaleFactor.x, sf::Mouse::getPosition(window).y * scaleFactor.y);
		guiManager.setMousePosition(mousePos);
		mouseListener.update();
		mouseListener.setMousePos(mousePos);
		guiHandler.update();
		return true;
	}
	return false;
}

void Engine::processInput()
{
	sf::Event event;

	mouseListener.setScroll(0);
	while(window.pollEvent(event))
	{
		switch(event.type)
		{
		case sf::Event::TextEntered:
			//TODO only allow certain ranges of unicode
			inputBuffer += event.text.unicode;
			break;
		case sf::Event::KeyPressed:
			break;
		case sf::Event::KeyReleased:
			break;
		case sf::Event::MouseWheelMoved:
			mouseListener.setScroll(event.mouseWheel.delta);
			break;
		case sf::Event::Resized:
			size_scaled = utl::Vector2ui(window.getSize().x, window.getSize().y);
			scaleFactor = utl::Vector2f(1920.f / static_cast<float>(size_scaled.x), 1080.f / static_cast<float>(size_scaled.y));
			break;
		case sf::Event::MouseButtonReleased:
			break;
		case sf::Event::MouseButtonPressed:
			break;
		case sf::Event::Closed:
			window.close();
			break;
		default:
			break;
		}
	}
}

void Engine::render()
{
	window.display();
	window.clear(sf::Color::White);
}
