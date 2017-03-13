#include "QuestiaEng/GuiManager/Button.h"

//{ Button() + copy constructors
Button::Button(sf::RenderWindow& _window, ResourceManager &_resourceManager, sf::Font& _buttonFont, bool _isTemplate, int _buttonID):
    window(_window)
    , resourceManager(_resourceManager)
    , buttonFont(_buttonFont)
    , buttonPosition (std::make_pair(0,0))
    , buttonBounds (std::make_pair(0,0))
    , isCoordsChanged(true)
    , scrollAmount_x (0)
    , scrollAmount_y (0)
    , isVisible(true)
    , isTemplate(_isTemplate)
    , buttonID(_buttonID)
    , isActive(true)
{}

// copy constructor
Button::Button(const Button& oldButton, int _buttonID): // const to make sure there are no changes to the original
    window(oldButton.window)
    , resourceManager(oldButton.resourceManager)
    , buttonFont (oldButton.buttonFont)
    , buttonPosition (std::make_pair(0,0))
    , buttonBounds (oldButton.buttonBounds)
    , isCoordsChanged(true)
    , scrollAmount_x (0)
    , scrollAmount_y (0)
    , isVisible(oldButton.isVisible)
    , isTemplate(false)
    , buttonID(_buttonID)
    , layer(oldButton.layer)
    , isActive(true)
{
    copyToThisButton(*this, oldButton);

    if(oldButton.isTemplate)
    {
        isVisible = true;
        isActive = true;
    }
}

// copies attributes
void Button::copyToThisButton(Button& newButton, const Button& oldButton)
{
    for(std::map<std::string, RegularSprite*>::const_iterator it = oldButton.heldSprites.begin(); it != oldButton.heldSprites.end(); ++it)
    {
        newButton.addButtonAtr(it->first, gui::ButtonAtr::Sprite);
        newButton.heldSprites[it->first]->normalSprite = oldButton.heldSprites.at(it->first)->normalSprite;
        newButton.heldSprites[it->first]->position     = oldButton.heldSprites.at(it->first)->position;
        newButton.heldSprites[it->first]->isChanged    = oldButton.heldSprites.at(it->first)->isChanged;
    }
    for(std::map<std::string, ButtonText*>::const_iterator it = oldButton.heldText.begin(); it != oldButton.heldText.end(); ++it)
    {
        newButton.addButtonAtr(it->first, gui::ButtonAtr::Text);
        newButton.heldText[it->first]->text      = oldButton.heldText.at(it->first)->text;
        newButton.heldText[it->first]->position  = oldButton.heldText.at(it->first)->position;
        newButton.heldText[it->first]->isChanged = oldButton.heldText.at(it->first)->isChanged;
    }
    for(std::map<std::string, OverlaySprite*>::const_iterator it = oldButton.heldOverlaySprites.begin(); it != oldButton.heldOverlaySprites.end(); ++it)
    {
        newButton.addButtonAtr(it->first, gui::ButtonAtr::Hover);
        newButton.heldOverlaySprites[it->first]->rectOverlay   = oldButton.heldOverlaySprites.at(it->first)->rectOverlay;
        newButton.heldOverlaySprites[it->first]->isChanged     = oldButton.heldOverlaySprites.at(it->first)->isChanged;
        newButton.heldOverlaySprites[it->first]->position      = oldButton.heldOverlaySprites.at(it->first)->position;
        newButton.heldOverlaySprites[it->first]->isHoveredOver = oldButton.heldOverlaySprites.at(it->first)->isHoveredOver;
    }
    for(std::map<std::string, PercentSprite*>::const_iterator it = oldButton.heldPercentSprites.begin(); it != oldButton.heldPercentSprites.end(); ++it)
    {
        newButton.addButtonAtr(it->first, gui::ButtonAtr::Percent);
        newButton.heldPercentSprites[it->first]->spritePercentage = oldButton.heldPercentSprites.at(it->first)->spritePercentage;
        newButton.heldPercentSprites[it->first]->normalSprite     = oldButton.heldPercentSprites.at(it->first)->normalSprite;
        newButton.heldPercentSprites[it->first]->directionOfGap   = oldButton.heldPercentSprites.at(it->first)->directionOfGap;
        newButton.heldPercentSprites[it->first]->isChanged        = oldButton.heldPercentSprites.at(it->first)->isChanged;
        newButton.heldPercentSprites[it->first]->position         = oldButton.heldPercentSprites.at(it->first)->position;
        newButton.heldPercentSprites[it->first]->rectOverlay      = oldButton.heldPercentSprites.at(it->first)->rectOverlay;
        newButton.heldPercentSprites[it->first]->originalTextureRect = oldButton.heldPercentSprites.at(it->first)->originalTextureRect;
    }
}
//}

