#include "QuestiaEng/GuiManager/TabBar.h"

TabBar::TabBar(utl::Direction direction)
{
	switch(direction)
	{
	case utl::Direction::down:
	case utl::Direction::downLeft:
	case utl::Direction::downRight:
		dir = utl::Direction::down;
		break;
	case utl::Direction::up:
	case utl::Direction::upLeft:
	case utl::Direction::upRight:
	default:
		dir = utl::Direction::up;
		break;
	}
}

TabBar::~TabBar()
{
	//dtor
}

int TabBar::addTab(std::string text)
{
	int tabID = tabs.size();
	tabs.push_back(Tab(text));
	return tabID;
}

void TabBar::init(std::string tabName, GuiManager& pGuiManager, GuiLoader& guiLoader)
{
	guiManager = &pGuiManager;
	guiLoader.loadGui(pGuiManager, "tabBar");

	int pos_y;
	switch(dir)
	{
	case utl::Direction::down:
		pos_y = 1080-30;
		break;
	case utl::Direction::up:
	default:
		pos_y = 0;
		break;
	}

	guiManager->createGroupFromTemplate(tabName, "tabBarTemplate");
	groupName = tabName;
	button_bar = guiManager->getGroupEntry(groupName, "barTemplate");
	
	//tab positioning along x axis
	unsigned int trav_x = 0;
	//make all tabs
	for(unsigned int i = 0; i < tabs.size(); i++)
	{
		std::string tabName = groupName + "_" + std::to_string(i);
		tabs[i].buttonName = tabName;
		
		guiManager->createButton(tabName, "tabTemplate");
		
		guiManager->setButton(tabName, gui::ButtonCharacteristic::coords, std::make_pair(trav_x, 0));
		guiManager->setButtonAtr(tabName, "buttonText", gui::ButtonAtrCharacteristic::text, tabs[i].tabName);
		
		//adjust to width of text
		sf::Text t(tabs[i].tabName, *guiManager->getFont(), 20);
		unsigned int tabWidth = t.getGlobalBounds().width + 14;
		guiManager->setButtonAtr(tabName, "buttonSprite", gui::ButtonAtrCharacteristic::size, std::make_pair(tabWidth, 28));
		trav_x += tabWidth + 4;
		
		guiManager->addToGroup(groupName, tabName);
	}

	guiManager->setGroupAtr(gui::ButtonCharacteristic::coords_group, std::make_pair(0, pos_y));
}
