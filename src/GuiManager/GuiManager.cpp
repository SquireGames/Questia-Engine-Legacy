#include "QuestiaEng/GuiManager/GuiManager.h"

//TODO reorder
//TODO check and give warnings if last~ is default
//TODO give more descriptive LOG's

GuiManager::GuiManager(sf::RenderWindow& window, ResourceManager& resourceManager):
	window(window)
	, resourceManager(resourceManager)
	, mouseCoords(0,0)
{
	buttons.reserve(30);
	visibleButtons.reserve(20);
	buttonIDs.clear();
}

GuiManager::~GuiManager()
{

}

void GuiManager::createButton(const std::string& buttonName)
{
	createButton(buttonName, 1);
}

void GuiManager::createButton(const std::string& buttonName, int layer)
{
#ifdef DEBUGMODE
	if(getPos(buttonName) != -1)
	{
		LOG("Button with name: '" + buttonName + "' already exists");
		return;
	}
#endif
	Button& btn = emplaceButton(false, buttonName);
	btn.layer = layer;
	lastButton = btn.buttonID;
	
	placeInDrawList(&btn);
}

void GuiManager::createButton(const std::string& copyName, const std::string& originalName)
{
#ifdef DEBUGMODE
	if(getPos(originalName) == -1)
	{
		LOG("Button with name: '" + copyName + "' already exists");
		return;
	}
	if(getPos(copyName) != -1)
	{
		LOG("Button with name: '" + originalName + "' does not exist");
		return;
	}
#endif
	Button& btn = copyButton(buttons.at(getPos(originalName)), copyName);
	
	placeInDrawList(&btn);
	lastButton = btn.buttonID;
}

void GuiManager::createAlias(const std::string& alias, const std::string& buttonName)
{
#ifdef DEBUGMODE
	if(getPos(buttonName) == -1)
	{
		LOG("Button with name: '" + buttonName + "' does not exist");
		return;
	}
	if(getPos(alias) != -1)
	{
		LOG("Button with name: '" + alias + "' already exists, failed to make alias");
		return;
	}
	if(buttonIDs.count(alias))
	{
		LOG("Alias with name: '" + alias + "' already exists");
		return;
	}
#endif
	int buttonID = buttons.at(getPos(buttonName)).buttonID;
	buttonIDs.emplace(std::make_pair(alias, buttonID));
}


void GuiManager::createButtonTemplate(const std::string& buttonName)
{
	createButtonTemplate(buttonName, 1);
}

void GuiManager::createButtonTemplate(const std::string& buttonName, int layer)
{
#ifdef DEBUGMODE
	if(getPos(buttonName) != -1)
	{
		LOG("Button with name: '" + buttonName + "' already exists");
		return;
	}
#endif
	Button& btn = emplaceButton(true, buttonName);
	btn.layer = layer;
	lastButton = btn.buttonID;
}

void GuiManager::createButtonTemplate(const std::string& copyName, const std::string& originalName)
{
#ifdef DEBUGMODE
	if(getPos(originalName) == -1)
	{
		LOG("Button with name: '" + originalName + "' does not exist");
		return;
	}
	if(getPos(copyName) != -1)
	{
		LOG("Button with name: '" + copyName + "' already exists");
		return;
	}
#endif
	Button& newBtn = copyButton(buttons.at(getPos(originalName)), copyName);
	newBtn.setButton(gui::BtnChar::isVisible, false);
	newBtn.setButton(gui::BtnChar::isTemplate, true);
	lastButton = newBtn.buttonID;
}

void GuiManager::createBtnAtr(const std::string& atrName, gui::BtnAtr buttonAtr)
{
	createBtnAtr(lastButton, atrName, buttonAtr);
}

void GuiManager::createBtnAtr(const std::string& buttonName, const std::string& atrName, gui::BtnAtr buttonAtr)
{
#ifdef DEBUGMODE
	if(getPos(buttonName) == -1)
	{
		LOG("Button with name: '" + buttonName + "' does not exist");
		return;
	}
#endif
	createBtnAtr(buttonIDs.at(buttonName), atrName, buttonAtr);
}

void GuiManager::createBtnAtr(int buttonID, const std::string& atrName, gui::BtnAtr buttonAtr)
{
#ifdef DEBUGMODE
	if(getPos(buttonID) == -1)
	{
		LOG("Button with ID: " + std::to_string(buttonID) + " does not exist");
		return;
	}
#endif
	Button& btn = buttons.at(getPos(buttonID));
	btn.addBtnAtr(atrName, buttonAtr);
	lastButton = buttonID;
	lastBtnAtr = atrName;
}

void GuiManager::setButtonLayer(int layer)
{
	setButtonLayer(lastButton, layer);
}

