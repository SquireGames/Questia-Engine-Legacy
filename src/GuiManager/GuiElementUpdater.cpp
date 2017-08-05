#include "QuestiaEng/GuiManager/GuiElementUpdater.h"

GuiElementUpdater::GuiElementUpdater(MouseListener& mouse):
	mouse(mouse)
{

}

GuiElementUpdater::~GuiElementUpdater()
{

}

void GuiElementUpdater::regMouseUpdates(GuiElement* guiElement)
{
	elements.push_back(guiElement);
}

void GuiElementUpdater::regTextUpdates(GuiElement* guiElement)
{
	inputs.push_back(guiElement);
}

void GuiElementUpdater::update(std::u32string& input)
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

void GuiElementUpdater::clear()
{
	elements.clear();
	inputs.clear();
}