// deletes attributes
Button::~Button()
{
    for(std::map<std::string, RegularSprite*>::iterator it = heldSprites.begin(); it != heldSprites.end(); ++it)
    {
        delete it->second;
    }
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

//////////////////////////////

void Button::setButton(gui::ButtonCharacteristic buttonChar, std::string value)
{
    switch (buttonChar)
    {
    case gui::ButtonCharacteristic::bounds:
    {
        if(heldSprites.count(value))
        {
            buttonBounds = std::make_pair(heldSprites[value]->normalSprite.getGlobalBounds().width, heldSprites[value]->normalSprite.getGlobalBounds().height);
        }
    }
    break;
    default:
        break;
    }
}
void Button::setButton(gui::ButtonCharacteristic buttonChar, const char* value)
{
    std::string atrName = static_cast <std::string> (value);
    switch (buttonChar)
    {
    case gui::ButtonCharacteristic::bounds:
    {
        if(heldSprites.count(value))
        {
            buttonBounds = std::make_pair(heldSprites[value]->normalSprite.getGlobalBounds().width, heldSprites[value]->normalSprite.getGlobalBounds().height);
        }
    }
    break;
    default:
        break;
    }
}
void Button::setButton(gui::ButtonCharacteristic buttonChar, bool value)
{
    switch (buttonChar)
    {
    case gui::ButtonCharacteristic::isVisible:
        isVisible = value;
        break;
    case gui::ButtonCharacteristic::isActive:
        isActive = value;
        break;
    case gui::ButtonCharacteristic::isTemplate:
        isTemplate = value;
    default:
        break;
    }
}
void Button::setButton(gui::ButtonCharacteristic buttonChar, int value)
{
    switch (buttonChar)
    {
    case gui::ButtonCharacteristic::addToScroll_x:
        scrollAmount_x += value;
        break;
    case gui::ButtonCharacteristic::addToScroll_y:
        scrollAmount_y += value;
        break;
    case gui::ButtonCharacteristic::scroll_x:
        scrollAmount_x = value;
        break;
    case gui::ButtonCharacteristic::scroll_y:
        scrollAmount_y = value;
        break;
    default:
        break;
    }
    isCoordsChanged = true;
}
void Button::setButton(gui::ButtonCharacteristic buttonChar, std::pair <int, int> value)
{
    switch (buttonChar)
    {
    case gui::ButtonCharacteristic::coords:
        isCoordsChanged = true;
        buttonPosition = value;
        break;
    case gui::ButtonCharacteristic::bounds:
        buttonBounds = value;
        break;
    case gui::ButtonCharacteristic::coords_group:
        buttonPosition = std::make_pair(buttonPosition.first + value.first, buttonPosition.second + value.second);
        break;
    default:
        break;
    }
}


void Button::setButtonAtr(std::string atrName, gui::ButtonAtrCharacteristic atrChar, std::string value)
{
    if(heldText.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::text:
            heldText[atrName]->text.setString(value);
            break;
        default:
            break;
        }
    }
    else if(heldSprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::texture:
            heldSprites[atrName]->normalSprite.setTexture(resourceManager.getTexture(value));
            break;
        default:
            break;
        }

    }
    else if(heldOverlaySprites.count(atrName))
    {
        switch (atrChar)
        {
        default:
            break;
        }

    }
    else if(heldPercentSprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::texture:
            heldPercentSprites[atrName]->normalSprite.setTexture(resourceManager.getTexture(value));
            break;
        default:
            break;
        }
    }
}
void Button::setButtonAtr(std::string atrName, gui::ButtonAtrCharacteristic atrChar, std::pair<int, int> value)
{
    if(heldText.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::coords:
            heldText[atrName]->position = value;
            break;
        default:
            break;
        }
    }
    else if(heldSprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::coords:
            heldSprites[atrName]->position = value;
            break;
        case gui::ButtonAtrCharacteristic::size:
            heldSprites[atrName]->normalSprite.setScale(
                sf::Vector2f(value.first/heldSprites[atrName]->normalSprite.getLocalBounds().width,
                             value.second/heldSprites[atrName]->normalSprite.getLocalBounds().height));
        default:
            break;
        }
    }
    else if(heldOverlaySprites.count(atrName))
    {
        switch (atrChar)
        {
        default:
            break;
        }
    }
    else if(heldPercentSprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::coords:
            heldPercentSprites[atrName]->position = value;
            break;
        case gui::ButtonAtrCharacteristic::size:
            if(heldPercentSprites[atrName]->normalSprite.getTexture() != nullptr)
            {
                heldPercentSprites[atrName]->normalSprite.setScale(
                    sf::Vector2f((float)value.first/heldPercentSprites[atrName]->normalSprite.getLocalBounds().width,
                                 (float)value.second/heldPercentSprites[atrName]->normalSprite.getLocalBounds().height));
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
void Button::setButtonAtr(std::string atrName, gui::ButtonAtrCharacteristic atrChar, sf::Color color)
{
    if(heldText.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::color:
            heldText[atrName]->text.setColor(color);
            break;
        default:
            break;
        }
    }
    else if(heldSprites.count(atrName))
    {
        switch (atrChar)
        {
        default:
            heldSprites[atrName]->normalSprite.setColor(color);
            break;
        }
    }
    else if(heldOverlaySprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::color:
            heldOverlaySprites[atrName]->rectOverlay.setFillColor(color);
            break;
        default:
            break;
        }

    }
    else if(heldPercentSprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::color:
            heldPercentSprites[atrName]->rectOverlay.setFillColor(color);
            break;
        default:
            break;
        }
    }
}
void Button::setButtonAtr(std::string atrName, gui::ButtonAtrCharacteristic atrChar, int value)
{
    if(heldText.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::charSize:
            heldText[atrName]->text.setCharacterSize(value);
            break;
        default:
            break;
        }
    }
    else if(heldSprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::transparency:
        {
            sf::Color newColor = heldSprites[atrName]->normalSprite.getColor();
            float trans = static_cast <float> (value);
            trans = trans * 255 / 100;
            newColor.a = trans;
            heldSprites[atrName]->normalSprite.setColor(newColor);
        }
        break;
        default:
            break;
        }

    }
    else if(heldOverlaySprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::transparency:
        {
            sf::Color newColor = heldOverlaySprites[atrName]->rectOverlay.getFillColor();
            float trans = static_cast <float> (value);
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
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::percentage:
        {
            float percent = (float)value / 100;
            heldPercentSprites[atrName]->spritePercentage = percent;
            if(heldPercentSprites[atrName]->normalSprite.getTexture() != nullptr)
            {
                switch (heldPercentSprites[atrName]->directionOfGap)
                {
                case gui::Direction::up:
                {
                    sf::IntRect spriteRect = heldPercentSprites[atrName]->originalTextureRect;
                    spriteRect.height *= percent;
                    heldPercentSprites[atrName]->normalSprite.setTextureRect(spriteRect);
                }
                break;
                case gui::Direction::down:
                {
                    sf::IntRect spriteRect = heldPercentSprites[atrName]->originalTextureRect;
                    spriteRect.height *= percent;
                    heldPercentSprites[atrName]->normalSprite.setTextureRect(spriteRect);
                }
                break;
                case gui::Direction::left:
                {
                    sf::IntRect spriteRect = heldPercentSprites[atrName]->originalTextureRect;
                    spriteRect.width *= percent;
                    heldPercentSprites[atrName]->normalSprite.setTextureRect(spriteRect);
                }
                break;
                case gui::Direction::right:
                {
                    sf::IntRect spriteRect = heldPercentSprites[atrName]->originalTextureRect;
                    spriteRect.width *= percent;
                    heldPercentSprites[atrName]->normalSprite.setTextureRect(spriteRect);
                }
                break;
                }

            }
            else
            {
                switch (heldPercentSprites[atrName]->directionOfGap)
                {
                case gui::Direction::up:
                {
                    heldPercentSprites[atrName]->rectOverlay.setOrigin(0, -1 * heldPercentSprites[atrName]->originalTextureRect.height * (1-percent));
                    heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
                                heldPercentSprites[atrName]->originalTextureRect.width,
                                heldPercentSprites[atrName]->originalTextureRect.height * percent));
                }
                break;
                case gui::Direction::down:
                {
                    heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
                                heldPercentSprites[atrName]->originalTextureRect.width,
                                heldPercentSprites[atrName]->originalTextureRect.height * percent));
                }
                break;
                case gui::Direction::left:
                {
                    heldPercentSprites[atrName]->rectOverlay.setOrigin(-1 * heldPercentSprites[atrName]->originalTextureRect.width * (1-percent), 0);
                    heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
                                heldPercentSprites[atrName]->originalTextureRect.width * percent,
                                heldPercentSprites[atrName]->originalTextureRect.height));
                }
                break;
                case gui::Direction::right:
                {
                    heldPercentSprites[atrName]->rectOverlay.setSize(sf::Vector2f(
                                heldPercentSprites[atrName]->originalTextureRect.width * percent,
                                heldPercentSprites[atrName]->originalTextureRect.height));
                }
                break;
                }
            }
        }
        break;

        case gui::ButtonAtrCharacteristic::transparency:
        {
            if(heldPercentSprites[atrName]->normalSprite.getTexture() != nullptr)
            {
                sf::Color newColor = heldPercentSprites[atrName]->normalSprite.getColor();
                float trans = static_cast <float> (value);
                trans = trans * 255 / 100;
                newColor.a = trans;
                heldPercentSprites[atrName]->normalSprite.setColor(newColor);
            }
            else
            {
                sf::Color newColor = heldPercentSprites[atrName]->rectOverlay.getFillColor();
                float trans = static_cast <float> (value);
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

void Button::setButtonAtr(std::string atrName, gui::ButtonAtrCharacteristic atrChar, char value)
{
    if(heldText.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::charSize:
            heldText[atrName]->text.setCharacterSize(value);
            break;
        default:
            break;
        }
    }
    else if(heldSprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::flip:
        {
            if (value == 'x')
            {
                heldSprites[atrName]->normalSprite.setTextureRect(sf::IntRect(heldSprites[atrName]->normalSprite.getLocalBounds().width,
                        0,
                        -1 * heldSprites[atrName]->normalSprite.getLocalBounds().width,
                        heldSprites[atrName]->normalSprite.getLocalBounds().height));
            }
            else if (value == 'y')
            {
                heldSprites[atrName]->normalSprite.setTextureRect(sf::IntRect(0,
                        heldSprites[atrName]->normalSprite.getLocalBounds().height,
                        heldSprites[atrName]->normalSprite.getLocalBounds().width,
                        -1 * heldSprites[atrName]->normalSprite.getLocalBounds().height));
            }
            else if (value == 'b')
            {
                heldSprites[atrName]->normalSprite.setTextureRect(sf::IntRect(heldSprites[atrName]->normalSprite.getLocalBounds().width,
                        heldSprites[atrName]->normalSprite.getLocalBounds().height,
                        -1 * heldSprites[atrName]->normalSprite.getLocalBounds().width,
                        -1 * heldSprites[atrName]->normalSprite.getLocalBounds().height));
            }
            else if (value == 0 || value == 'n')
            {
                heldSprites[atrName]->normalSprite.setTextureRect(sf::IntRect(0,
                        0,
                        heldSprites[atrName]->normalSprite.getLocalBounds().width,
                        heldSprites[atrName]->normalSprite.getLocalBounds().height));
            }
        }

        break;
        default:
            break;
        }

    }
    else if(heldOverlaySprites.count(atrName))
    {
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::transparency:
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
        switch (atrChar)
        {
        case gui::ButtonAtrCharacteristic::direction:
        {
            if(heldPercentSprites[atrName]->normalSprite.getTexture() != nullptr)
            {
                if(value == 'u')
                {
                    heldPercentSprites[atrName]->directionOfGap = gui::Direction::up;

                    heldPercentSprites[atrName]->normalSprite.setOrigin(0, heldPercentSprites[atrName]->normalSprite.getLocalBounds().height);
                    heldPercentSprites[atrName]->normalSprite.setScale(heldPercentSprites[atrName]->normalSprite.getScale().x,heldPercentSprites[atrName]->normalSprite.getScale().y * -1);

                    heldPercentSprites[atrName]->originalTextureRect = sf::IntRect(
                                0,
                                heldPercentSprites[atrName]->normalSprite.getLocalBounds().height,
                                heldPercentSprites[atrName]->normalSprite.getLocalBounds().width,
                                -1 * heldPercentSprites[atrName]->normalSprite.getLocalBounds().height);
                }
                else if(value == 'd')
                {
                    heldPercentSprites[atrName]->directionOfGap = gui::Direction::down;

                    heldPercentSprites[atrName]->originalTextureRect = sf::IntRect(
                                0,
                                0,
                                heldPercentSprites[atrName]->normalSprite.getLocalBounds().width,
                                heldPercentSprites[atrName]->normalSprite.getLocalBounds().height);

                }
                else if(value == 'l')
                {
                    heldPercentSprites[atrName]->directionOfGap = gui::Direction::left;

                    heldPercentSprites[atrName]->normalSprite.setOrigin(heldPercentSprites[atrName]->normalSprite.getLocalBounds().width, 0);
                    heldPercentSprites[atrName]->normalSprite.setScale(heldPercentSprites[atrName]->normalSprite.getScale().x * -1,heldPercentSprites[atrName]->normalSprite.getScale().y);

                    heldPercentSprites[atrName]->originalTextureRect = sf::IntRect(
                                heldPercentSprites[atrName]->normalSprite.getLocalBounds().width,
                                0,
                                -1 * heldPercentSprites[atrName]->normalSprite.getLocalBounds().width,
                                heldPercentSprites[atrName]->normalSprite.getLocalBounds().height);
                }
                else if(value == 'r')
                {
                    heldPercentSprites[atrName]->directionOfGap = gui::Direction::right;

                    heldPercentSprites[atrName]->originalTextureRect = sf::IntRect(
                                0,
                                0,
                                heldPercentSprites[atrName]->normalSprite.getLocalBounds().width,
                                heldPercentSprites[atrName]->normalSprite.getLocalBounds().height);
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
                    heldPercentSprites[atrName]->directionOfGap = gui::Direction::up;
                }
                else if(value == 'd')
                {
                    heldPercentSprites[atrName]->directionOfGap = gui::Direction::down;
                }
                else if(value == 'l')
                {
                    heldPercentSprites[atrName]->directionOfGap = gui::Direction::left;
                }
                else if(value == 'r')
                {
                    heldPercentSprites[atrName]->directionOfGap = gui::Direction::right;
                }
            }
        }
        default:
            break;
        }
    }
}

