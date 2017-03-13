#ifndef ENGINE_H
#define ENGINE_H

#include "QuestiaEng/ResourceManager/ResourceManager.h"
#include "QuestiaEng/GuiManager/GuiManager.h"

class Engine
{
public:
	Engine();
	~Engine();

private:
	ResourceManager resourceManager;
};

#endif // ENGINE_H
