#include "QuestiaEng/GuiManager/GuiManager.h"

GuiManager::GuiManager(sf::RenderWindow &_window, ResourceManager &_resourceManager):
	window(_window)
	, resourceManager(_resourceManager)
	, currentButtonEdit("NOBUTTON")
	, currentButtonAtrEdit("NOBUTTONATR")
	, currentGroupEdit("NOGROUP")
	, currentListEdit("NOLIST")
	, mouseCoords(0,0)
{

}

GuiManager::~GuiManager()
{
	for(std::map<std::string, Button*>::iterator it = buttonMap.begin(); it != buttonMap.end(); ++it)
	{
		delete it->second;
	}
}

void GuiManager::purgeButtons()
{
	for(std::map<std::string, Button*>::iterator it = buttonMap.begin(); it != buttonMap.end(); ++it)
	{
		delete it->second;
	}

	buttonMap.clear();
	aliasMap.clear();
	buttonDrawList.clear();
	groupMap.clear();
	groupTemplateMap.clear();
	listMap.clear();
	loadedGuiPacks.clear();
	buttonCount = 0;
}

void GuiManager::createButtonTemplate(std::string buttonName)
{
	if(!buttonMap.count(buttonName))
	{
		Button* newButton = new Button(window, resourceManager, buttonFont, true, -1);
		newButton->buttonName = buttonName;
		buttonMap[buttonName] = newButton;
		buttonMap[buttonName]->setButton(gui::ButtonCharacteristic::isVisible, false);

		currentButtonEdit = buttonName;
	}
}

void GuiManager::createButtonTemplate(std::string buttonName, int layer)
{
	if(!buttonMap.count(buttonName))
	{
		Button* newButton = new Button(window, resourceManager, buttonFont, true, -1);
		newButton->buttonName = buttonName;
		buttonMap[buttonName] = newButton;
		buttonMap[buttonName]->setButton(gui::ButtonCharacteristic::isVisible, false);

		newButton->layer = layer;

		currentButtonEdit = buttonName;
	}
}

void GuiManager::createButtonTemplate(std::string copyName, std::string originalName)
{
	if(!buttonMap.count(copyName))
	{
		if(buttonMap.count(originalName))
		{
			Button* newButton = new Button(*buttonMap[originalName], -1);
			newButton->buttonName = copyName;
			buttonMap[copyName] = newButton;
			buttonMap[copyName]->setButton(gui::ButtonCharacteristic::isVisible, false);
			buttonMap[copyName]->setButton(gui::ButtonCharacteristic::isTemplate, true);

			currentButtonEdit = copyName;
		}
	}
}


void GuiManager::createButton(std::string buttonName)
{
	if(!buttonMap.count(buttonName))
	{
		Button* newButton = new Button(window, resourceManager, buttonFont, false, buttonCount);
		newButton->buttonName = buttonName;
		buttonCount++;
		buttonMap[buttonName] = newButton;

		currentButtonEdit = buttonName;

		placeInDrawList(newButton);
	}
}

void GuiManager::createButton(std::string buttonName, int layer)
{
	if(!buttonMap.count(buttonName))
	{
		Button* newButton = new Button(window, resourceManager, buttonFont, false, buttonCount);
		newButton->buttonName = buttonName;
		buttonCount++;
		buttonMap[buttonName] = newButton;

		currentButtonEdit = buttonName;

		newButton->layer = layer;
		placeInDrawList(newButton);
	}
}

void GuiManager::createButton(std::string copyName, std::string originalName)
{
	if(buttonMap.count(originalName))
	{
		if(!buttonMap.count(copyName))
		{
			Button* newButton = new Button(*buttonMap[originalName], buttonCount);
			newButton->buttonName = copyName;
			buttonCount++;
			buttonMap[copyName] = newButton;

			currentButtonEdit = copyName;

			placeInDrawList(newButton);
		}
	}
}

void GuiManager::placeInDrawList(Button* button)
{
	//check if it exists
	for(auto it = buttonDrawList.begin(); it != buttonDrawList.end(); it++)
	{
		if((*it)->buttonID == button->buttonID)
		{
			buttonDrawList.erase(it);
			break;
		}
	}

	//insert the new button
	if(buttonDrawList.size() == 0)
	{
		buttonDrawList.push_back(button);
		return;
	}
	else
	{
		for(auto it = buttonDrawList.begin(); it != buttonDrawList.end(); it++)
		{
			if(button->layer <= (*it)->layer)
			{
				buttonDrawList.insert(it,button);
				return;
			}
		}
		//in the back
		buttonDrawList.push_back(button);
	}
}