void GuiManager::setButtonLayer(const std::string& buttonName, int layer)
{
#ifdef DEBUGMODE
	if(getPos(buttonName) == -1)
	{
		LOG("Button with name: '" + buttonName + "' does not exist");
		return;
	}
#endif
	setButtonLayer(buttonIDs.at(buttonName), layer);
}

void GuiManager::setButtonLayer(int buttonID, int layer)
{
#ifdef DEBUGMODE
	if(getPos(buttonID) == -1)
	{
		LOG("Button with ID: " + std::to_string(buttonID) + " does not exist");
		return;
	}
#endif
	Button& btn = buttons.at(getPos(buttonID));
	btn.layer = layer;
	if(btn.isTemplate)
	{
		return;
	}
	//remove if it exists in the vector
	auto it = std::find_if(visibleButtons.begin(), visibleButtons.end(), [&](const Button* b)
	{
		return b->buttonID == buttonID;
	});
	if(it != visibleButtons.end())
	{
		visibleButtons.erase(it);
	}
	//re-add it
	placeInDrawList(&btn);
}

void GuiManager::createGroup(const std::string& groupName)
{
#ifdef DEBUGMODE
	if(groupExists(groupName))
	{
		LOG("Group with name: '" + groupName + "' already exists");
		return;
	}
#endif
	groups.push_back(std::make_pair(groupName, std::vector<int>()));
	lastGroup = groupName;
}

void GuiManager::createGroupTemplate(const std::string& groupName)
{
#ifdef DEBUGMODE
	if(groupTemplateExists(groupName))
	{
		LOG("Group Template with name: '" + groupName + "' already exists");
		return;
	}
	if(groupExists(groupName))
	{
		LOG("Group with name: '" + groupName + "' already exists");
		return;
	}
	groupTemplates.push_back(groupName);
#endif
	createGroup(groupName);
}

void GuiManager::createGroupFromTemplate(const std::string& groupName, const std::string& templateName)
{
#ifdef DEBUGMODE
	if(!groupTemplateExists(templateName))
	{
		LOG("Group Template with name: '" + templateName + "' does not exist");
		return;
	}
#endif
	createGroup(groupName);
	std::vector<int>& templateIDs = getGroupIDs(templateName);
	std::vector<int>& groupIDs = getGroupIDs(groupName);
	for(int it : templateIDs)
	{
		Button& btnTemplate = buttons.at(getPos(it));
		std::string btnName =  groupName + "__" + std::to_string(btnTemplate.buttonID);
		Button& newBtn = copyButton(btnTemplate, btnName, btnTemplate.buttonID);
		newBtn.setButton(gui::BtnChar::coords, btnTemplate.buttonPosition);
		
		groupIDs.push_back(newBtn.buttonID);
		
		placeInDrawList(&newBtn);
	}
	lastGroup = groupName;
}

void GuiManager::addToGroup(const std::string& entryName)
{
	addToGroup(lastGroup, entryName);
}

void GuiManager::addToGroup(const std::string& groupName, const std::string& entryName)
{
#ifdef DEBUGMODE
	if(getPos(entryName) == -1)
	{
		LOG("Button with name: '" + entryName + "' does not exist");
		return;
	}
	if(!groupExists(groupName))
	{
		LOG("Group with name: '" + groupName + "' does not exist");
		return;
	}
	if(groupTemplateExists(groupName) && !(buttons.at(getPos(entryName)).isTemplate))
	{
		LOG("Button with name: '" + entryName + "' is not a template, and therefore can't be added to a group template");
		return;
	}
	std::vector<int>& ids = getGroupIDs(groupName);
	int buttonID = buttons.at(getPos(entryName)).buttonID;
	for(int id : ids)
	{
		if(buttonID == id)
		{
			//TODO print warning
			return;
		}
	}
#endif
	getGroupIDs(groupName).push_back(buttons.at(getPos(entryName)).buttonID);
}

