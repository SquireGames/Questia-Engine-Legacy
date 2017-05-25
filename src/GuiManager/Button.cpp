#include "QuestiaEng/GuiManager/Button.h"

Button::Button(sf::RenderWindow& _window, ResourceManager &_resourceManager, sf::Font& _buttonFont, bool _isTemplate, int _buttonID):
	window(_window)
	, resourceManager(_resourceManager)
	, buttonFont(_buttonFont)
	, buttonBounds(std::make_pair(0,0))
	, buttonID(_buttonID)
	, isTemplate(_isTemplate)
	, isVisible(true)
{}

Button::Button(const Button& oldButton, int _buttonID):
	window(oldButton.window)
	, resourceManager(oldButton.resourceManager)
	, buttonFont(oldButton.buttonFont)
	, buttonBounds(oldButton.buttonBounds)
	, buttonID(_buttonID)
	, layer(oldButton.layer)
	, isTemplate(false)
	, isVisible(oldButton.isVisible || oldButton.isTemplate)
	, sprites(oldButton.sprites)
	, texts(oldButton.texts)
	, hovers(oldButton.hovers)
	, percents(oldButton.percents)
{
	//TODO remove
	copyToThisButton(*this, oldButton);
}

// copies attributes
//TODO remove
void Button::copyToThisButton(Button& newButton, const Button& oldButton)
{
	for(std::map<std::string, ButtonText*>::const_iterator it = oldButton.heldText.begin(); it != oldButton.heldText.end(); ++it)
	{
		newButton.addBtnAtr(it->first, gui::BtnAtr::Text);
		newButton.heldText[it->first]->text      = oldButton.heldText.at(it->first)->text;
		newButton.heldText[it->first]->position  = oldButton.heldText.at(it->first)->position;
		newButton.heldText[it->first]->isChanged = oldButton.heldText.at(it->first)->isChanged;
	}
	for(std::map<std::string, OverlaySprite*>::const_iterator it = oldButton.heldOverlaySprites.begin(); it != oldButton.heldOverlaySprites.end(); ++it)
	{
		newButton.addBtnAtr(it->first, gui::BtnAtr::Hover);
		newButton.heldOverlaySprites[it->first]->rectOverlay   = oldButton.heldOverlaySprites.at(it->first)->rectOverlay;
		newButton.heldOverlaySprites[it->first]->isChanged     = oldButton.heldOverlaySprites.at(it->first)->isChanged;
		newButton.heldOverlaySprites[it->first]->position      = oldButton.heldOverlaySprites.at(it->first)->position;
		newButton.heldOverlaySprites[it->first]->isHoveredOver = oldButton.heldOverlaySprites.at(it->first)->isHoveredOver;
	}
	for(std::map<std::string, PercentSprite*>::const_iterator it = oldButton.heldPercentSprites.begin(); it != oldButton.heldPercentSprites.end(); ++it)
	{
		newButton.addBtnAtr(it->first, gui::BtnAtr::Percent);
		newButton.heldPercentSprites[it->first]->spritePercentage = oldButton.heldPercentSprites.at(it->first)->spritePercentage;
		newButton.heldPercentSprites[it->first]->sprite     = oldButton.heldPercentSprites.at(it->first)->sprite;
		newButton.heldPercentSprites[it->first]->directionOfGap   = oldButton.heldPercentSprites.at(it->first)->directionOfGap;
		newButton.heldPercentSprites[it->first]->isChanged        = oldButton.heldPercentSprites.at(it->first)->isChanged;
		newButton.heldPercentSprites[it->first]->position         = oldButton.heldPercentSprites.at(it->first)->position;
		newButton.heldPercentSprites[it->first]->rectOverlay      = oldButton.heldPercentSprites.at(it->first)->rectOverlay;
		newButton.heldPercentSprites[it->first]->originalTextureRect = oldButton.heldPercentSprites.at(it->first)->originalTextureRect;
	}
}

