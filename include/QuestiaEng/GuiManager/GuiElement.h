#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include <string>

class GuiElement
{
public:
	GuiElement() = default;
	~GuiElement() = default;

	virtual void onClick();
	virtual void handleText(std::u32string input); 
	
	//TODO make pure virtual
	virtual bool hasID(int ID);
	virtual bool isClicked(int ID);
};

#endif // GUIELEMENT_H
