#include "QuestiaEng/Utl/Controls/MouseListener.h"

MouseListener::MouseListener():
	mousePos(0,0)
{
	//ctor
}

void MouseListener::update()
{
	isMouseHeld_l = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	isMouseHeld_m = sf::Mouse::isButtonPressed(sf::Mouse::Middle);
	isMouseHeld_r = sf::Mouse::isButtonPressed(sf::Mouse::Right);
	isMouseHeld_1 = sf::Mouse::isButtonPressed(sf::Mouse::XButton1);
	isMouseHeld_2 = sf::Mouse::isButtonPressed(sf::Mouse::XButton2);

	isMouseReleased_l = false;
	isMouseReleased_m = false;
	isMouseReleased_r = false;
	isMouseReleased_1 = false;
	isMouseReleased_2 = false;

	isMousePressed_l = false;
	isMousePressed_m = false;
	isMousePressed_r = false;
	isMousePressed_1 = false;
	isMousePressed_2 = false;
}

void MouseListener::update_pressed(sf::Mouse::Button mouse)
{
	switch(mouse)
	{
	case sf::Mouse::Left:
		isMousePressed_l = true;
		break;
	case sf::Mouse::Middle:
		isMousePressed_m = true;
		break;
	case sf::Mouse::Right:
		isMousePressed_r = true;
		break;
	case sf::Mouse::XButton1:
		isMousePressed_1 = true;
		break;
	case sf::Mouse::XButton2:
		isMousePressed_2 = true;
		break;
	default:
		break;
	}
}

void MouseListener::update_released(sf::Mouse::Button mouse)
{
	switch(mouse)
	{
	case sf::Mouse::Left:
		isMouseReleased_l = true;
		break;
	case sf::Mouse::Middle:
		isMouseReleased_m = true;
		break;
	case sf::Mouse::Right:
		isMouseReleased_r = true;
		break;
	case sf::Mouse::XButton1:
		isMouseReleased_1 = true;
		break;
	case sf::Mouse::XButton2:
		isMouseReleased_2 = true;
		break;
	default:
		break;
	}
}


bool MouseListener::isMouseHeld(ctr::Input button)
{
	switch(button)
	{
	case ctr::Input::LMouse:
		return isMouseHeld_l;
		break;
	case ctr::Input::RMouse:
		return isMouseHeld_r;
		break;
	case ctr::Input::MMouse:
		return isMouseHeld_m;
		break;
	case ctr::Input::Macro1:
		return isMouseHeld_1;
		break;
	case ctr::Input::Macro2:
		return isMouseHeld_2;
		break;
	default:
		break;
	}
	//TODO log error
	return false;
}

bool MouseListener::isMousePressed(ctr::Input button)
{
	switch(button)
	{
	case ctr::Input::LMouse:
		return isMousePressed_l;
	case ctr::Input::RMouse:
		return isMousePressed_r;
	case ctr::Input::MMouse:
		return isMousePressed_m;
	case ctr::Input::Macro1:
		return isMousePressed_1;
	case ctr::Input::Macro2:
		return isMousePressed_2;
	default:
		break;
	}
	//TODO log error
	return false;
}

bool MouseListener::isMouseReleased(ctr::Input button)
{
	switch(button)
	{
	case ctr::Input::LMouse:
		return isMouseReleased_l;
	case ctr::Input::RMouse:
		return isMouseReleased_r;
	case ctr::Input::MMouse:
		return isMouseReleased_m;
	case ctr::Input::Macro1:
		return isMouseReleased_1;
	case ctr::Input::Macro2:
		return isMouseReleased_2;
	default:
		break;
	}
	//TODO log error
	return false;
}