void GuiManager::createButtonAtr(std::string buttonName, std::string atrName, gui::ButtonAtr buttonAtr)
{
	if(buttonMap.count(buttonName))
	{
		buttonMap[buttonName]->addButtonAtr(atrName, buttonAtr);
		currentButtonEdit = buttonName;
		currentButtonAtrEdit = atrName;
	}
}

void GuiManager::createButtonAtr(std::string atrName, gui::ButtonAtr buttonAtr)
{
	buttonMap[currentButtonEdit]->addButtonAtr(atrName, buttonAtr);
	currentButtonAtrEdit = atrName;
}

void GuiManager::createGroup(std::string groupName)
{
	groupMap[groupName];
	currentGroupEdit = groupName;
}

void GuiManager::createGroupTemplate(std::string groupName)
{
	groupTemplateMap[groupName];
	currentGroupEdit = groupName;
}

void GuiManager::createGroupFromTemplate(std::string groupName, std::string templateName)
{
	if(groupTemplateMap.count(templateName))
	{
		groupMap[groupName];
		for(std::string it : groupTemplateMap[templateName])
		{
			Button* newButton = new Button(*buttonMap[it], buttonCount);
			newButton->buttonName = buttonMap[it]->buttonName;
			buttonCount++;
			std::string buttonName = it + "_" + groupName;
			buttonMap[buttonName] = newButton;
			buttonMap[buttonName]->setButton(gui::ButtonCharacteristic::coords, buttonMap[it]->buttonPosition);

			groupMap[groupName].push_back(buttonName);

			currentGroupEdit = groupName;

			placeInDrawList(newButton);
		}
	}
}

std::string GuiManager::getGroupEntry(std::string groupName, std::string buttonName)
{
	if(groupMap.count(groupName))
	{
		for(const std::string& it : groupMap[groupName])
		{
			if(buttonMap[it]->buttonName == buttonName)
			{
				return it;
			}
		}
	}
	return "nil";
}

void GuiManager::addToGroup(std::string groupName, std::string entryName)
{
	if(groupMap.count(groupName))
	{
		if(buttonMap.count(entryName))
		{
			groupMap[groupName].push_back(entryName);
		}
	}
	else if(groupTemplateMap.count(groupName))
	{
		if(buttonMap.count(entryName))
		{
			if(buttonMap[entryName]->isTemplate)
			{
				groupTemplateMap[groupName].push_back(entryName);
			}
		}
	}
}
void GuiManager::addToGroup(std::string entryName)
{
	if(groupMap.count(currentGroupEdit))
	{
		if(buttonMap.count(entryName))
		{
			groupMap[currentGroupEdit].push_back(entryName);
		}
	}
	else if(groupTemplateMap.count(currentGroupEdit))
	{
		if(buttonMap.count(entryName))
		{
			if(buttonMap[entryName]->isTemplate)
			{
				groupTemplateMap[currentGroupEdit].push_back(entryName);
			}
		}
	}
}

bool GuiManager::isClicked(std::string buttonName)
{
	if(buttonMap.count(buttonName))
	{
		if(buttonMap[buttonName]->isVisible && buttonMap[buttonName]->isActive)
		{
			std::pair <int, int> buttonCoords = buttonMap[buttonName]->buttonPosition;
			std::pair <int, int> buttonBounds = buttonMap[buttonName]->buttonBounds;
			buttonCoords.first  += buttonMap[buttonName]->scrollAmount_x;
			buttonCoords.second += buttonMap[buttonName]->scrollAmount_y;

			if(mouseCoords.x  > buttonCoords.first  && mouseCoords.x  < buttonCoords.first  + buttonBounds.first &&
			        mouseCoords.y > buttonCoords.second && mouseCoords.y < buttonCoords.second + buttonBounds.second)
			{
				return true;
			}
		}
	}
	else if(aliasMap.count(buttonName))
	{
		return isClicked(aliasMap[buttonName]);
	}
	return false;
}

