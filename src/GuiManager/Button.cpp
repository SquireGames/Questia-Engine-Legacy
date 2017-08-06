#include "QuestiaEng/GuiManager/Button.h"

//TODO give more descriptive LOG's

Button::Button(sf::RenderWindow& window, ResourceManager& resourceManager, sf::Font& buttonFont, bool isTemplate, int buttonID, int buttonGroupID) noexcept:
	window(window)
	, resourceManager(resourceManager)
	, buttonFont(buttonFont)
	, buttonBounds(std::make_pair(0,0))
	, buttonID(buttonID)
	, buttonGroupID(buttonGroupID)
	, isTemplate(isTemplate)
	, isVisible(true)
{}
Button::Button(const Button& oldButton, int buttonID, int buttonGroupID) noexcept:
	window(oldButton.window)
	, resourceManager(oldButton.resourceManager)
	, buttonFont(oldButton.buttonFont)
	, buttonBounds(oldButton.buttonBounds)
	, buttonID(buttonID)
	, buttonGroupID(buttonGroupID)
	, layer(oldButton.layer)
	, isTemplate(false)
	, isVisible(oldButton.isVisible || oldButton.isTemplate)
	, sprites(oldButton.sprites)
	, texts(oldButton.texts)
	, hovers(oldButton.hovers)
	, percents(oldButton.percents)
{}

Button& Button::operator=(const Button& other) noexcept
{
	buttonPosition 	= other.buttonPosition;
	buttonBounds 	= other.buttonBounds;
	buttonID 		= other.buttonID;
	buttonGroupID	= other.buttonGroupID;
	layer 			= other.layer;
	scroll_x 		= other.scroll_x;
	scroll_y	 	= other.scroll_y;
	isTemplate 		= other.isTemplate;
	isVisible 		= other.isVisible;
	isCoordsChanged = other.isCoordsChanged;
	isActive 		= other.isActive;
	sprites 		= other.sprites;
	texts 			= other.texts;
	hovers 			= other.hovers;
	percents 		= other.percents;
	return *this;
}

void Button::setButton(gui::BtnChar buttonChar, const char* value) noexcept
{
	std::string str = std::string(value);
	setButton(buttonChar, str);
}
void Button::setButton(gui::BtnChar buttonChar, const std::string& value) noexcept
{
	switch(buttonChar)
	{
	case gui::BtnChar::bounds:
		{
			int elementID = count(value, sprites);
			if(elementID != -1)
			{
				buttonBounds = std::make_pair(sprites.at(elementID).sprite.getGlobalBounds().width, sprites.at(elementID).sprite.getGlobalBounds().height);
			}
#ifdef DEBUGMODE
			else
			{
				LOG("Sprite with name: '" + value + "' does not exist. Failed to bind bounds to sprite in use of set|Button|Bonds|<string>" + value);
				return;
			}
#endif
		}
		break;
	default:
#ifdef DEBUGMODE
		LOG("Invalid ButtonChar in use of set|Button|ButtonChar|<string>" + value);
#endif
		break;
	}
}
void Button::setButton(gui::BtnChar buttonChar, bool value) noexcept
{
	switch(buttonChar)
	{
	case gui::BtnChar::isVisible:
		isVisible = value;
		break;
	case gui::BtnChar::isActive:
		isActive = value;
		break;
	case gui::BtnChar::isTemplate:
		isTemplate = value;
	default:
		break;
	}
}
void Button::setButton(gui::BtnChar buttonChar, int value) noexcept
{
	switch(buttonChar)
	{
	case gui::BtnChar::addToScroll_x:
		scroll_x += value;
		break;
	case gui::BtnChar::addToScroll_y:
		scroll_y += value;
		break;
	case gui::BtnChar::scroll_x:
		scroll_x = value;
		break;
	case gui::BtnChar::scroll_y:
		scroll_y = value;
		break;
	default:
		break;
	}
	isCoordsChanged = true;
}
void Button::setButton(gui::BtnChar buttonChar, std::pair <int, int> value) noexcept
{
	switch(buttonChar)
	{
	case gui::BtnChar::coords:
		isCoordsChanged = true;
		buttonPosition = value;
		break;
	case gui::BtnChar::bounds:
		buttonBounds = value;
		break;
	case gui::BtnChar::coords_group:
		isCoordsChanged = true;
		buttonPosition = std::make_pair(buttonPosition.first + value.first, buttonPosition.second + value.second);
		break;
	default:
		break;
	}
}

