#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <type_traits>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Text.hpp"

#include "QuestiaEng/Utl/Logger.h"

#include "QuestiaEng/ResourceManager/ResourceManager.h"
#include "QuestiaEng/GuiManager/Button.h"
#include "QuestiaEng/Utl/Type/Vector2.h"

#include "QuestiaEng/Utl/Savefile.h"

#include "QuestiaEng/GuiManager/GuiBuilder.h"

class GuiManager
{
public:
	GuiManager(sf::RenderWindow& window, ResourceManager& resourceManager) noexcept;
	GuiManager(GuiManager&&) noexcept = default;
	GuiManager& operator= (GuiManager&&) noexcept = default;
	~GuiManager() noexcept;

	GuiManager(const GuiManager&) = delete;
	GuiManager& operator= (const GuiManager&) = delete;
	
	GuiBuilder& edit() noexcept;
	
	template<class String, typename = typename std::enable_if<std::is_constructible<std::string, String>::value>::type>
	bool isHovered(String&& buttonName) noexcept;
	bool isHovered(int buttonID) noexcept;
	void draw() noexcept;
	
	///to be called by Engine every tick (not necessary for states to call in update function):
	void setMousePosition(utl::Vector2f mouseCoords) noexcept;

private:
	//returns the button with the given ID, or nullptr if it does not exist
	Button* getButton(int buttonID);
	//returns the button with the given name, or nullptr if it does not exist
	template<class String, typename = typename std::enable_if<std::is_constructible<std::string, String>::value>::type>
	Button* getButton(String&& name);

	sf::RenderWindow& window;
	ResourceManager& resourceManager;

	utl::Vector2f mouseCoords;

	std::vector<Button> buttons;
	std::vector<Button*> orderedDrawnButtons;
	//used to hash a button name to a buttonID
	std::unordered_map<std::string, int> buttonIDs;

	GuiBuilder guiBuilder;
};

template<class String, typename = typename std::enable_if<std::is_constructible<std::string, String>::value>::type>
inline bool GuiManager::isHovered(String&& buttonName) noexcept
{
	static_assert(std::is_constructible<std::string, String>::value, "Parameter buttonName can not construct a std::basic_string<char>");
#ifdef DEBUGMODE
	if(getButton(buttonName) == nullptr)
	{
		LOG("Button with name: '" + buttonName + "' does not exist");
		return false;
	}
#endif
	return getButton(std::forward<String>(buttonName))->isHovered;
}

template<class String, typename = typename std::enable_if<std::is_constructible<std::string, String>::value>::type>
inline Button* GuiManager::getButton(String&& name)
{
	static_assert(std::is_constructible<std::string, String>::value, "Parameter buttonName can not construct a std::basic_string<char>");
#ifdef DEBUGMODE
	if(!buttonIDs.count(name))
	{
		return -1;
	}
#endif
	return(getButton(buttonIDs.at(std::forward<String>(name))));
}

#endif // GUIMANAGER_H
