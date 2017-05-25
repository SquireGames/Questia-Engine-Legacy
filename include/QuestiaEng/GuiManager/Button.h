#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

#include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include "QuestiaEng/Utl/Utl.h"

#include "QuestiaEng/StateManager/State.h"
#include "QuestiaEng/ResourceManager/ResourceManager.h"

namespace gui
{

enum BtnAtr
{
    Sprite,
    Percent,
    Hover,
    Text
};
enum class BtnChar
{
    coords,
    bounds,
    scroll_x, scroll_y,
    addToScroll_x, addToScroll_y,
    isVisible,
    isActive,
    isTemplate,
    coords_group
};
enum class BtnAtrChar
{
    sprite,
    texture,
    flip,
    coords,
    color,
    charSize,
    text,
    transparency,
    size,
    percentage,
    direction
};

};

class Button
{
public:
	//ctor + dtor
	Button(sf::RenderWindow& _window, ResourceManager &_resourceManager, sf::Font& _buttonFont, bool _isTemplate, int _buttonID);
	Button(const Button& oldButton, int _buttonID);
	~Button();
	
	void addBtnAtr(std::string atrName, gui::BtnAtr buttonAtr);

	void setButton(gui::BtnChar buttonChar, std::string value);
	void setButton(gui::BtnChar buttonChar, const char* value);
	void setButton(gui::BtnChar buttonChar, bool value);
	void setButton(gui::BtnChar buttonChar, int value);
	void setButton(gui::BtnChar buttonChar, std::pair <int, int> value);

	void setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, std::string value);
	void setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, std::pair<int, int> value);
	void setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, sf::Color color);
	void setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, int value);
	void setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, char value);

	void update(std::pair <int, int> mouseCoords);
	void draw();

	//TODO REMOVE
	void copyToThisButton(Button& newButton, const Button& oldButton);

	//refs
	sf::RenderWindow& window;
	ResourceManager& resourceManager;
	sf::Font& buttonFont;

	//vars
	std::string buttonName = "none";
	std::pair <int, int> buttonPosition = std::make_pair(0,0);
	std::pair <int, int> buttonBounds;
	int buttonID;
	int layer = 1;
	int scroll_x = 0;
	int scroll_y = 0;
	bool isTemplate;
	bool isVisible;
	bool isCoordsChanged = true;
	bool isActive = true;

	//atrs
	struct RegularSprite
	{
		RegularSprite(std::string name):atrName(name) {}
		RegularSprite(const RegularSprite& old):atrName(old.atrName), sprite(old.sprite), position(old.position) {}
		std::string atrName;
		sf::Sprite sprite;
		std::pair <int, int> position = std::make_pair(0,0);
		bool isChanged = true;
	};
	struct ButtonText
	{
		std::string atrName;
		sf::Text text;
		std::pair <int, int> position;
		bool isChanged;
	};
	struct OverlaySprite
	{
		std::string atrName;
		sf::RectangleShape rectOverlay;
		std::pair <int, int> position;
		bool isChanged;
		bool isHoveredOver;
	};
	struct PercentSprite
	{
		std::string atrName;
		sf::Sprite sprite;
		sf::RectangleShape rectOverlay;
		float spritePercentage;
		utl::Direction directionOfGap;
		sf::IntRect originalTextureRect;
		std::pair <int, int> position;
		bool isChanged;
	};


	//holds attributes
	std::vector<RegularSprite> sprites;
	std::vector<ButtonText> texts;
	std::vector<OverlaySprite> hovers;
	std::vector<PercentSprite> percents;

	std::map<std::string, ButtonText*> heldText;
	std::map<std::string, OverlaySprite*> heldOverlaySprites;
	std::map<std::string, PercentSprite*> heldPercentSprites;

	
	
private:
	inline int count(const std::string& name, const std::vector<RegularSprite>& vec)
	{
		for(unsigned int it = 0; it < vec.size(); it++)
		{
			if(name == vec.at(it).atrName)
			{
				return (int)it;
			}
		}
		return -1;
	}
	inline int count(const std::string& name, const std::vector<ButtonText>& vec)
	{
		for(unsigned int it = 0; it < vec.size(); it++)
		{
			if(name == vec.at(it).atrName)
			{
				return (int)it;
			}
		}
		return -1;
	}
	inline int count(const std::string& name, const std::vector<OverlaySprite>& vec)
	{
		for(unsigned int it = 0; it < vec.size(); it++)
		{
			if(name == vec.at(it).atrName)
			{
				return (int)it;
			}
		}
		return -1;
	}
	inline int count(const std::string& name, const std::vector<PercentSprite>& vec)
	{
		for(unsigned int it = 0; it < vec.size(); it++)
		{
			if(name == vec.at(it).atrName)
			{
				return (int)it;
			}
		}
		return -1;
	}
};



#endif // BUTTON_H