void Button::addBtnAtr(const std::string& atrName, gui::BtnAtr buttonAtr) noexcept
{
	switch(buttonAtr)
	{
	case gui::BtnAtr::Sprite:
		sprites.emplace_back(RegularSprite(atrName));
		break;
	case gui::BtnAtr::Text:
		texts.emplace_back(ButtonText(atrName, buttonFont));
		break;
	case gui::BtnAtr::Hover:
		hovers.emplace_back(OverlaySprite(atrName, sf::Vector2f(buttonBounds.first,buttonBounds.second)));
		break;
	case gui::BtnAtr::Percent:
		percents.emplace_back(PercentSprite(atrName));
		break;
	default:
		break;
	}
}
void Button::setBtnAtr(const std::string& atrName, gui::BtnAtrChar atrChar, const std::string& value) noexcept
{
	int elementID = count(atrName, sprites);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::texture:
			sprites.at(elementID).sprite.setTexture(resourceManager.getTexture(value));
			break;
		case gui::BtnAtrChar::flip:
			setBtnAtr(atrName, atrChar, value.size() ? value.at(0) : 0);
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<string>" + value + " for Sprite");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, texts);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::text:
			texts.at(elementID).text.setString(value);
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<string>" + value + " for Text");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, percents);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::texture:
			percents.at(elementID).sprite.setTexture(resourceManager.getTexture(value));
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<string>" + value + " for Percent");
#endif
			break;
		}
	}
}
void Button::setBtnAtr(const std::string& atrName, gui::BtnAtrChar atrChar, const std::u32string& value) noexcept
{
	int elementID = count(atrName, texts);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::text:
			texts.at(elementID).text.setString(sf::String::fromUtf32(value.begin(), value.end()));
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<32string>" + sf::String::fromUtf32(value.begin(), value.end()) + " for Text");
#endif
			break;
		}
		return;
	}
#ifdef DEBUGMODE
	LOG("'" + atrName + "' is not an existing Text.");