void Button::addButtonAtr (std::string atrName, gui::ButtonAtr buttonAtr)
{
    switch(buttonAtr)
    {
    case gui::ButtonAtr::Sprite:
    {
        RegularSprite* newSprite = new RegularSprite;
        newSprite->isChanged = true;
        newSprite->position = std::make_pair(0,0);
        newSprite->normalSprite.setPosition(newSprite->position.first  + buttonPosition.first,
                                            newSprite->position.second + buttonPosition.second);

        heldSprites[atrName] = newSprite;
    }
    break;
    case gui::ButtonAtr::Text:
    {
        ButtonText* newText = new ButtonText;
        newText->isChanged = true;
        newText->position = std::make_pair(0,0);
        newText->text.setFont(buttonFont);
        newText->text.setString(std::string());
        newText->text.setColor(sf::Color::Black);
        newText->text.setPosition(newText->position.first  + buttonPosition.first,
                                  newText->position.second + buttonPosition.second);
        heldText[atrName] = newText;
    }
    break;
    case gui::ButtonAtr::Hover:
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
    case gui::ButtonAtr::Percent:
    {
        PercentSprite* newPercent = new PercentSprite;
        newPercent->isChanged = true;
        newPercent->position = std::make_pair(0,0);
        newPercent->spritePercentage = 1.f;
        newPercent->directionOfGap = gui::Direction::right;
        newPercent->normalSprite.setPosition(newPercent->position.first  + buttonPosition.first,
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

void Button::update(std::pair <int, int> mouseCoords)
{
    for(std::map<std::string, OverlaySprite*>::iterator it = heldOverlaySprites.begin(); it != heldOverlaySprites.end(); it++)
    {
        if(mouseCoords.first >  buttonPosition.first + scrollAmount_x && mouseCoords.first  < buttonPosition.first + scrollAmount_x  + buttonBounds.first &&
                mouseCoords.second > buttonPosition.second + scrollAmount_y && mouseCoords.second < buttonPosition.second + scrollAmount_y + buttonBounds.second)
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

void Button::drawButton()
{
    if(isVisible)
    {
        for(std::map<std::string, RegularSprite*>::iterator it = heldSprites.begin(); it != heldSprites.end(); it++)
        {
            if(it->second->isChanged || isCoordsChanged)
            {
                it->second->normalSprite.setPosition(buttonPosition.first  + it->second->position.first  + scrollAmount_x,
                                                     buttonPosition.second + it->second->position.second + scrollAmount_y);
                it->second->isChanged = false;
                window.draw(it->second->normalSprite);
            }
            else
            {
                window.draw(it->second->normalSprite);
            }
        }
        for(std::map<std::string, ButtonText*>::iterator it = heldText.begin(); it != heldText.end(); it++)
        {
            if(it->second->isChanged || isCoordsChanged)
            {
                it->second->text.setPosition(buttonPosition.first  + it->second->position.first  + scrollAmount_x,
                                             buttonPosition.second + it->second->position.second + scrollAmount_y);
                it->second->isChanged = false;

                window.draw(it->second->text);
            }
            else
            {
                window.draw(it->second->text);
            }
        }

        for(std::map<std::string, OverlaySprite*>::iterator it = heldOverlaySprites.begin(); it != heldOverlaySprites.end(); it++)
        {
            if(it->second->isChanged || isCoordsChanged)
            {
                it->second->rectOverlay.setPosition(buttonPosition.first  + it->second->position.first  + scrollAmount_x,
                                                    buttonPosition.second + it->second->position.second + scrollAmount_y);
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
                if(it->second->normalSprite.getTexture() != nullptr)
                {
                    it->second->normalSprite.setPosition(buttonPosition.first  + it->second->position.first  + scrollAmount_x,
                                                         buttonPosition.second + it->second->position.second + scrollAmount_y);
                    window.draw(it->second->normalSprite);
                }
                else
                {
                    it->second->rectOverlay.setPosition(buttonPosition.first  + it->second->position.first  + scrollAmount_x,
                                                        buttonPosition.second + it->second->position.second + scrollAmount_y);
                    window.draw(it->second->rectOverlay);
                }
                it->second->isChanged = false;
            }
            else
            {
                if(it->second->normalSprite.getTexture() != nullptr)
                {
                    window.draw(it->second->normalSprite);
                }
                else
                {
                    window.draw(it->second->rectOverlay);
                }
            }
        }
        isCoordsChanged = false;
    }
}
