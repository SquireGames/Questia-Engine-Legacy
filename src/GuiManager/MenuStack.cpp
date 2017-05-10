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
	int tabID = entries.size();
	entries.push_back(Entry(text));
	return tabID;
}

void MenuStack::init(std::string stackName, int x, int y, GuiManager& pGuiManager, GuiLoader& guiLoader)
{
	guiManager = &pGuiManager;
	listName = stackName;
	
	guiLoader.loadGui(pGuiManager, "menuStack");
	
	guiManager->createList(stackName);
	guiManager->setListTemplate("stackTemplate");
	guiManager->setListSpacing(0, ((dir == utl::Direction::down) ? 30 : -30));
	guiManager->setListPosition(std::make_pair(x, (dir == utl::Direction::down) ? y : (y - 30)));
	
	//make all entries
	for(unsigned int i = 0; i < entries.size(); i++)
	{
		entries[i].buttonName = guiManager->getGroupEntry(guiManager->createListEntry(), "stackButtonTemplate");
		guiManager->setButtonAtr(entries[i].buttonName, "buttonText", gui::ButtonAtrCharacteristic::text, entries[i].entryText);
	}
}

void MenuStack::handleInput(std::u32string& input)
{
	
}

void MenuStack::update(MouseListener& mouse)
{
	
}
