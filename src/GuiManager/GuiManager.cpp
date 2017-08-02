#include "QuestiaEng/GuiManager/GuiManager.h"

GuiManager::GuiManager(sf::RenderWindow& window, ResourceManager& resourceManager) noexcept:
	window(window)
	, resourceManager(resourceManager)
	, mouseCoords(0,0)
	, guiBuilder(window, resourceManager, buttons, orderedDrawnButtons, buttonIDs)
{
	buttons.reserve(60);
	orderedDrawnButtons.reserve(50);
	buttonIDs.clear();
}

GuiManager::~GuiManager() noexcept
{

}

GuiBuilder& GuiManager::edit() noexcept 
{
	return guiBuilder;
}

void GuiManager::setMousePosition(utl::Vector2f mouseCoords) noexcept
{
	for(auto& it : buttons)
	{
		if(it.isActive)
		{
			it.update(std::make_pair((int)mouseCoords.x, (int)mouseCoords.y));
		}
	}
	this->mouseCoords = std::move(mouseCoords);
}

bool GuiManager::isHovered(int buttonID) noexcept
{
#ifdef DEBUGMODE
	if(getButton(buttonID) == nullptr)
	{
		LOG("Button with ID: " + std::to_string(buttonID) + " does not exist");
		return false;
	}
#endif
	return getButton(buttonID)->isHovered;
}

void GuiManager::draw() noexcept
{
	for(unsigned int i = 0; i < orderedDrawnButtons.size(); i++)
	{
		orderedDrawnButtons.at(i)->draw();
	}
}

Button* GuiManager::getButton(const int buttonID)
{
	for(unsigned int it = 0; it < buttons.size(); it++)
	{
		if(buttonID == buttons.at(it).buttonID)
		{
			return &buttons.at(it);
		}
	}
	return nullptr;
}

