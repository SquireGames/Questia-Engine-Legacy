#include "QuestiaEng/GuiManager/GuiBuilder.h"

//forward declared from header
#include "QuestiaEng/GuiManager/GuiLoader.h"

GuiBuilder::GuiBuilder(sf::RenderWindow& window, ResourceManager& resourceManager, GuiLoader& guiLoader
                       , std::vector<Button>& buttons, std::vector <Button*>& orderedDrawnButtons
                       , std::unordered_map<std::string, int>& buttonIDs) noexcept:
	window(window)
	, resourceManager(resourceManager)
	, guiLoader(guiLoader)
	, buttons(buttons)
	, orderedDrawnButtons(orderedDrawnButtons)
	, buttonIDs(buttonIDs)
{
}

void GuiBuilder::purgeButtons() noexcept
{
	buttons.clear();
	unusedPos.clear();
	groups.clear();
#ifdef DEBUGMODE
	groupTemplates.clear();
#endif
	lists.clear();
	orderedDrawnButtons.clear();
	visibleButtonIDs.clear();
	guiLoader.loadedGuis.clear();
	buttonIDs.clear();

	buttonCount = 0;
	lastButton = -1;
	lastBtnAtr = std::string();
	lastGroup = std::string();
	lastList = std::string();
}

void GuiBuilder::setFont(sf::Font buttonFont) noexcept
{
	this->buttonFont = buttonFont;
}

sf::Font& GuiBuilder::getFont() noexcept
{
	return buttonFont;
}

void GuiBuilder::setLang(std::string lang) noexcept
{
	this->lang = lang;
	SaveFile langSave("Data/Language Pack/" + lang + "/gui.txt", true);
	if(langSave.readFile('='))
	{
		langPairs = langSave.getSaveList_uni();
	}
}
//TODO Have different context's for different categories, aka gui, items..
std::u32string GuiBuilder::getText(const std::string& key) const noexcept
{
	std::u32string str = utl::toU32(key);
	for(auto& p : langPairs)
	{
		if(p.first == str)
		{
			return p.second;
		}
	}
	return U"nil";
}

bool GuiBuilder::existsButton(const std::string& buttonName) const noexcept
{
#ifdef DEBUGMODE
	if(getPos(buttonName) == -1)
	{
		LOG("<GUI Verify> Button with name: '" + buttonName + "' does not exists");
		return false;
	}
	return true;
#else
	return (getPos(buttonName) != -1);
#endif
}

bool GuiBuilder::existsButtonTemplate(const std::string& buttonName) const noexcept
{
#ifdef DEBUGMODE
	if(!(existsButton(buttonName) && buttons.at(getPos(buttonName)).isTemplate))
	{
		LOG("<GUI Verify> Button Template with name: '" + buttonName + "' does not exists");
		return false;
	}
	return true;
#else
	return (existsButton(buttonName) && buttons.at(getPos(buttonName)).isTemplate);
#endif
}

bool GuiBuilder::existsButtonAtr(const std::string& buttonName, gui::BtnAtr atr, const std::string& atrName) const noexcept
{
	if(!existsButton(buttonName))
	{
		return false;
	}
	Button& btn = buttons.at(getPos(buttonName));
	return btn.count(atr, atrName);
}

bool GuiBuilder::existsGroup(const std::string& groupName) const noexcept
{
	return (std::find_if(groups.begin(), groups.end(), [&groupName](const std::pair<std::string, std::vector<int>>& s) {return s.first == groupName;}) != groups.end());
}

bool GuiBuilder::existsGroupTemplate(const std::string& groupName) const noexcept
{
#ifdef DEBUGMODE
	return (std::find_if(groupTemplates.begin(), groupTemplates.end(), [&groupName](const std::string& s) {return s == groupName;}) != groupTemplates.end());
#endif
	return existsGroup(groupName);
}

