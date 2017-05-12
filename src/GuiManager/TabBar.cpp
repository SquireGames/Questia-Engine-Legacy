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
	int tabID = mostRecentTab = tabs.size();
	tabs.push_back(Tab(text, dir));
	return tabID;
}

void TabBar::addEntry(int tabID, std::string entryText, std::string buttonName)
{
	tabs.at(tabID).menu.addEntry(entryText, buttonName);
}
void TabBar::addEntry(std::string entryText, std::string buttonName)
{
	tabs.at(mostRecentTab).menu.addEntry(entryText, buttonName);
}

void TabBar::init(std::string tabBarName, GuiManager& pGuiManager, GuiLoader& guiLoader)
{
	guiManager = &pGuiManager;
	guiLoader.loadGui(pGuiManager, "tabBar");

	int pos_y = ((dir == utl::Direction::down) ? 1080-30 : 0);

	guiManager->createGroupFromTemplate(tabBarName, "tabBarTemplate");
	groupName = tabBarName;
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

		//make MenuStacks
		tabs[i].menu.init("S_" + tabName, trav_x, ((dir == utl::Direction::down) ? 1080-30 : 30), pGuiManager, guiLoader);

		//adjust to width of text
		sf::Text t(tabs[i].tabName, *guiManager->getFont(), 20);
		unsigned int tabWidth = t.getGlobalBounds().width + 21;
		guiManager->setButtonAtr(tabName, "buttonSprite", gui::ButtonAtrCharacteristic::size, std::make_pair(tabWidth, 28));
		guiManager->setButton(tabName, gui::ButtonCharacteristic::bounds, "buttonSprite");
		trav_x += tabWidth;

		guiManager->addToGroup(groupName, tabName);
	}
	guiManager->setGroupAtr(groupName, gui::ButtonCharacteristic::coords_group, std::make_pair(0, pos_y));
}

void TabBar::update(MouseListener& mouse)
{
	if(mouse.isMouseReleased(ctr::Input::LMouse))
	{
		for(Tab& tab : tabs)
		{
			if(guiManager->isClicked(tab.buttonName))
			{
				tab.menu.setActivity(!tab.menu.isActive());
				continue;
			}
			tab.menu.update(mouse);
		}
		return;
	}
	else
	{
		for(Tab& tab : tabs)
		{
			tab.menu.update(mouse);
		}
	}
}

void TabBar::setActivity(bool isActive)
{
	for(Tab& tab : tabs)
	{
		tab.menu.setActivity(false);
	}
	guiManager->setGroupAtr(groupName, gui::ButtonCharacteristic::isVisible, isActive);
}
