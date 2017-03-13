#include "QuestiaEng/Utl/Controls/MouseListener.h"

MouseListener::MouseListener()
{
    //ctor
}

void MouseListener::update()
{
    bool new_isMouseHeld_l= sf::Mouse::isButtonPressed(sf::Mouse::Left);
    bool new_isMouseHeld_m = sf::Mouse::isButtonPressed(sf::Mouse::Right);
    bool new_isMouseHeld_r = sf::Mouse::isButtonPressed(sf::Mouse::Middle);
    bool new_isMouseHeld_1 = sf::Mouse::isButtonPressed(sf::Mouse::XButton1);
    bool new_isMouseHeld_2 = sf::Mouse::isButtonPressed(sf::Mouse::XButton2);

    isMouseReleased_l = new_isMouseHeld_l ? false : isMouseHeld_l;
    isMouseReleased_m = new_isMouseHeld_m ? false : isMouseHeld_m;
    isMouseReleased_r = new_isMouseHeld_r ? false : isMouseHeld_r;
    isMouseReleased_1 = new_isMouseHeld_1 ? false : isMouseHeld_1;
    isMouseReleased_2 = new_isMouseHeld_2 ? false : isMouseHeld_2;

    isMousePressed_l = new_isMouseHeld_l && !isMouseHeld_l;
    isMousePressed_m = new_isMouseHeld_m && !isMouseHeld_m;
    isMousePressed_r = new_isMouseHeld_r && !isMouseHeld_r;
    isMousePressed_1 = new_isMouseHeld_1 && !isMouseHeld_1;
    isMousePressed_2 = new_isMouseHeld_2 && !isMouseHeld_2;

    isMouseHeld_l = new_isMouseHeld_l;
    isMouseHeld_m = new_isMouseHeld_m;
    isMouseHeld_r = new_isMouseHeld_r;
    isMouseHeld_1 = new_isMouseHeld_1;
    isMouseHeld_2 = new_isMouseHeld_2;
}

bool MouseListener::isMouseHeld(sf::Mouse::Button button)
{
    switch(button)
    {
    case sf::Mouse::Button::Left:
        return isMouseHeld_l;
        break;
    case sf::Mouse::Button::Right:
        return isMouseHeld_r;
        break;
    case sf::Mouse::Button::Middle:
        return isMouseHeld_m;
        break;
    case sf::Mouse::Button::XButton1:
        return isMouseHeld_1;
        break;
    case sf::Mouse::Button::XButton2:
        return isMouseHeld_2;
        break;
    }
}

bool MouseListener::isMousePressed(sf::Mouse::Button button)
{
    switch(button)
    {
    case sf::Mouse::Button::Left:
        return isMousePressed_l;
        break;
    case sf::Mouse::Button::Right:
        return isMousePressed_r;
        break;
    case sf::Mouse::Button::Middle:
        return isMousePressed_m;
        break;
    case sf::Mouse::Button::XButton1:
        return isMousePressed_1;
        break;
    case sf::Mouse::Button::XButton2:
        return isMousePressed_2;
        break;
    }
}

bool MouseListener::isMouseReleased(sf::Mouse::Button button)
{
    switch(button)
    {
    case sf::Mouse::Button::Left:
        return isMouseReleased_l;
        break;
    case sf::Mouse::Button::Right:
        return isMouseReleased_r;
        break;
    case sf::Mouse::Button::Middle:
        return isMouseReleased_m;
        break;
    case sf::Mouse::Button::XButton1:
        return isMouseReleased_1;
        break;
    case sf::Mouse::Button::XButton2:
        return isMouseReleased_2;
        break;
    }
}