bool GuiBuilder::existsInGroup(const std::string& groupName, const std::string& buttonName) const noexcept
{
	if(!existsButton(buttonName))
	{
		return false;
	}
	int buttonID = buttons.at(getPos(buttonName)).buttonID;
	const std::vector<int>& groupIDs = getGroupIDs(groupName);
	return (std::find_if(groupIDs.begin(), groupIDs.end(), [buttonID](int id) {return id == buttonID;}) != groupIDs.end());
}

bool GuiBuilder::existsInGroupTemplate(const std::string& groupName, const std::string& buttonName) const noexcept
{
	return (existsButtonTemplate(buttonName) && existsInGroup(groupName, buttonName));
}

bool GuiBuilder::existsList(const std::string& listName) const noexcept
{
	return (std::find_if(lists.begin(), lists.end(), [&listName](const ListData& l) {return l.listName == listName;}) != lists.end());
}

bool GuiBuilder::existsListGroup(const std::string& listName, const std::string& groupName) const noexcept
{
	return (getList(listName).groupTemplate == groupName);
}

void GuiBuilder::createButton(const std::string& buttonName, int layer) noexcept
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

void GuiBuilder::createButton(const std::string& copyName, const std::string& originalName) noexcept
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

void GuiBuilder::createAlias(const std::string& alias, const std::string& buttonName) noexcept
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

void GuiBuilder::createAlias(const std::string& alias, int buttonID) noexcept
{
#ifdef DEBUGMODE
	if(getPos(buttonID) == -1)
	{
		LOG("Button with ID: '" + std::to_string(buttonID) + "' does not exist");
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
	buttonIDs.emplace(std::make_pair(alias, buttonID));
}

void GuiBuilder::createButtonTemplate(const std::string& buttonName, int layer) noexcept
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

void GuiBuilder::createButtonTemplate(const std::string& copyName, const std::string& originalName) noexcept
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

void GuiBuilder::createBtnAtr(const std::string& atrName, gui::BtnAtr buttonAtr) noexcept
{
	createBtnAtr(lastButton, atrName, buttonAtr);
}

void GuiBuilder::createBtnAtr(const std::string& buttonName, const std::string& atrName, gui::BtnAtr buttonAtr) noexcept
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

void GuiBuilder::createBtnAtr(int buttonID, const std::string& atrName, gui::BtnAtr buttonAtr) noexcept
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

void GuiBuilder::setButtonLayer(int layer) noexcept
{
	setButtonLayer(lastButton, layer);
}

void GuiBuilder::setButtonLayer(const std::string& buttonName, int layer) noexcept
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

void GuiBuilder::setButtonLayer(int buttonID, int layer) noexcept
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
	auto it = std::find_if(orderedDrawnButtons.begin(), orderedDrawnButtons.end(), [&](const Button* b)
	{
		return b->buttonID == buttonID;
	});
	if(it != orderedDrawnButtons.end())
	{
		orderedDrawnButtons.erase(it);
	}
	//re-add it
	placeInDrawList(&btn);
}

void GuiBuilder::createGroup(const std::string& groupName) noexcept
{
#ifdef DEBUGMODE
	if(existsGroup(groupName))
	{
		LOG("Group with name: '" + groupName + "' already exists");
		return;
	}
#endif
	groups.push_back(std::make_pair(groupName, std::vector<int>()));
	lastGroup = groupName;
}

void GuiBuilder::createGroupTemplate(const std::string& groupName) noexcept
{
#ifdef DEBUGMODE
	if(existsGroupTemplate(groupName))
	{
		LOG("Group Template with name: '" + groupName + "' already exists");
		return;
	}
	if(existsGroup(groupName))
	{
		LOG("Group with name: '" + groupName + "' already exists");
		return;
	}
	groupTemplates.push_back(groupName);
#endif
	createGroup(groupName);
}

