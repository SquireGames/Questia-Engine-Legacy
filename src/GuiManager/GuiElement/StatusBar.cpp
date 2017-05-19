#include "QuestiaEng/GuiManager/GuiElement/StatusBar.h"

StatusBar::StatusBar(utl::Direction direction, int offset):
	dir(direction)
	, offset_y(offset)
{

}

StatusBar::~StatusBar()
{

}

void StatusBar::init(std::string barName, GuiManager& pGuiManager, GuiLoader& guiLoader)
{
	guiManager = &pGuiManager;
	buttonName = barName;

	guiLoader.loadGui(pGuiManager, "statusBar");

	int pos_y = ((dir == utl::Direction::down) ? 1080-22-offset_y : offset_y);

	guiManager->createButton(buttonName, "statusBarTemplate");

	guiManager->setButton(buttonName, gui::ButtonCharacteristic::coords_group, std::make_pair(0, pos_y));

	//tab positioning along x axis
	unsigned int trav_x = 0;
	//make all entries
	for(unsigned int i = 0; i < entries.size(); i++)
	{
		entries[i].key = entries[i].atrName_key;

		if(entries[i].atrName_key == "SPACE")
		{
			trav_x += entries[i].spacing;
			continue;
		}

		std::string atrName_key = buttonName + "k" + std::to_string(i);
		std::string atrName_val = buttonName + "v" + std::to_string(i);

		guiManager->createButtonAtr(buttonName, atrName_key, gui::ButtonAtr::Text);
		guiManager->setButtonAtr(gui::ButtonAtrCharacteristic::charSize, 18);
		guiManager->setButtonAtr(gui::ButtonAtrCharacteristic::color, sf::Color::Black);
		guiManager->setButtonAtr(gui::ButtonAtrCharacteristic::text, entries[i].atrName_key);
		guiManager->setButtonAtr(gui::ButtonAtrCharacteristic::coords, std::make_pair(trav_x, 0));
		int textSpace = sf::Text(entries[i].atrName_key, *guiManager->getFont(), 18).getGlobalBounds().width;
		trav_x += textSpace;
		entries[i].atrName_key = atrName_key;

		guiManager->createButtonAtr(buttonName, atrName_val, gui::ButtonAtr::Text);
		guiManager->setButtonAtr(gui::ButtonAtrCharacteristic::charSize, 18);
		guiManager->setButtonAtr(gui::ButtonAtrCharacteristic::color, sf::Color::Black);
		guiManager->setButtonAtr(gui::ButtonAtrCharacteristic::text, entries[i].atrName_val);
		guiManager->setButtonAtr(gui::ButtonAtrCharacteristic::coords, std::make_pair(trav_x, 0));
		entries[i].atrName_val = atrName_val;

		trav_x += entries[i].spacing;
	}
}

void StatusBar::track(std::string text, int val, int space)
{
	track(text, std::to_string(val), space);
}

void StatusBar::track(std::string text, std::string val, int space)
{
	entries.emplace_back(Entry());
	Entry& e = entries.back();
	e.atrName_key = text;
	e.atrName_val = val;
	e.spacing = space;
}

void StatusBar::addSpace(int space)
{
	entries.emplace_back(Entry());
	Entry& e = entries.back();
	e.atrName_key = "SPACE";
	e.spacing = space;
}

void StatusBar::updateVal(std::string key, int val)
{
	updateVal(key, std::to_string(val));
}

void StatusBar::updateVal(std::string key, std::string val)
{
	for(Entry& entry : entries)
	{
		if(entry.key == key)
		{
			guiManager->setButtonAtr(buttonName, entry.atrName_val, gui::ButtonAtrCharacteristic::text, val);
			return;
		}
	}
}

void StatusBar::setActivity(bool isActive)
{
	if(isActive != isBarActive)
	{
		isBarActive = isActive;
		guiManager->setButton(buttonName, gui::ButtonCharacteristic::isVisible, isActive);
	}
}

void StatusBar::update(MouseListener& mouse)
{
	int mousePos = mouse.getMousePos().y;
	if((dir == utl::Direction::up && mousePos > offset_y && mousePos < (22 + offset_y))
	        ||(dir == utl::Direction::down && mousePos > (1080 - 22 - offset_y) && mousePos < (1080 - offset_y)))
	{
		isBarHovered = true;
	}
}