#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <memory>

#include "SFML/Window/Keyboard.hpp"
#include "SFML/Graphics.hpp"

//forward declarations
class Engine;
class StateManager;

class State
{
public:
	State(std::shared_ptr<void> params = nullptr);
	virtual ~State();

	virtual void init() = 0;
	virtual void processInput(std::u32string const& inputText) = 0;
	virtual void update(sf::Time elapsedTime) = 0;
	virtual void displayTextures() = 0;

protected:
	template<class ParameterStruct, class... Args>
	static std::shared_ptr<void> makeParams(Args&&... args)
	{
		std::shared_ptr<ParameterStruct> params(new ParameterStruct{std::forward<Args>(args)...});
		std::shared_ptr<void> ret = std::static_pointer_cast<void>(params);
		return ret;
	}
	
	template<class ParameterStruct>
	ParameterStruct getParameters()
	{
		ParameterStruct ret = *(reinterpret_cast<ParameterStruct*>((pParams).get()));
		pParams.reset();
		return ret;
	}

	Engine* eng = nullptr;
	
private:
	std::shared_ptr<void> pParams;
	friend class StateManager;	
};

#endif // STATE_H