void GuiManager::createAlias(std::string alias, std::string buttonName)
{
	aliasMap[alias] = buttonName;
}

void GuiManager::drawButtons()
{
	for(auto it = buttonDrawList.begin(); it != buttonDrawList.end(); it++)
	{
		(*it)->drawButton();
	}
}

void GuiManager::setButtonLayer(std::string buttonName, int layer)
{
	if(buttonMap.count(buttonName))
	{
		Button* button = buttonMap[buttonName];
		button->layer = layer;
		if(button->isTemplate != true)
		{
			placeInDrawList(button);
		}
	}
}
void GuiManager::setButtonLayer(int layer)
{
	Button* button = buttonMap[currentButtonEdit];
	button->layer = layer;
	if(button->isTemplate != true)
	{
		placeInDrawList(button);
	}
}

void GuiManager::deleteButton(std::string buttonName)
{
	delete buttonMap[buttonName];
	buttonMap.erase(buttonName);
}

void GuiManager::setMousePosition(utl::Vector2f _mouseCoords)
{
	mouseCoords = _mouseCoords;

	for(std::map<std::string, Button*>::iterator it = buttonMap.begin(); it != buttonMap.end(); ++it)
	{
		it->second->update(std::make_pair((int)_mouseCoords.x, (int)_mouseCoords.y));
	}
}

void GuiManager::setFont(sf::Font _buttonFont)
{
	buttonFont = _buttonFont;
}







void GuiManager::createList(std::string listName)
{
	listMap[listName] = ListData();
	currentListEdit = listName;
}

void GuiManager::setListSpacing(std::string listName, int spacing_x, int spacing_y)
{
	if(listMap.count(listName))
	{
		listMap[listName].listSpacing = std::make_pair(spacing_x, spacing_y);
	}
}
void GuiManager::setListSpacing(int spacing_x, int spacing_y)
{
	listMap[currentListEdit].listSpacing = std::make_pair(spacing_x, spacing_y);
}
void GuiManager::setListTemplate(std::string listName, std::string groupTemplate)
{
	if(listMap.count(listName))
	{
		if(groupTemplateMap.count(groupTemplate))
		{
			listMap[listName].groupTemplate = groupTemplate;
		}
	}
}
void GuiManager::setListTemplate(std::string groupTemplate)
{
	if(groupTemplateMap.count(groupTemplate))
	{
		listMap[currentListEdit].groupTemplate = groupTemplate;
	}
}

void GuiManager::setListPosition(std::string listName, std::pair<int, int> position)
{
	if(listMap.count(listName))
	{
		listMap[listName].position = position;
	}
}
void GuiManager::setListPosition(std::pair<int, int> position)
{
	listMap[currentListEdit].position = position;
}

std::string GuiManager::createListEntry(std::string listName)
{
	if(listMap[listName].groupTemplate != "NOTEMPLATE")
	{
		int entries = listMap[listName].elementGroups.size();
		std::stringstream ss;
		ss << entries;
		std::string entryName = listName + "_" + ss.str();

		createGroupFromTemplate(entryName, listMap[listName].groupTemplate);
		setGroupAtr(gui::ButtonCharacteristic::coords,
		            std::make_pair(listMap[listName].position.first  + entries * listMap[listName].listSpacing.first,
		                           listMap[listName].position.second + entries * listMap[listName].listSpacing.second));
		listMap[listName].elementGroups.push_back(entryName);

		return entryName;
	}
	return "nil";
}
std::string GuiManager::createListEntry()
{
	if(listMap[currentListEdit].groupTemplate != "NOTEMPLATE")
	{
		int entries = listMap[currentListEdit].elementGroups.size();
		std::stringstream ss;
		ss << entries;
		std::string entryName = currentListEdit + "_" + ss.str();

		createGroupFromTemplate(entryName, listMap[currentListEdit].groupTemplate);
		setGroupAtr(gui::ButtonCharacteristic::coords,
		            std::make_pair(listMap[currentListEdit].position.first  + entries * listMap[currentListEdit].listSpacing.first,
		                           listMap[currentListEdit].position.second + entries * listMap[currentListEdit].listSpacing.second));
		listMap[currentListEdit].elementGroups.push_back(entryName);

		return entryName;
	}
	return "nil";
}