#endif
}
void Button::setBtnAtr(const std::string& atrName, gui::BtnAtrChar atrChar, std::pair<int, int> value) noexcept
{
	int elementID = count(atrName, sprites);
	if(elementID != -1)
	{
		RegularSprite& spr = sprites.at(elementID);
		switch(atrChar)
		{
		case gui::BtnAtrChar::coords:
			spr.position = value;
			break;
		case gui::BtnAtrChar::size:
			spr.sprite.setScale(sf::Vector2f(value.first/spr.sprite.getLocalBounds().width,
			                                 value.second/spr.sprite.getLocalBounds().height));
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<int>" + std::to_string(value.first) + "|<int>" + std::to_string(value.second) + " for Sprite");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, texts);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::coords:
			texts.at(elementID).position = value;
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<int>" + std::to_string(value.first) + "|<int>" + std::to_string(value.second) + " for Text");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, percents);
	if(elementID != -1)
	{
		PercentSprite& per = percents.at(elementID);
		switch(atrChar)
		{
		case gui::BtnAtrChar::coords:
			per.position = value;
			break;
		case gui::BtnAtrChar::size:
			if(per.sprite.getTexture() != nullptr)
			{
				per.sprite.setScale(sf::Vector2f((float)value.first/per.sprite.getLocalBounds().width,
				                                 (float)value.second/per.sprite.getLocalBounds().height));
			}
			else
			{
				per.rectOverlay.setSize(sf::Vector2f(value.first, value.second));
				per.originalTextureRect.width = value.first;
				per.originalTextureRect.height = value.second;
			}
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<int>" + std::to_string(value.first) + "|<int>" + std::to_string(value.second) + " for Percent");
#endif
			break;
		}
		return;
	}
}
void Button::setBtnAtr(const std::string& atrName, gui::BtnAtrChar atrChar, sf::Color color) noexcept
{
	int elementID = count(atrName, sprites);
	if(elementID != -1)
	{
		sprites.at(elementID).sprite.setColor(color);
		return;
	}
	elementID = count(atrName, texts);
	if(elementID != -1)
	{
		texts.at(elementID).text.setFillColor(color);
		texts.at(elementID).text.setOutlineColor(color);
		return;
	}
	elementID = count(atrName, hovers);
	if(elementID != -1)
	{
		hovers.at(elementID).rectOverlay.setFillColor(color);
		return;
	}
	elementID = count(atrName, percents);
	if(elementID != -1)
	{
		percents.at(elementID).rectOverlay.setFillColor(color);
		return;
	}
#ifdef DEBUGMODE
	LOG("No ButtonAtr with name: '" + atrName + "'  in use of set|ButtonAtr|" + atrName + "|<Should be Color>|<Color>");
#endif
}
void Button::setBtnAtr(const std::string& atrName, gui::BtnAtrChar atrChar, int value) noexcept
{
	int elementID = count(atrName, sprites);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::transparency:
			{
				sf::Color newColor = sprites.at(elementID).sprite.getColor();
				newColor.a =  static_cast <float>(value) * 255 / 100;
				sprites.at(elementID).sprite.setColor(newColor);
			}
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<int>" + std::to_string(value) + " for Sprite");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, texts);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::charSize:
			texts.at(elementID).text.setCharacterSize(value * 2);
			texts.at(elementID).text.setScale(0.5, 0.5);
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<int>" + std::to_string(value) + " for Text");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, hovers);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::transparency:
			{
				sf::Color newColor = hovers.at(elementID).rectOverlay.getFillColor();
				newColor.a = static_cast <float>(value) * 255 / 100;
				hovers.at(elementID).rectOverlay.setFillColor(newColor);
			}
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<int>" + std::to_string(value) + " for Hover");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, percents);
	if(elementID != -1)
	{
		PercentSprite& per = percents.at(elementID);
		switch(atrChar)
		{
		case gui::BtnAtrChar::percentage:
			{
				float percent = (float) value / 100;
				per.spritePercentage = percent;
				if(per.sprite.getTexture() != nullptr)
				{
					switch(per.directionOfGap)
					{
					case utl::Direction::up:
						{
							sf::IntRect spriteRect = per.originalTextureRect;
							spriteRect.height *= percent;
							per.sprite.setTextureRect(spriteRect);
						}
						break;
					case utl::Direction::down:
						{
							sf::IntRect spriteRect = per.originalTextureRect;
							spriteRect.height *= percent;
							per.sprite.setTextureRect(spriteRect);
						}
						break;
					case utl::Direction::left:
						{
							sf::IntRect spriteRect = per.originalTextureRect;
							spriteRect.width *= percent;
							per.sprite.setTextureRect(spriteRect);
						}
						break;
					case utl::Direction::right:
						{
							sf::IntRect spriteRect = per.originalTextureRect;
							spriteRect.width *= percent;
							per.sprite.setTextureRect(spriteRect);
						}
						break;
					default:
						break;
					}
				}
				else
				{
					switch(per.directionOfGap)
					{
					case utl::Direction::up:
						per.rectOverlay.setOrigin(0, -1 * per.originalTextureRect.height * (1-percent));
						per.rectOverlay.setSize(sf::Vector2f(per.originalTextureRect.width,
						                                     per.originalTextureRect.height * percent));
						break;
					case utl::Direction::down:
						per.rectOverlay.setSize(sf::Vector2f(per.originalTextureRect.width,
						                                     per.originalTextureRect.height * percent));
						break;
					case utl::Direction::left:
						per.rectOverlay.setOrigin(-1 * per.originalTextureRect.width * (1-percent), 0);
						per.rectOverlay.setSize(sf::Vector2f(per.originalTextureRect.width * percent,
						                                     per.originalTextureRect.height));
						break;
					case utl::Direction::right:
						per.rectOverlay.setSize(sf::Vector2f(per.originalTextureRect.width * percent,
						                                     per.originalTextureRect.height));
						break;
					default:
						break;
					}
				}
			}
			break;
		case gui::BtnAtrChar::transparency:
			{
				if(per.sprite.getTexture() != nullptr)
				{
					sf::Color newColor = per.sprite.getColor();
					newColor.a = static_cast <float>(value) * 255 / 100;
					per.sprite.setColor(newColor);
				}
				else
				{
					sf::Color newColor = per.rectOverlay.getFillColor();
					newColor.a = static_cast <float>(value) * 255 / 100;
					per.rectOverlay.setFillColor(newColor);
				}
			}
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<int>" + std::to_string(value) + " for Percent");
#endif
			break;
		}
		return;
	}
}
void Button::setBtnAtr(const std::string& atrName, gui::BtnAtrChar atrChar, char value) noexcept
{
	int elementID = count(atrName, sprites);
	if(elementID != -1)
	{
		RegularSprite& spr = sprites.at(elementID);
		switch(atrChar)
		{
		case gui::BtnAtrChar::flip:
			{
				switch(value)
				{
				case 'x':
					spr.sprite.setTextureRect(sf::IntRect(spr.sprite.getLocalBounds().width,
					                                      0,
					                                      -1 * spr.sprite.getLocalBounds().width,
					                                      spr.sprite.getLocalBounds().height));
					break;
				case 'y':
					spr.sprite.setTextureRect(sf::IntRect(0,
					                                      spr.sprite.getLocalBounds().height,
					                                      spr.sprite.getLocalBounds().width,
					                                      -1 * spr.sprite.getLocalBounds().height));
					break;
				case 'b':
					spr.sprite.setTextureRect(sf::IntRect(spr.sprite.getLocalBounds().width,
					                                      spr.sprite.getLocalBounds().height,
					                                      -1 * spr.sprite.getLocalBounds().width,
					                                      -1 * spr.sprite.getLocalBounds().height));
					break;
				case '0':
				case 'n':
					spr.sprite.setTextureRect(sf::IntRect(0,
					                                      0,
					                                      spr.sprite.getLocalBounds().width,
					                                      spr.sprite.getLocalBounds().height));
					break;
				default:
#ifdef DEBUGMODE
					LOG("Invalid <char> value in use of set|ButtonAtr|" + atrName + "|BtnAtr|<char>" + value + " for Sprite");
#endif
					break;
				}
			}
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<char>" + value + " for Sprite");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, texts);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::charSize:
			texts.at(elementID).text.setCharacterSize(value*2);
			texts.at(elementID).text.setScale(0.5, 0.5);
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<char>" + value + " for Text");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, hovers);
	if(elementID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::transparency:
			{
				sf::Color newColor = hovers.at(elementID).rectOverlay.getFillColor();
				newColor.a = value;
				hovers.at(elementID).rectOverlay.setFillColor(newColor);
			}
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<char>" + value + " for Hover");
#endif
			break;
		}
		return;
	}
	elementID = count(atrName, percents);
	if(elementID != -1)
	{
		PercentSprite& per = percents.at(elementID);
		switch(atrChar)
		{
		case gui::BtnAtrChar::direction:
			{
				if(per.sprite.getTexture() != nullptr)
				{
					switch(value)
					{
					case 'u':
						per.directionOfGap = utl::Direction::up;
						per.sprite.setOrigin(0, per.sprite.getLocalBounds().height);
						per.sprite.setScale(per.sprite.getScale().x,per.sprite.getScale().y * -1);
						per.originalTextureRect = sf::IntRect(0,
						                                      per.sprite.getLocalBounds().height,
						                                      per.sprite.getLocalBounds().width,
						                                      -1 * per.sprite.getLocalBounds().height);
						break;
					case 'd':
						per.directionOfGap = utl::Direction::down;
						per.originalTextureRect = sf::IntRect(0,
						                                      0,
						                                      per.sprite.getLocalBounds().width,
						                                      per.sprite.getLocalBounds().height);
						break;
					case 'l':
						per.directionOfGap = utl::Direction::left;
						per.sprite.setOrigin(per.sprite.getLocalBounds().width, 0);
						per.sprite.setScale(per.sprite.getScale().x * -1,per.sprite.getScale().y);
						per.originalTextureRect = sf::IntRect(per.sprite.getLocalBounds().width,
						                                      0,
						                                      -1 * per.sprite.getLocalBounds().width,
						                                      per.sprite.getLocalBounds().height);
						break;
					case 'r':
						per.directionOfGap = utl::Direction::right;
						per.originalTextureRect = sf::IntRect(0,
						                                      0,
						                                      per.sprite.getLocalBounds().width,
						                                      per.sprite.getLocalBounds().height);
						break;
					default:
#ifdef DEBUGMODE
						LOG("Invalid <char> value in use of set|ButtonAtr|" + atrName + "|BtnAtr|<char>" + value + " for Percent");
#endif
						break;
					}
				}
				else
				{
					per.rectOverlay.setOrigin(0,0);
					per.rectOverlay.setSize(sf::Vector2f(per.originalTextureRect.width,
					                                     per.originalTextureRect.height));
					switch(value)
					{
					case 'u':
						per.directionOfGap = utl::Direction::up;
						break;
					case 'd':
						per.directionOfGap = utl::Direction::down;
						break;
					case 'l':
						per.directionOfGap = utl::Direction::left;
						break;
					case 'r':
						per.directionOfGap = utl::Direction::right;
						break;
					default:
#ifdef DEBUGMODE
						LOG("Invalid <char> value in use of set|ButtonAtr|" + atrName + "|BtnAtr|<char>" + value + " for Percent");
#endif
						break;
					}
				}
			}
			break;
		default:
#ifdef DEBUGMODE
			LOG("Invalid BtnAtr in use of set|ButtonAtr|" + atrName + "|BtnAtr|<char>" + value + " for Percent");
#endif
			break;
		}
		return;
	}
}

void Button::update(std::pair <int, int> mouseCoords) noexcept
{
	bool wasHovered = isHovered;
	if(mouseCoords.first >  buttonPosition.first + scroll_x && mouseCoords.first < buttonPosition.first + scroll_x + buttonBounds.first &&
	        mouseCoords.second > buttonPosition.second + scroll_y && mouseCoords.second < buttonPosition.second + scroll_y + buttonBounds.second)
	{
		isHovered = (isActive && isVisible);
		if(wasHovered != isHovered)
		{
			for(unsigned int i = 0; i < hovers.size(); i++)
			{
				hovers.at(i).isChanged = true;
			}
		}
	}
	else
	{
		isHovered = false;
		if(wasHovered != isHovered)
		{
			for(unsigned int i = 0; i < hovers.size(); i++)
			{
				hovers.at(i).isChanged = true;
			}
		}
	}
}

void Button::draw() noexcept
{
	if(isVisible && !isTemplate)
	{
		for(unsigned int i = 0; i < sprites.size(); i++)
		{
			RegularSprite& spr = sprites.at(i);
			if(!(spr.isChanged || isCoordsChanged))
			{
				window.draw(spr.sprite);
			}
			else
			{
				spr.sprite.setPosition(buttonPosition.first  + spr.position.first  + scroll_x,
				                       buttonPosition.second + spr.position.second + scroll_y);
				spr.isChanged = false;
				window.draw(spr.sprite);
			}
		}
		for(unsigned int i = 0; i < percents.size(); i++)
		{
			PercentSprite& per = percents.at(i);
			if(!(per.isChanged || isCoordsChanged))
			{
				if(per.sprite.getTexture() != nullptr)
				{
					window.draw(per.sprite);
				}
				else
				{
					window.draw(per.rectOverlay);
				}
			}
			else
			{
				if(per.sprite.getTexture() != nullptr)
				{
					per.sprite.setPosition(buttonPosition.first  + per.position.first  + scroll_x,
					                       buttonPosition.second + per.position.second + scroll_y);
					window.draw(per.sprite);
				}
				else
				{
					per.rectOverlay.setPosition(buttonPosition.first  + per.position.first  + scroll_x,
					                            buttonPosition.second + per.position.second + scroll_y);
					window.draw(per.rectOverlay);
				}
				per.isChanged = false;
			}
		}
		for(unsigned int i = 0; i < hovers.size(); i++)
		{
			OverlaySprite& ovr = hovers.at(i);
			if(!(ovr.isChanged || isCoordsChanged))
			{
				if(isHovered && isActive)
				{
					window.draw(ovr.rectOverlay);
				}
			}
			else
			{
				ovr.rectOverlay.setPosition(buttonPosition.first  + ovr.position.first  + scroll_x,
				                            buttonPosition.second + ovr.position.second + scroll_y);
				ovr.isChanged = false;
				if(isHovered && isActive)
				{
					window.draw(ovr.rectOverlay);
				}
			}
		}
		for(unsigned int i = 0; i < texts.size(); i++)
		{
			ButtonText& txt = texts.at(i);
			if(!(txt.isChanged || isCoordsChanged))
			{
				window.draw(txt.text);
			}
			else
			{
				txt.text.setPosition(buttonPosition.first  + txt.position.first  + scroll_x,
				                     buttonPosition.second + txt.position.second + scroll_y);
				txt.isChanged = false;
				window.draw(txt.text);
			}
		}
		isCoordsChanged = false;
	}
}

bool Button::count(gui::BtnAtr atr, const std::string& atrName) const noexcept
{
	switch(atr)
	{
	case gui::BtnAtr::Sprite:
		return count(atrName, sprites);
		break;
	case gui::BtnAtr::Text:
		return count(atrName, texts);
		break;
	case gui::BtnAtr::Hover:
		return count(atrName, hovers);
		break;
	case gui::BtnAtr::Percent:
		return count(atrName, percents);
		break;
	default:
		break;
	}
	return false;
}
