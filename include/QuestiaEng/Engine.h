#ifndef ENGINE_H
#define ENGINE_H

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Font.hpp"

#include "QuestiaEng/ResourceManager/ResourceManager.h"
#include "QuestiaEng/GuiManager/GuiManager.h"
#include "QuestiaEng/GuiManager/GuiHandler.h"
#include "QuestiaEng/GuiManager/GuiLoader.h"
#include "QuestiaEng/TileEngine/TileEngine.h"
#include "QuestiaEng/StateManager/StateManager.h"

#include "QuestiaEng/SaveFile_Options.h"

#include "QuestiaEng/Utl/Type/Vector2.h"
#include "QuestiaEng/Utl/Controls/MouseListener.h"

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
	sf::RenderWindow&   win()   {return window;}
	ResourceManager& 	res() 	{return resourceManager;}
	GuiManager&			gui() 	{return guiManager;}
	GuiHandler& 		guiH()	{return guiHandler;}
	GuiLoader& 			guiLd() {return guiLoader;}
	TileEngine&	 		tile()	{return tileEngine;}
	StateManager& 		state() {return stateManager;}

	//input
	MouseListener& 		mouse() {return mouseListener;}

private:
	//options
	SaveFile_Options saveFile;

	//main window
	sf::RenderWindow window;
	utl::Vector2ui size_real;
	utl::Vector2ui size_scaled;
	utl::Vector2f scaleFactor;
	utl::Vector2f mousePos;

	//manager
	ResourceManager resourceManager;
	GuiManager guiManager;
	GuiHandler guiHandler;
	GuiLoader guiLoader;
	TileEngine tileEngine;
	StateManager stateManager;

	//input
	MouseListener mouseListener;

	//text
	sf::Font font;
	
	//input buffer
	std::u32string inputBuffer;

	//timing
	sf::Clock clock;
	sf::Time timePerFrame;
	sf::Time timeSinceLastTick;

	//flags
	bool toTerminate = false;
};

#endif // ENGINE_H
