#include "QuestiaEng/Utl/Controls/MouseListener.h"

MouseListener::MouseListener()
{
	//ctor
}

void MouseListener::update()
{
	bool new_isMouseHeld_l = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	bool new_isMouseHeld_m = sf::Mouse::isButtonPressed(sf::Mouse::Middle);
	bool new_isMouseHeld_r = sf::Mouse::isButtonPressed(sf::Mouse::Right);
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
		break;
	case ctr::Input::RMouse:
		return isMousePressed_r;
		break;
	case ctr::Input::MMouse:
		return isMousePressed_m;
		break;
	case ctr::Input::Macro1:
		return isMousePressed_1;
		break;
	case ctr::Input::Macro2:
		return isMousePressed_2;
		break;
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
		break;
	case ctr::Input::RMouse:
		return isMouseReleased_r;
		break;
	case ctr::Input::MMouse:
		return isMouseReleased_m;
		break;
	case ctr::Input::Macro1:
		return isMouseReleased_1;
		break;
	case ctr::Input::Macro2:
		return isMouseReleased_2;
		break;
	default:
		break;
	}
	//TODO log error
	return false;
}
