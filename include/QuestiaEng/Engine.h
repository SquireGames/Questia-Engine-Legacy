#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics/RenderWindow.hpp>

#include "QuestiaEng/ResourceManager/ResourceManager.h"
#include "QuestiaEng/Data/InputBuffer.h"
#include "QuestiaEng/GuiManager/GuiManager.h"
#include "QuestiaEng/GuiManager/GuiLoader.h"
#include "QuestiaEng/TileEngine/TileEngine.h"

class Engine
{
public:
	//ctor + dtor
	Engine(std::string windowName, int tickRate);
	~Engine() = default;

	//runtime loop function
	bool run();
	bool tick();
	void processInput();
	void render();
	
	//used in a state
	void terminate() {toTerminate = true;}

	//interfaces
	ResourceManager& 	res() 	{return resourceManager;}
	InputBuffer& 		input() {return inputBuffer;}
	GuiManager&			gui() 	{return guiManager;}
	GuiLoader& 			guiLd() {return guiLoader;}
	TileEngine&	 		tile()	{return tileEngine;}

private:
	sf::RenderWindow window;

	//manager
	ResourceManager resourceManager;
	InputBuffer inputBuffer;
	GuiManager guiManager;
	GuiLoader guiLoader;
	TileEngine tileEngine;

	//timing
	sf::Clock clock;
	sf::Time timePerFrame;
	sf::Time timeSinceLastTick;
	
	//flags
	bool toTerminate = false;
};

#endif // ENGINE_H