Button::~Button()
{
	//TODO remove
	for(std::map<std::string, ButtonText*>::iterator it = heldText.begin(); it != heldText.end(); ++it)
	{
		delete it->second;
	}
	for(std::map<std::string, OverlaySprite*>::iterator it = heldOverlaySprites.begin(); it != heldOverlaySprites.end(); ++it)
	{
		delete it->second;
	}
	for(std::map<std::string, PercentSprite*>::iterator it = heldPercentSprites.begin(); it != heldPercentSprites.end(); ++it)
	{
		delete it->second;
	}
}

void Button::setButton(gui::BtnChar buttonChar, std::string value)
{
	switch(buttonChar)
	{
	case gui::BtnChar::bounds:
		{
			int spriteID = count(value, sprites);
			if(spriteID != -1)
			{
				buttonBounds = std::make_pair(sprites.at(spriteID).sprite.getGlobalBounds().width, sprites.at(spriteID).sprite.getGlobalBounds().height);
			}
#ifdef DEBUGMODE
			else
			{
				std::cout << "BUTTON: Warning - there is no button atr with the name: " << value << " in the button " << buttonName << std::endl;
			}
#endif
		}
		break;
	default:
		break;
	}
}
void Button::setButton(gui::BtnChar buttonChar, const char* value)
{
	setButton(buttonChar, std::string(value));
}
void Button::setButton(gui::BtnChar buttonChar, bool value)
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
void Button::setButton(gui::BtnChar buttonChar, int value)
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
void Button::setButton(gui::BtnChar buttonChar, std::pair <int, int> value)
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

void Button::addBtnAtr(std::string atrName, gui::BtnAtr buttonAtr)
{
	switch(buttonAtr)
	{
	case gui::BtnAtr::Sprite:
		{
			sprites.emplace_back(RegularSprite(atrName));
		}
		break;
	case gui::BtnAtr::Text:
		{
			ButtonText* newText = new ButtonText;
			newText->isChanged = true;
			newText->position = std::make_pair(0,0);
			newText->text.setFont(buttonFont);
			newText->text.setString(std::string());
			newText->text.setFillColor(sf::Color::Black);
			newText->text.setOutlineColor(sf::Color::Black);
			newText->text.setPosition(newText->position.first  + buttonPosition.first,
			                          newText->position.second + buttonPosition.second);
			heldText[atrName] = newText;
		}
		break;
	case gui::BtnAtr::Hover:
		{
			OverlaySprite* newHover = new OverlaySprite;
			newHover->isChanged = true;
			newHover->position = std::make_pair(0,0);

			newHover->rectOverlay.setFillColor(sf::Color(0,0,0, 100));

			newHover->rectOverlay.setPosition(buttonPosition.first, buttonPosition.second);
			newHover->rectOverlay.setSize(sf::Vector2f(buttonBounds.first,buttonBounds.second));
			heldOverlaySprites[atrName] = newHover;
		}
		break;
	case gui::BtnAtr::Percent:
		{
			PercentSprite* newPercent = new PercentSprite;
			newPercent->isChanged = true;
			newPercent->position = std::make_pair(0,0);
			newPercent->spritePercentage = 1.f;
			newPercent->directionOfGap = utl::Direction::right;
			newPercent->sprite.setPosition(newPercent->position.first  + buttonPosition.first,
			                               newPercent->position.second + buttonPosition.second);
			newPercent->rectOverlay.setPosition(newPercent->position.first  + buttonPosition.first,
			                                    newPercent->position.second + buttonPosition.second);
			newPercent->originalTextureRect = sf::IntRect(0,0,0,0);
			heldPercentSprites[atrName] = newPercent;
		}
		break;
	default:
		{

		}
		break;
	}
}

