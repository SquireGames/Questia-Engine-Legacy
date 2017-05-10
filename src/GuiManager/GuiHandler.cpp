#include "QuestiaEng/GuiManager/GuiHandler.h"

GuiHandler::GuiHandler(MouseListener& mou):
	mouse(mou)
{

}

GuiHandler::~GuiHandler()
{

}

void GuiHandler::reg(GuiElement* guiElement)
{
	elements.push_back(guiElement);
}

void GuiHandler::update()
{
	for(auto& element : elements)
	{
		element->update(mouse);
	}
}

void GuiHandler::clear()
{
	elements.clear();
}
