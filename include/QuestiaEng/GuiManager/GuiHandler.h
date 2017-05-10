#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#include <memory>
#include <string>

#include "QuestiaEng/GuiManager/GuiElement.h"
#include "QuestiaEng/GuiManager/TabBar.h"

class GuiHandler
{
public:
	GuiHandler();
	~GuiHandler();

	void reg(GuiElement* guiElement);
	void clear();

private:
	std::vector<std::unique_ptr<GuiElement> > elements;
	std::
};

#endif // GUIHANDLER_H