std::string GuiManager::getGroupEntry(const std::string& groupName, const std::string& buttonName)
{
	//get button ID
	int realButtonID = getGroupEntryID(groupName, buttonName);

	//get the buttonName from the ID
	for(const auto& it : buttonIDs)
	{
		const std::string& buttonName = it.first;
		int buttonID = it.second;

		if(buttonID == realButtonID)
		{
			return buttonName;
		}
	}
	return std::string();
}
int GuiManager::getGroupEntryID(const std::string& groupName, const std::string& buttonName)
{
#ifdef DEBUGMODE
	if(getPos(buttonName) == -1)
	{
		LOG("Button with name: '" + buttonName + "' does not exist");
		return -1;
	}
	if(!groupExists(groupName))
	{
		LOG("Group with name: '" + groupName + "' does not exist");
		return -1;
	}
	if(groupTemplateExists(groupName))
	{
		LOG("Group with name: '" + groupName + "' is a template, and therefore can't have real entries");
		return -1;
	}
	if(!buttons.at(getPos(buttonName)).isTemplate)
	{
		LOG("Button with name: '" + buttonName + "' is not a template, and therefore cannot exist in a template group");
		return -1;
	}
	//checks if template exists in group
	std::vector<int>& ids = getGroupIDs(groupName);
	int templateID = buttons.at(getPos(buttonName)).buttonID;
	bool templateExists = false;
	for(int id : ids)
	{
		if(templateID == buttons.at(getPos(id)).buttonGroupID)
		{
			templateExists = true;
		}
	}
	if(!templateExists)
	{
		LOG("Button with name: '" + buttonName + "' does not exist inside of " + groupName);
		return -1;
	}
#endif
	//ID of the given template
	int buttonTemplateID = buttons.at(getPos(buttonName)).buttonID;

	//find the real button ID
	std::vector<int>& groupIDs = getGroupIDs(groupName);
	for(int id : groupIDs)
	{
		Button& btn = buttons.at(getPos(id));
		if(btn.buttonGroupID == buttonTemplateID)
		{
			return btn.buttonID;
		}
	}
	//should never reach here
	return -1;
}

void GuiManager::createList(const std::string& listName)
{
#ifdef DEBUGMODE
	if(listExists(listName))
	{
		LOG("List with name: '" + listName + "' already exists");
		return;
	}
#endif
	lists.emplace_back(ListData(listName));
	lastList = listName;
}

void GuiManager::setListTemplate(const std::string& groupTemplate)
{
	setListTemplate(lastList, groupTemplate);
}

