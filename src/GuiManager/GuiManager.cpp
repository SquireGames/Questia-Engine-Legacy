#include "QuestiaEng/GuiManager/GuiManager.h"

GuiManager::GuiManager(sf::RenderWindow& window, ResourceManager& resourceManager) noexcept:
	window(window)
	, resourceManager(resourceManager)
	, mouseCoords(0,0)
	, guiLoader(*this)
	, guiBuilder(window, resourceManager, guiLoader, buttons, orderedDrawnButtons, buttonIDs)
{
	buttons.reserve(60);
	orderedDrawnButtons.reserve(50);
	buttonIDs.clear();
}

GuiBuilder& GuiManager::edit() noexcept
{
	return guiBuilder;
}

void GuiManager::setGuiPack(const std::string& guiPack)
{
	guiLoader.setGuiPack(guiPack);
}

void GuiManager::loadGui(const std::string& gui)
{
	guiLoader.loadGui(gui);
}

bool GuiManager::isHovered(int buttonID) const noexcept
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

const Button* GuiManager::getButton(int buttonID) const noexcept
{
	auto it = std::find_if(buttons.begin(), buttons.end(), [buttonID](const Button& btn) {return buttonID == btn.buttonID;});
	return (it == buttons.end()) ? nullptr : &*it;
}
