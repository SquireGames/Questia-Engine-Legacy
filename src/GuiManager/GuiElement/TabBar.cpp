#include "QuestiaEng/GuiManager/GuiElement/TabBar.h"

TabBar::TabBar(utl::Direction direction, int offset):
	offset_y(offset)
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

int TabBar::addTab(const std::string& text, const std::string& buttonName)
{
	int tabID = mostRecentTab = tabs.size();
	tabs.push_back(Tab(text, buttonName, dir));
	return tabID;
}

void TabBar::addEntry(int tabID, const std::string& entryText, const std::string& buttonName)
{
	tabs.at(tabID).menu.addEntry(entryText, buttonName);
}
void TabBar::addEntry(const std::string& entryText, const std::string&buttonName)
{
	tabs.at(mostRecentTab).menu.addEntry(entryText, buttonName);
}

void TabBar::init(const std::string& tabBarName, GuiManager& guiManager, GuiLoader& guiLoader)
{
	this->guiManager = &guiManager;
	guiLoader.loadGui("tabBar");
	
	GuiBuilder& guiBuilder = guiManager.edit();

	int pos_y = ((dir == utl::Direction::down) ? 1080-22-offset_y : 0+offset_y);

	guiBuilder.createGroupFromTemplate(tabBarName, "tabBarTemplate");
	groupName = tabBarName;
	button_bar = guiBuilder.getGroupEntry(groupName, "barTemplate");

	//tab positioning along x axis
	unsigned int trav_x = 0;
	//make all tabs
	for(unsigned int i = 0; i < tabs.size(); i++)
	{
		std::string tabName = (tabs[i].buttonName.size() > 0) ? tabs[i].buttonName : (groupName + "-" + std::to_string(i));
		tabs[i].buttonName = tabName;

		guiBuilder.createButton(tabName, "tabTemplate");

		guiBuilder.setButton(tabName, gui::BtnChar::coords, std::make_pair(trav_x, 0));
		guiBuilder.setBtnAtr(tabName, "buttonText", gui::BtnAtrChar::text, tabs[i].tabName);

		//make MenuStacks
		tabs[i].menu.init("S_" + tabName, trav_x, ((dir == utl::Direction::down) ? 1080-22-offset_y : 22 + offset_y), guiManager, guiLoader);

		//adjust to width of text
		sf::Text t(tabs[i].tabName, *guiBuilder.getFont(), 15);
		unsigned int tabWidth = t.getGlobalBounds().width + 14;
		guiBuilder.setBtnAtr(tabName, "buttonSprite", gui::BtnAtrChar::size, std::make_pair(tabWidth, 22));
		guiBuilder.setButton(tabName, gui::BtnChar::bounds, "buttonSprite");
		guiBuilder.createBtnAtr(tabName, "hover", gui::BtnAtr::Hover);
		guiBuilder.setBtnAtr(tabName, "hover", gui::BtnAtrChar::color, sf::Color(0,206,209));
		guiBuilder.setBtnAtr(tabName, "hover", gui::BtnAtrChar::transparency, 20);

		trav_x += tabWidth;
		trav_x += tabs[i].offset;

		guiBuilder.addToGroup(groupName, tabName);
	}
	guiBuilder.setGroupAtr(groupName, gui::BtnChar::coords_group, std::make_pair(0, pos_y));
}

void TabBar::update(MouseListener& mouse)
{
	//update if tab is hovered
	isTabBarHovered = false;
	for(Tab& tab : tabs)
	{
		if(tab.menu.isActive())
		{
			isTabBarHovered = true;
			break;
		}
	}
	float mousePos = mouse.getMousePos().y;
	if((dir == utl::Direction::up && mousePos > offset_y && mousePos < (22 + offset_y))
	        ||(dir == utl::Direction::down && mousePos > (1080 - 22 - offset_y) && mousePos < (1080 - offset_y)))
	{
		isTabBarHovered = true;
	}
	if(otherBar != nullptr && otherBar->isHovered())
	{
		isTabBarHovered = false;
		allowHover(false);
	}
	else
	{
		allowHover(true);
	}

	//update acticity of tab
	if(mouse.isMouseReleased(ctr::Input::LMouse) && isTabBarHovered)
	{
		for(Tab& tab : tabs)
		{
			if(guiManager->isHovered(tab.buttonName) && tab.menu.size())
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
	if(isTabBarActive != isActive)
	{
		isTabBarActive = isActive;
		for(Tab& tab : tabs)
		{
			tab.menu.setActivity(false);
		}
		guiManager->edit().setGroupAtr(groupName, gui::BtnChar::isVisible, isActive);
	}
}

void TabBar::addSpace(int tabID, int spacing)
{
	tabs.at(tabID).offset += spacing;
}

void TabBar::addSpace(int spacing)
{
	if(tabs.size())
	{
		tabs.at(tabs.size() - 1).offset += spacing;
	}
}

void TabBar::setBelow(TabBar& other)
{
	otherBar = &other;
}
