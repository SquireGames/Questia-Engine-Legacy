#include "QuestiaEng/Engine.h"

#ifdef _WIN32
#if !(__GNUC__ == 6 && \
			  __GNUC_MINOR__ == 1 && \
			  __GNUC_PATCHLEVEL__ == 0)
#warning Compiler version may be incompatible with pre-built SFML dlls. Use g++ 6.1.0 (DW2).
#endif // __GNUC__
#elif linux
#if !(__GNUC__ == 6 && \
			  __GNUC_MINOR__ == 2 && \
			  __GNUC_PATCHLEVEL__ == 0)
#warning Compiler version may be incompatible with pre-built SFML dlls. Use g++ 6.2.0.
#endif // __GNUC__
#else
#warning Your operating system was not yet tested with the prebuilt SFML dlls.
#endif

Engine::Engine(std::string windowName, int tickRate, int majorVersion, int minorVersion, int revision, std::string versionSuffix):
	//options
	saveFile()
	//window
	, window(sf::VideoMode(1920, 1080),
	         windowName,
	         saveFile.getWindowMode() ? sf::Style::Default : sf::Style::Fullscreen,
	         sf::ContextSettings(0,0,16))
	, size_real(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height)
	, size_scaled(window.getSize().x, window.getSize().y)
	, scaleFactor(1920.f / static_cast<float>(size_scaled.x), 1080.f / static_cast<float>(size_scaled.y))
	, mousePos(sf::Mouse::getPosition(window).x * scaleFactor.x, sf::Mouse::getPosition(window).y * scaleFactor.y)
	//input
	, mouseListener()
	//managers
	, resourceManager()
	, guiManager(window, resourceManager)
	, guiElementUpdater(mouseListener)
	, tileEngine(window, resourceManager)
	, tileEngineEditor(window, resourceManager, tileEngine)
	, entityManager(resourceManager, window)
	, stateManager(*this)
	//variables
	, inputBuffer()
	, clock()
	, timePerFrame(sf::seconds(1.f/std::max(1, tickRate)))
	, timeSinceLastTick()
	, majorVersion(majorVersion)
	, minorVersion(minorVersion)
	, revision(revision)
	, versionSuffix(versionSuffix)
{
	//load render refresh rate
	switch(saveFile.getFps())
	{
	case 0:
		window.setVerticalSyncEnabled(true);
		break;
	case -1:
		window.setFramerateLimit(100'000);
		window.setVerticalSyncEnabled(false);
		break;
	default:
		window.setFramerateLimit(saveFile.getFps());
		window.setVerticalSyncEnabled(false);
		break;
	}
	//center view
	window.setView(sf::View(sf::Vector2f(window.getSize().x/2, window.getSize().y/2), sf::Vector2f(window.getSize())));

	//set font and language
	///TODO find better way to match fonts to languages
	sf::Font font;
	guiManager.edit().setLang(saveFile.getLang());
	if(saveFile.getLang() != "en")
	{
		font.loadFromFile("Media/Fonts/unifont-9.0.06.ttf");
	}
	else
	{
		if(!font.loadFromFile("Media/Fonts/" + saveFile.getFont()))
		{
			font.loadFromFile("Media/Fonts/unifont-9.0.06.ttf");
		}
	}
	guiManager.edit().setFont(font);

	//allocate space for inputBuffer
	inputBuffer.reserve(4);

#ifdef DEBUGMODE
	//std::freopen("log.txt", "w", stdout);
	std::cout << "------------------" << std::endl;
	std::cout << "LAUNCH OPTIONS " << std::endl;
	std::cout << "------------------" << std::endl;
	std::cout << "Window mode:  " << (saveFile.getWindowMode() ? "Default" : "Fullscreen") << std::endl;
	std::cout << "Window size:  " << size_real.x << ", " << size_real.y << std::endl;
	std::cout << "Scaled size:  " << size_scaled.x << ", " << size_scaled.y << std::endl;
	std::cout << "Scale factor: " << scaleFactor.x << ", " << scaleFactor.y << std::endl;
	std::cout << "------------------" << std::endl;
#endif
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
		return true;
	}
	return false;
}

void Engine::processInput()
{
	sf::Event event;

	lastInput = ctr::Input::None;

	mousePos = utl::Vector2f(sf::Mouse::getPosition(window).x * scaleFactor.x, sf::Mouse::getPosition(window).y * scaleFactor.y);
	guiManager.setMousePosition(mousePos);
	mouseListener.update();
	mouseListener.setMousePos(mousePos);

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
			lastInput = static_cast<ctr::Input>(event.key.code);
			break;
		case sf::Event::KeyReleased:
			break;
		case sf::Event::MouseWheelMoved:
			mouseListener.setScroll(event.mouseWheel.delta);
			break;
		case sf::Event::Resized:
			fixWindowScale();
			break;
		case sf::Event::MouseButtonReleased:
			mouseListener.update_released(event.mouseButton.button);
			break;
		case sf::Event::MouseButtonPressed:
			mouseListener.update_pressed(event.mouseButton.button);
			break;
		case sf::Event::Closed:
			window.close();
			break;
		default:
			break;
		}
	}
	guiElementUpdater.update(inputBuffer);
	inputBuffer.clear();
}

void Engine::render()
{
	window.display();
	window.clear(sf::Color::White);
}

std::string Engine::getVersion()
{
	return std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." +
	       std::to_string(revision) + " " + versionSuffix;
}

std::string Engine::getVersion_eng()
{
	return std::to_string(majorVersion_eng) + "." + std::to_string(minorVersion_eng) + "." +
	       std::to_string(revision_eng) + " " + versionSuffix_eng;
}

void Engine::fixWindowScale()
{
	size_scaled = utl::Vector2ui(window.getSize().x, window.getSize().y);
	scaleFactor = utl::Vector2f(1920.f / static_cast<float>(size_scaled.x), 1080.f / static_cast<float>(size_scaled.y));
}
