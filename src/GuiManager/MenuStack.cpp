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

int MenuStack::addEntry(std::string text, std::string buttonName)
{
	int tabID = entries.size();
	entries.push_back(Entry(text));
	entries.at(tabID).buttonName = buttonName;
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
		std::string alias = entries[i].buttonName;
		entries[i].buttonName = guiManager->getGroupEntry(guiManager->createListEntry(), "stackButtonTemplate");
		guiManager->createAlias(alias, entries[i].buttonName);
		guiManager->setButtonAtr(entries[i].buttonName, "buttonText", gui::ButtonAtrCharacteristic::text, entries[i].entryText);
	}
	guiManager->setListAtr(listName, gui::ButtonCharacteristic::isVisible, false);
}

void MenuStack::handleInput(std::u32string& input)
{

}

void MenuStack::update(MouseListener& mouse)
{
	if(toHide)
	{
		setActivity(false);
		toHide = false;
		return;
	}
	
	if(mouse.isMouseReleased(ctr::Input::LMouse))
	{
		toHide = true;
	}
}

void MenuStack::setActivity(bool active)
{
	if(isStackActive != active)
	{
		isStackActive = active;
		guiManager->setListAtr(listName, gui::ButtonCharacteristic::isVisible, active);
	}
}

bool MenuStack::isActive()
{
	return isStackActive;
}