void Button::setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, std::string value)
{
	int spriteID = count(atrName, sprites);
	if(spriteID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::texture:
			sprites.at(spriteID).sprite.setTexture(resourceManager.getTexture(value));
			break;
		case gui::BtnAtrChar::flip:
			setBtnAtr(atrName, atrChar, value.size() ? value.at(0) : 0);
			break;
		default:
			//TODO print warning if invalid
			break;
		}

		//TODO uncomment
		//return;
	}


	//TODO remove
	if(heldText.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::text:
			heldText[atrName]->text.setString(value);
			break;
		default:
			break;
		}
	}
	else if(heldOverlaySprites.count(atrName))
	{
		switch(atrChar)
		{
		default:
			break;
		}

	}
	else if(heldPercentSprites.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::texture:
			heldPercentSprites[atrName]->sprite.setTexture(resourceManager.getTexture(value));
			break;
		default:
			break;
		}
	}
}
void Button::setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, std::pair<int, int> value)
{
	int spriteID = count(atrName, sprites);
	if(spriteID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::coords:
			sprites.at(spriteID).position = value;
			break;
		case gui::BtnAtrChar::size:
			sprites.at(spriteID).sprite.setScale(
			    sf::Vector2f(value.first/sprites.at(spriteID).sprite.getLocalBounds().width,
			                 value.second/sprites.at(spriteID).sprite.getLocalBounds().height));
		default:
			//TODO print warning if invalid
			break;
		}

		//TODO uncomment
		//return;
	}

	//TODO remove
	if(heldText.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::coords:
			heldText[atrName]->position = value;
			break;
		default:
			break;
		}
	}
	else if(heldOverlaySprites.count(atrName))
	{
		switch(atrChar)
		{
		default:
			break;
		}
	}
	else if(heldPercentSprites.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::coords:
			heldPercentSprites[atrName]->position = value;
			break;
		case gui::BtnAtrChar::size:
			if(heldPercentSprites[atrName]->sprite.getTexture() != nullptr)
			{
				heldPercentSprites[atrName]->sprite.setScale(
				    sf::Vector2f((float)value.first/heldPercentSprites[atrName]->sprite.getLocalBounds().width,
				                 (float)value.second/heldPercentSprites[atrName]->sprite.getLocalBounds().height));
			}
			else
			{
				heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(value.first, value.second));
				heldPercentSprites[atrName]->originalTextureRect.width = value.first;
				heldPercentSprites[atrName]->originalTextureRect.height = value.second;
			}
			break;
		default:
			break;
		}
	}
}
void Button::setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, sf::Color color)
{
	int spriteID = count(atrName, sprites);
	if(spriteID != -1)
	{
		sprites.at(spriteID).sprite.setColor(color);
		//TODO uncomment
		//return;
	}

	//TODO remove
	if(heldText.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::color:
			heldText[atrName]->text.setFillColor(color);
			heldText[atrName]->text.setOutlineColor(color);
			break;
		default:
			break;
		}
	}
	else if(heldOverlaySprites.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::color:
			heldOverlaySprites[atrName]->rectOverlay.setFillColor(color);
			break;
		default:
			break;
		}

	}
	else if(heldPercentSprites.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::color:
			heldPercentSprites[atrName]->rectOverlay.setFillColor(color);
			break;
		default:
			break;
		}
	}
}
void Button::setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, int value)
{
	int spriteID = count(atrName, sprites);
	if(spriteID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::transparency:
			{
				sf::Color newColor = sprites.at(spriteID).sprite.getColor();
				float trans = static_cast <float>(value);
				trans = trans * 255 / 100;
				newColor.a = trans;
				sprites.at(spriteID).sprite.setColor(newColor);
			}
			break;
		default:
			//TODO print warning if invalid
			break;
		}

		//TODO uncomment
		//return;

	}

	if(heldText.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::charSize:
			heldText[atrName]->text.setCharacterSize(value * 2);
			heldText[atrName]->text.setScale(0.5, 0.5);
			break;
		default:
			break;
		}
	}
	else if(heldOverlaySprites.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::transparency:
			{
				sf::Color newColor = heldOverlaySprites[atrName]->rectOverlay.getFillColor();
				float trans = static_cast <float>(value);
				trans = trans * 255 / 100;
				newColor.a = trans;
				heldOverlaySprites[atrName]->rectOverlay.setFillColor(newColor);
			}
			break;
		default:
			break;
		}

	}
	else if(heldPercentSprites.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::percentage:
			{
				float percent = (float)value / 100;
				heldPercentSprites[atrName]->spritePercentage = percent;
				if(heldPercentSprites[atrName]->sprite.getTexture() != nullptr)
				{
					switch(heldPercentSprites[atrName]->directionOfGap)
					{
					case utl::Direction::up:
						{
							sf::IntRect spriteRect = heldPercentSprites[atrName]->originalTextureRect;
							spriteRect.height *= percent;
							heldPercentSprites[atrName]->sprite.setTextureRect(spriteRect);
						}
						break;
					case utl::Direction::down:
						{
							sf::IntRect spriteRect = heldPercentSprites[atrName]->originalTextureRect;
							spriteRect.height *= percent;
							heldPercentSprites[atrName]->sprite.setTextureRect(spriteRect);
						}
						break;
					case utl::Direction::left:
						{
							sf::IntRect spriteRect = heldPercentSprites[atrName]->originalTextureRect;
							spriteRect.width *= percent;
							heldPercentSprites[atrName]->sprite.setTextureRect(spriteRect);
						}
						break;
					case utl::Direction::right:
						{
							sf::IntRect spriteRect = heldPercentSprites[atrName]->originalTextureRect;
							spriteRect.width *= percent;
							heldPercentSprites[atrName]->sprite.setTextureRect(spriteRect);
						}
						break;
					default:
						break;
					}

				}
				else
				{
					switch(heldPercentSprites[atrName]->directionOfGap)
					{
					case utl::Direction::up:
						heldPercentSprites[atrName]->rectOverlay.setOrigin(0, -1 * heldPercentSprites[atrName]->originalTextureRect.height * (1-percent));
						heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
						            heldPercentSprites[atrName]->originalTextureRect.width,
						            heldPercentSprites[atrName]->originalTextureRect.height * percent));
						break;
					case utl::Direction::down:

						heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
						            heldPercentSprites[atrName]->originalTextureRect.width,
						            heldPercentSprites[atrName]->originalTextureRect.height * percent));
						break;
					case utl::Direction::left:
						heldPercentSprites[atrName]->rectOverlay.setOrigin(-1 * heldPercentSprites[atrName]->originalTextureRect.width * (1-percent), 0);
						heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
						            heldPercentSprites[atrName]->originalTextureRect.width * percent,
						            heldPercentSprites[atrName]->originalTextureRect.height));
						break;
					case utl::Direction::right:
						heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
						            heldPercentSprites[atrName]->originalTextureRect.width * percent,
						            heldPercentSprites[atrName]->originalTextureRect.height));
						break;
					default:
						break;
					}

				}
			}
			break;

		case gui::BtnAtrChar::transparency:
			{
				if(heldPercentSprites[atrName]->sprite.getTexture() != nullptr)
				{
					sf::Color newColor = heldPercentSprites[atrName]->sprite.getColor();
					float trans = static_cast <float>(value);
					trans = trans * 255 / 100;
					newColor.a = trans;
					heldPercentSprites[atrName]->sprite.setColor(newColor);
				}
				else
				{
					sf::Color newColor = heldPercentSprites[atrName]->rectOverlay.getFillColor();
					float trans = static_cast <float>(value);
					trans = trans * 255 / 100;
					newColor.a = trans;
					heldPercentSprites[atrName]->rectOverlay.setFillColor(newColor);
				}
			}
			break;
		default:
			break;
		}
	}
}
void Button::setBtnAtr(std::string atrName, gui::BtnAtrChar atrChar, char value)
{
	int spriteID = count(atrName, sprites);
	if(spriteID != -1)
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::flip:
			{
				switch(value)
				{
				case 'x':
					sprites.at(spriteID).sprite.setTextureRect(sf::IntRect(sprites.at(spriteID).sprite.getLocalBounds().width,
					        0,
					        -1 * sprites.at(spriteID).sprite.getLocalBounds().width,
					        sprites.at(spriteID).sprite.getLocalBounds().height));
					break;
				case 'y':
					sprites.at(spriteID).sprite.setTextureRect(sf::IntRect(0,
					        sprites.at(spriteID).sprite.getLocalBounds().height,
					        sprites.at(spriteID).sprite.getLocalBounds().width,
					        -1 * sprites.at(spriteID).sprite.getLocalBounds().height));
					break;
				case 'b':
					sprites.at(spriteID).sprite.setTextureRect(sf::IntRect(sprites.at(spriteID).sprite.getLocalBounds().width,
					        sprites.at(spriteID).sprite.getLocalBounds().height,
					        -1 * sprites.at(spriteID).sprite.getLocalBounds().width,
					        -1 * sprites.at(spriteID).sprite.getLocalBounds().height));
					break;
				case '0':
				case 'n':
					sprites.at(spriteID).sprite.setTextureRect(sf::IntRect(0,
					        0,
					        sprites.at(spriteID).sprite.getLocalBounds().width,
					        sprites.at(spriteID).sprite.getLocalBounds().height));
					break;
				default:
					//TODO print warning if invalid
					break;
				}
			}
			break;
		default:
			//TODO print warning if invalid
			break;
		}

		//TODO uncomment
		//return;
	}

	if(heldText.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::charSize:
			heldText[atrName]->text.setCharacterSize(value*2);
			heldText[atrName]->text.setScale(0.5, 0.5);
			break;
		default:
			break;
		}
	}
	else if(heldOverlaySprites.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::transparency:
			{
				sf::Color newColor = heldOverlaySprites[atrName]->rectOverlay.getFillColor();
				newColor.a = value;
				heldOverlaySprites[atrName]->rectOverlay.setFillColor(newColor);
			}
			break;
		default:
			break;
		}

	}
	else if(heldPercentSprites.count(atrName))
	{
		switch(atrChar)
		{
		case gui::BtnAtrChar::direction:
			{
				if(heldPercentSprites[atrName]->sprite.getTexture() != nullptr)
				{
					if(value == 'u')
					{
						heldPercentSprites[atrName]->directionOfGap = utl::Direction::up;

						heldPercentSprites[atrName]->sprite.setOrigin(0, heldPercentSprites[atrName]->sprite.getLocalBounds().height);
						heldPercentSprites[atrName]->sprite.setScale(heldPercentSprites[atrName]->sprite.getScale().x,heldPercentSprites[atrName]->sprite.getScale().y * -1);

						heldPercentSprites[atrName]->originalTextureRect = sf::IntRect(
						            0,
						            heldPercentSprites[atrName]->sprite.getLocalBounds().height,
						            heldPercentSprites[atrName]->sprite.getLocalBounds().width,
						            -1 * heldPercentSprites[atrName]->sprite.getLocalBounds().height);
					}
					else if(value == 'd')
					{
						heldPercentSprites[atrName]->directionOfGap = utl::Direction::down;

						heldPercentSprites[atrName]->originalTextureRect = sf::IntRect(
						            0,
						            0,
						            heldPercentSprites[atrName]->sprite.getLocalBounds().width,
						            heldPercentSprites[atrName]->sprite.getLocalBounds().height);

					}
					else if(value == 'l')
					{
						heldPercentSprites[atrName]->directionOfGap = utl::Direction::left;

						heldPercentSprites[atrName]->sprite.setOrigin(heldPercentSprites[atrName]->sprite.getLocalBounds().width, 0);
						heldPercentSprites[atrName]->sprite.setScale(heldPercentSprites[atrName]->sprite.getScale().x * -1,heldPercentSprites[atrName]->sprite.getScale().y);

						heldPercentSprites[atrName]->originalTextureRect = sf::IntRect(
						            heldPercentSprites[atrName]->sprite.getLocalBounds().width,
						            0,
						            -1 * heldPercentSprites[atrName]->sprite.getLocalBounds().width,
						            heldPercentSprites[atrName]->sprite.getLocalBounds().height);
					}
					else if(value == 'r')
					{
						heldPercentSprites[atrName]->directionOfGap = utl::Direction::right;

						heldPercentSprites[atrName]->originalTextureRect = sf::IntRect(
						            0,
						            0,
						            heldPercentSprites[atrName]->sprite.getLocalBounds().width,
						            heldPercentSprites[atrName]->sprite.getLocalBounds().height);
					}
				}
				else
				{
					heldPercentSprites[atrName]->rectOverlay.setOrigin(0,0);
					heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
					            heldPercentSprites[atrName]->originalTextureRect.width,
					            heldPercentSprites[atrName]->originalTextureRect.height));
					if(value == 'u')
					{
						heldPercentSprites[atrName]->directionOfGap = utl::Direction::up;
					}
					else if(value == 'd')
					{
						heldPercentSprites[atrName]->directionOfGap = utl::Direction::down;
					}
					else if(value == 'l')
					{
						heldPercentSprites[atrName]->directionOfGap = utl::Direction::left;
					}
					else if(value == 'r')
					{
						heldPercentSprites[atrName]->directionOfGap = utl::Direction::right;
					}
				}
			}
		default:
			break;
		}
	}
}

