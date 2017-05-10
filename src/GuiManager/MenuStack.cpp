#include "QuestiaEng/GuiManager/MenuStack.h"

MenuStack::MenuStack(utl::Direction direction)
{
	switch(direction)
	{
	case utl::Direction::up:
	case utl::Direction::upLeft:
	case utl::Direction::upRight:
		dir = utl::Direction::up;
		break;
	case utl::Direction::down:
	case utl::Direction::downLeft:
	case utl::Direction::downRight:
	default:
		dir = utl::Direction::down;
		break;
	}
}

MenuStack::~MenuStack()
{
	//dtor
}

int MenuStack::addEntry(std::string text)
{
	
}

void MenuStack::init(std::string tabName, GuiManager& pGuiManager, GuiLoader& guiLoader)
{
	
}
