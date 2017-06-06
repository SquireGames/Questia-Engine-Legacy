#include "QuestiaEng/GuiManager/GuiHandler.h"

GuiHandler::GuiHandler(MouseListener& mouse):
	mouse(mouse)
{

}

GuiHandler::~GuiHandler()
{

}

void GuiHandler::reg(GuiElement* guiElement)
{
	elements.push_back(guiElement);
}

void GuiHandler::regInput(GuiElement* guiElement)
{
	inputs.push_back(guiElement);
}

void GuiHandler::update(std::u32string& input)
{
	for(auto& element : elements)
	{
		element->update(mouse);
	}
	for(auto& element : inputs)
	{
		element->handleInput(input);
	}
}

void GuiHandler::clear()
{
	elements.clear();
	inputs.clear();
}