void GuiManager::setListTemplate(const std::string& listName, const std::string& groupTemplate)
{
#ifdef DEBUGMODE
	if(!listExists(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return;
	}
	if(!groupExists(groupTemplate))
	{
		LOG("Group with name: '" + groupTemplate + "' does not exist");
		return;
	}
	if(!groupTemplateExists(groupTemplate))
	{
		LOG("Group Template with name: '" + groupTemplate + "' does not exist");
		return;
	}
#endif
	getList(listName).groupTemplate = groupTemplate;
}

void GuiManager::setListSpacing(int spacing_x, int spacing_y)
{
	setListSpacing(lastList, spacing_x, spacing_y);
}

void GuiManager::setListSpacing(const std::string& listName, int spacing_x, int spacing_y)
{
#ifdef DEBUGMODE
	if(!listExists(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return;
	}
#endif
	getList(listName).listSpacing = std::make_pair(spacing_x, spacing_y);
}

void GuiManager::setListPosition(std::pair<int, int> position)
{
	setListPosition(lastList, position);
}

void GuiManager::setListPosition(const std::string& listName, std::pair<int, int> position)
{
#ifdef DEBUGMODE
	if(!listExists(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return;
	}
#endif
	getList(listName).position = position;
}

std::string GuiManager::createListEntry()
{
	return createListEntry(lastList);
}

std::string GuiManager::createListEntry(const std::string& listName)
{
#ifdef DEBUGMODE
	if(!listExists(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return std::string();
	}
	ListData& l = getList(listName);
	if(l.groupTemplate.size() == 0)
	{
		LOG("List with name: '" + listName + "' does not have a set Group Template");
		return std::string();
	}
	if(!groupTemplateExists(l.groupTemplate))
	{
		LOG("List with name: '" + listName + "' does not have a valid Group Template (" + l.groupTemplate + ")");
		return std::string();
	}
#endif
	ListData& list = getList(listName);
	int entries = list.groups.size();
	std::string entryName = listName + "_" + std::to_string(entries);
	list.groups.push_back(entryName);

	createGroupFromTemplate(entryName, list.groupTemplate);
	setGroupAtr(gui::BtnChar::coords,
	            std::make_pair(list.position.first  + entries * list.listSpacing.first,
	                           list.position.second + entries * list.listSpacing.second));
	return entryName;
}

void GuiManager::purgeButtons()
{
	buttons.clear();
	unusedPos.clear();
	groups.clear();
#ifdef DEBUGMODE
	groupTemplates.clear();
#endif
	lists.clear();
	visibleButtons.clear();
	visibleButtonIDs.clear();
	loadedGuiPacks.clear();
	buttonIDs.clear();
	
	buttonCount = 0;
	lastButton = -1;
	lastBtnAtr = std::string();
	lastGroup = std::string();
	lastList = std::string();
}

void GuiManager::deleteList(const std::string& listName)
{
#ifdef DEBUGMODE
	if(!listExists(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return;
	}
#endif
	//delete lists
	ListData& list = getList(listName);
	for(const std::string& group : list.groups)
	{
		deleteGroup(group);
	}
	//erase from list vector
	for(auto it = lists.begin(); it != lists.end(); it++)
	{
		if((*it).listName == listName)
		{
			lists.erase(it);
			break;
		}
	}
}

void GuiManager::deleteGroup(const std::string& groupName)
{
#ifdef DEBUGMODE
	if(!groupExists(groupName))
	{
		LOG("Group with name: '" + groupName + "' does not exist");
		return;
	}
#endif
	//delete buttons
	std::vector<int>& buttonIDs = getGroupIDs(groupName);
	for(int buttonID : buttonIDs)
	{
		deleteButton(buttonID);
	}
	//erase from group vector
	for(auto it = groups.begin(); it != groups.end(); it++)
	{
		if((*it).first == groupName)
		{
			groups.erase(it);
			break;
		}
	}
}

void GuiManager::deleteButton(const std::string& buttonName)
{
	deleteButton(buttonIDs.at(buttonName));
}

void GuiManager::deleteButton(int buttonID)
{
#ifdef DEBUGMODE
	if(getPos(buttonID) == -1)
	{
		LOG("Button with ID: " + std::to_string(buttonID) + " does not exist");
		return;
	}
#endif
	Button& btn = buttons.at(getPos(buttonID));

	//make the button inactive
	btn.isActive = false;
	btn.isVisible = false;

	//erase from draw vector
	bool refreshDraw = false;
	for(auto it = visibleButtonIDs.begin(); it != visibleButtonIDs.end(); it++)
	{
		if((*it) == buttonID)
		{
			visibleButtonIDs.erase(it);
			refreshDraw = true;
			break;
		}
	}
	if(refreshDraw)
	{
		refreshDrawVector();
	}

	//free up space in buttons vector
	for(unsigned int it = 0; it < buttons.size(); it++)
	{
		if(buttons.at(it).buttonID == buttonID)
		{
			unusedPos.push_back(it);
			break;
		}
	}

	//remove all aliases to buttonID
	auto it = buttonIDs.begin();
	while(it != buttonIDs.end())
	{
		if(it->second == buttonID)
		{
			it = buttonIDs.erase(it);
		}
		else
		{
			it++;
		}
	}
}

bool GuiManager::isClicked(const std::string& buttonName)
{
#ifdef DEBUGMODE
	if(getPos(buttonName) == -1)
	{
		LOG("Button with name: '" + buttonName + "' does not exist");
		return false;
	}
#endif
	return buttons.at(getPos(buttonName)).isHovered;
}
bool GuiManager::isClicked(int buttonID)
{
#ifdef DEBUGMODE
	if(getPos(buttonID) == -1)
	{
		LOG("Button with ID: " + std::to_string(buttonID) + " does not exist");
		return false;
	}
#endif
	return buttons.at(getPos(buttonID)).isHovered;
}

void GuiManager::draw()
{
	for(unsigned int i = 0; i < visibleButtons.size(); i++)
	{
		visibleButtons.at(i)->draw();
	}
}

void GuiManager::placeInDrawList(Button* button)
{
	int layer = button->layer;
	std::vector<int>::iterator it = visibleButtonIDs.begin();
	bool inserted = false;
	for(; it != visibleButtonIDs.end(); it++)
	{
		if(layer <= buttons.at(getPos(*it)).layer)
		{
			visibleButtonIDs.insert(it, button->buttonID);
			inserted = true;
			break;
		}
	}
	if(!inserted)
	{
		visibleButtonIDs.push_back(button->buttonID);
	}
	refreshDrawVector();
}

void GuiManager::refreshDrawVector()
{
	visibleButtons.clear();
	for(int id : visibleButtonIDs)
	{
		visibleButtons.push_back(&buttons.at(getPos(id)));
	}
}

void GuiManager::setMousePosition(utl::Vector2f mouseCoords)
{
	this->mouseCoords = mouseCoords;

	for(auto& it : buttons)
	{
		if(it.isActive)
		{
			it.update(std::make_pair((int)mouseCoords.x, (int)mouseCoords.y));
		}
	}
}

void GuiManager::setFont(sf::Font buttonFont)
{
	this->buttonFont = buttonFont;
}

bool GuiManager::isLoadedGuiPack(const std::string& guiPack)
{
	if(std::find(loadedGuiPacks.begin(), loadedGuiPacks.end(), guiPack) == loadedGuiPacks.end())
	{
		loadedGuiPacks.push_back(guiPack);
		return false;
	}
	return true;
}
