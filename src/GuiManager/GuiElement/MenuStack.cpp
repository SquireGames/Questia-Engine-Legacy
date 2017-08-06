#include "QuestiaEng/GuiManager/GuiElement/MenuStack.h"

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

int MenuStack::addEntry(const std::string& text, const std::string& buttonName)
{
	int tabID = entries.size();
	entries.push_back(Entry(text));
	entries.at(tabID).buttonName = buttonName;
	return tabID;
}

void MenuStack::init(const std::string& stackName, int x, int y, GuiManager& guiManager)
{
	this->guiManager = &guiManager;
	listName = stackName;

	guiManager.loadGui("menuStack");

	GuiBuilder& guiBuilder = guiManager.edit();

	guiBuilder.createList(stackName);
	guiBuilder.setListTemplate("stackTemplate");
	guiBuilder.setListSpacing(0, ((dir == utl::Direction::down) ? 24 : -24));
	guiBuilder.setListPosition(std::make_pair(x, (dir == utl::Direction::down) ? y : (y - 24)));

	//make all entries
	for(unsigned int i = 0; i < entries.size(); i++)
	{ 
		int buttonID = entries[i].buttonID = guiBuilder.getGroupEntryID(guiBuilder.createListEntry(), "stackButtonTemplate");
		guiBuilder.createAlias(entries[i].buttonName, buttonID);
		guiBuilder.setBtnAtr(buttonID, "buttonText", gui::BtnAtrChar::text, entries[i].entryText);
	}
	guiBuilder.setListAtr(listName, gui::BtnChar::isVisible, false);
}

void MenuStack::handleInput(std::u32string& input)
{

}

void MenuStack::update(MouseListener& mouse)
{
	if(hideNextTick)
	{
		setActivity(false);
		hideNextTick = false;
		return;
	}

	if(mouse.isMouseReleased(ctr::Input::LMouse))
	{
		hideNextTick = true;
	}
}

void MenuStack::setActivity(bool active)
{
	if(isStackActive != active)
	{
		isStackActive = active;
		guiManager->edit().setListAtr(listName, gui::BtnChar::isVisible, active);
	}
}

bool MenuStack::isActive()
{
	return isStackActive;
}