void Button::update(std::pair <int, int> mouseCoords)
{
	for(std::map<std::string, OverlaySprite*>::iterator it = heldOverlaySprites.begin(); it != heldOverlaySprites.end(); it++)
	{
		if(mouseCoords.first >  buttonPosition.first + scroll_x && mouseCoords.first  < buttonPosition.first + scroll_x  + buttonBounds.first &&
		        mouseCoords.second > buttonPosition.second + scroll_y && mouseCoords.second < buttonPosition.second + scroll_y + buttonBounds.second)
		{
			it->second->isChanged = true;
			it->second->isHoveredOver = true;
		}
		else
		{
			it->second->isChanged = true;
			it->second->isHoveredOver = false;
		}
	}
}

void Button::draw()
{
	if(isVisible)
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

		//TODO remove
		for(std::map<std::string, OverlaySprite*>::iterator it = heldOverlaySprites.begin(); it != heldOverlaySprites.end(); it++)
		{
			if(it->second->isChanged || isCoordsChanged)
			{
				it->second->rectOverlay.setPosition(buttonPosition.first  + it->second->position.first  + scroll_x,
				                                    buttonPosition.second + it->second->position.second + scroll_y);
				it->second->isChanged = false;
				if(it->second->isHoveredOver)
				{
					if(isActive)
					{
						window.draw(it->second->rectOverlay);
					}
				}
			}
			else
			{
				if(it->second->isHoveredOver && isActive)
				{
					window.draw(it->second->rectOverlay);
				}
			}
		}

		for(std::map<std::string, PercentSprite*>::iterator it = heldPercentSprites.begin(); it != heldPercentSprites.end(); it++)
		{
			if(it->second->isChanged || isCoordsChanged)
			{
				if(it->second->sprite.getTexture() != nullptr)
				{
					it->second->sprite.setPosition(buttonPosition.first  + it->second->position.first  + scroll_x,
					                               buttonPosition.second + it->second->position.second + scroll_y);
					window.draw(it->second->sprite);
				}
				else
				{
					it->second->rectOverlay.setPosition(buttonPosition.first  + it->second->position.first  + scroll_x,
					                                    buttonPosition.second + it->second->position.second + scroll_y);
					window.draw(it->second->rectOverlay);
				}
				it->second->isChanged = false;
			}
			else
			{
				if(it->second->sprite.getTexture() != nullptr)
				{
					window.draw(it->second->sprite);
				}
				else
				{
					window.draw(it->second->rectOverlay);
				}
			}
		}
		for(std::map<std::string, ButtonText*>::iterator it = heldText.begin(); it != heldText.end(); it++)
		{
			if(it->second->isChanged || isCoordsChanged)
			{
				it->second->text.setPosition(buttonPosition.first  + it->second->position.first  + scroll_x,
				                             buttonPosition.second + it->second->position.second + scroll_y);
				it->second->isChanged = false;

				window.draw(it->second->text);
			}
			else
			{
				window.draw(it->second->text);
			}
		}
		isCoordsChanged = false;
	}
}