void GuiBuilder::createGroupFromTemplate(const std::string& groupName, const std::string& templateName) noexcept
{
#ifdef DEBUGMODE
	if(!existsGroupTemplate(templateName))
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
		std::pair<int, int> pos = btnTemplate.buttonPosition;
		std::string btnName =  groupName + "__" + std::to_string(btnTemplate.buttonID);
		Button& newBtn = copyButton(btnTemplate, btnName, btnTemplate.buttonID);
		newBtn.setButton(gui::BtnChar::coords, pos);

		groupIDs.push_back(newBtn.buttonID);

		placeInDrawList(&newBtn);
	}
	lastGroup = groupName;
}

void GuiBuilder::addToGroup(const std::string& entryName) noexcept
{
	addToGroup(lastGroup, entryName);
}

void GuiBuilder::addToGroup(const std::string& groupName, const std::string& entryName) noexcept
{
#ifdef DEBUGMODE
	if(getPos(entryName) == -1)
	{
		LOG("Button with name: '" + entryName + "' does not exist");
		return;
	}
	if(!existsGroup(groupName))
	{
		LOG("Group with name: '" + groupName + "' does not exist");
		return;
	}
	if(existsGroupTemplate(groupName) && !(buttons.at(getPos(entryName)).isTemplate))
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

std::string GuiBuilder::getGroupEntry(const std::string& groupName, const std::string& buttonName) const noexcept
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
int GuiBuilder::getGroupEntryID(const std::string& groupName, const std::string& buttonName) const noexcept
{
#ifdef DEBUGMODE
	if(getPos(buttonName) == -1)
	{
		LOG("Button with name: '" + buttonName + "' does not exist");
		return -1;
	}
	if(!existsGroup(groupName))
	{
		LOG("Group with name: '" + groupName + "' does not exist");
		return -1;
	}
	if(existsGroupTemplate(groupName))
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
	const std::vector<int>& ids = getGroupIDs(groupName);
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
	const std::vector<int>& groupIDs = getGroupIDs(groupName);
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

void GuiBuilder::createList(const std::string& listName) noexcept
{
#ifdef DEBUGMODE
	if(existsList(listName))
	{
		LOG("List with name: '" + listName + "' already exists");
		return;
	}
#endif
	lists.emplace_back(ListData(listName));
	lastList = listName;
}

void GuiBuilder::setListTemplate(const std::string& groupTemplate) noexcept
{
	setListTemplate(lastList, groupTemplate);
}

void GuiBuilder::setListTemplate(const std::string& listName, const std::string& groupTemplate) noexcept
{
#ifdef DEBUGMODE
	if(!existsList(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return;
	}
	if(!existsGroup(groupTemplate))
	{
		LOG("Group with name: '" + groupTemplate + "' does not exist");
		return;
	}
	if(!existsGroupTemplate(groupTemplate))
	{
		LOG("Group Template with name: '" + groupTemplate + "' does not exist");
		return;
	}
#endif
	getList(listName).groupTemplate = groupTemplate;
}

void GuiBuilder::setListSpacing(int spacing_x, int spacing_y) noexcept
{
	setListSpacing(lastList, spacing_x, spacing_y);
}

void GuiBuilder::setListSpacing(const std::string& listName, int spacing_x, int spacing_y) noexcept
{
#ifdef DEBUGMODE
	if(!existsList(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return;
	}
#endif
	getList(listName).listSpacing = std::make_pair(spacing_x, spacing_y);
}

void GuiBuilder::setListPosition(std::pair<int, int> position) noexcept
{
	setListPosition(lastList, position);
}

void GuiBuilder::setListPosition(const std::string& listName, std::pair<int, int> position) noexcept
{
#ifdef DEBUGMODE
	if(!existsList(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return;
	}
#endif
	getList(listName).position = position;
}

std::string GuiBuilder::createListEntry() noexcept
{
	return createListEntry(lastList);
}

std::string GuiBuilder::createListEntry(const std::string& listName) noexcept
{
#ifdef DEBUGMODE
	if(!existsList(listName))
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
	if(!existsGroupTemplate(l.groupTemplate))
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

void GuiBuilder::deleteList(const std::string& listName) noexcept
{
#ifdef DEBUGMODE
	if(!existsList(listName))
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

void GuiBuilder::deleteGroup(const std::string& groupName) noexcept
{
#ifdef DEBUGMODE
	if(!existsGroup(groupName))
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

void GuiBuilder::deleteButton(const std::string& buttonName) noexcept
{
	deleteButton(buttonIDs.at(buttonName));
}

void GuiBuilder::deleteButton(int buttonID) noexcept
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

int GuiBuilder::getNewID(const std::string& buttonName) noexcept
{
	int newButtonID = buttonCount;
	buttonCount++;
	buttonIDs.emplace(std::make_pair(buttonName, newButtonID));
	return newButtonID;
}
Button& GuiBuilder::emplaceButton(bool isTemplate, const std::string& buttonName, int templateID) noexcept
{
	int buttonID = getNewID(buttonName);
	if(unusedPos.size())
	{
		int pos = unusedPos.back();
		unusedPos.pop_back();
		buttons.at(pos) = Button(window, resourceManager, buttonFont, isTemplate, buttonID, templateID);
		return buttons.at(pos);
	}
	buttons.emplace_back(Button(window, resourceManager, buttonFont, isTemplate, buttonID, templateID));
	refreshDrawVector();
	return buttons.back();
}
Button& GuiBuilder::copyButton(Button& old, const std::string& buttonName, int templateID) noexcept
{
	int buttonID = getNewID(buttonName);
	int oldButtonID = old.buttonID;
	if(buttons.capacity() < (buttons.size() + 2))
	{
		buttons.reserve(buttons.size() + 20);
	}
	Button& realOld = buttons.at(getPos(oldButtonID));

	if(unusedPos.size())
	{
		int pos = unusedPos.back();
		unusedPos.pop_back();
		buttons.at(pos) = Button(realOld, buttonID, templateID);
		return buttons.at(pos);
	}
	buttons.emplace_back(Button(realOld, buttonID, templateID));
	refreshDrawVector();
	return buttons.back();
}

int GuiBuilder::getPos(int buttonID) const noexcept
{
	for(unsigned int it = 0; it < buttons.size(); it++)
	{
		if(buttonID == buttons.at(it).buttonID)
		{
			return (int)it;
		}
	}
	return -1;
}

const std::vector<int>& GuiBuilder::getGroupIDs(const std::string& groupName) const noexcept
{
#ifdef DEBUGMODE
	if(!existsGroup(groupName))
	{
		LOG("Group with name: '" + groupName + "' does not exist");
		return groups.back().second;
	}
#endif
	return (*std::find_if(groups.begin(), groups.end(), [&groupName](const std::pair<std::string, std::vector<int>>& s) {return s.first == groupName;})).second;
}
std::vector<int>& GuiBuilder::getGroupIDs(const std::string& groupName) noexcept
{
	const GuiBuilder* const constThis = this;
	return const_cast<std::vector<int>&>(constThis->getGroupIDs(groupName));
}

const GuiBuilder::ListData& GuiBuilder::getList(const std::string& listName) const noexcept
{
#ifdef DEBUGMODE
	if(!existsList(listName))
	{
		LOG("List with name: '" + listName + "' does not exist");
		return lists.back();
	}
#endif
	for(auto& it : lists)
	{
		if(it.listName == listName)
		{
			return it;
		}
	}
	//should never reach here
	return lists.back();
}
GuiBuilder::ListData& GuiBuilder::getList(const std::string& listName) noexcept
{
	const GuiBuilder* const constThis = this;
	return const_cast<GuiBuilder::ListData&>(constThis->getList(listName));
}

void GuiBuilder::placeInDrawList(Button* button) noexcept
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

void GuiBuilder::refreshDrawVector() noexcept
{
	orderedDrawnButtons.clear();
	for(int id : visibleButtonIDs)
	{
		orderedDrawnButtons.push_back(&buttons.at(getPos(id)));
	}
}
