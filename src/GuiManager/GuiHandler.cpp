#include "QuestiaEng/GuiManager/GuiHandler.h"

GuiHandler::GuiHandler()
{

}

GuiHandler::~GuiHandler()
{

}

void GuiHandler::reg(GuiElement* guiElement)
{
	elements.push_back(std::unique_ptr<GuiElement>(guiElement));
}

void GuiHandler::clear()
{
	elements.clear();
}
