#include "QuestiaEng/GuiManager/QueryWindow.h"

QueryWindow::QueryWindow()
{

}

QueryWindow::~QueryWindow()
{
	//dtor
}

void QueryWindow::addQuery(std::string queryID, std::string queryQuestion, QueryType queryType)
{
	queryValues.push_back(std::make_pair(queryID, Query(queryQuestion, queryType)));
}

void QueryWindow::init(std::string windowName,GuiManager& pGuiManager, GuiLoader& guiLoader, std::function<void()> handle)
{
	guiManager = &pGuiManager;
	guiLoader.loadGui(pGuiManager, "queryWindow");

	handleRes = handle;

	for(unsigned int i = 0; i < queryValues.size(); i++)
	{
		windowSize_y += 40;
		windowPos_y -= 20;
	}

	pGuiManager.createGroupFromTemplate(windowName, "queryWindowTemplate");
	groupName = windowName;
	button_windowBox   = pGuiManager.getGroupEntry(windowName, "windowBox");
	button_closeButton = pGuiManager.getGroupEntry(windowName, "windowClose");
	pGuiManager.setButtonAtr(button_windowBox, "windowSprite", gui::ButtonAtrCharacteristic::size, std::make_pair(windowSize_x, windowSize_y));

	//add queries
	for(unsigned int it = 0; it != queryValues.size(); it++)
	{
		switch(queryValues.at(it).second.queryType)
		{
		case QueryType::Input_int:
		case QueryType::Input_string:
			queryValues.at(it).second.buttonName = groupName + "_" + utl::asString(it);
			pGuiManager.createButton(queryValues.at(it).second.buttonName, "textInputBox");
			pGuiManager.setButton(queryValues.at(it).second.buttonName, gui::ButtonCharacteristic::coords, std::make_pair(250, 60 + it*40));
			pGuiManager.setButtonAtr(queryValues.at(it).second.buttonName, "buttonText", gui::ButtonAtrCharacteristic::text, queryValues.at(it).second.queryQuestion);
			pGuiManager.addToGroup(groupName, queryValues.at(it).second.buttonName);
			break;
		case QueryType::Choice_string:
			queryValues.at(it).second.buttonName = groupName + "_" + utl::asString(it);
			pGuiManager.createButton(queryValues.at(it).second.buttonName, "windowChoice");
			pGuiManager.setButton(queryValues.at(it).second.buttonName, gui::ButtonCharacteristic::coords, std::make_pair(100, 60 + it*40));
			pGuiManager.setButtonAtr(queryValues.at(it).second.buttonName, "buttonTextAnswer", gui::ButtonAtrCharacteristic::text, queryValues.at(it).second.queryQuestion);
			pGuiManager.addToGroup(groupName, queryValues.at(it).second.buttonName);
			break;
		}
	}

	//add title
	pGuiManager.createButton(groupName + "_title", "windowTitle");
	pGuiManager.setButtonAtr(groupName + "_title", "buttonText", gui::ButtonAtrCharacteristic::text, windowName);
	pGuiManager.addToGroup(groupName, groupName + "_title");

	//add submit button
	button_submitButton = groupName + "_submit";
	pGuiManager.createButton(button_submitButton, "windowSubmit");
	pGuiManager.setButton(button_submitButton, gui::ButtonCharacteristic::coords, std::make_pair(350-25, windowSize_y - 35));
	pGuiManager.addToGroup(groupName, button_submitButton);

	//choice - when more than 24 options add scrolling and move OK button
	if(queryValues.size() > 24)
	{
		pGuiManager.setButton(button_submitButton, gui::ButtonCharacteristic::coords, std::make_pair(700-50-10, 1060-35));
		pGuiManager.setButtonAtr(groupName + "_title", "buttonText", gui::ButtonAtrCharacteristic::text, "");
		scrollWindow = true;
	}

	//set position and not visible
	pGuiManager.setGroupAtr(gui::ButtonCharacteristic::coords_group, std::make_pair(windowPos_x, windowPos_y));
	pGuiManager.setGroupAtr(groupName, gui::ButtonCharacteristic::isVisible, false);
}

void QueryWindow::postAddChoice(std::string queryID, std::string choice)
{
	queryValues.push_back(std::make_pair(queryID, Query(choice, QueryType::Choice_string)));

	int it = queryValues.size() - 1;
	//make the button
	queryValues.back().second.buttonName = groupName + "_" + std::to_string(it);
	guiManager->createButton(queryValues.at(it).second.buttonName, "windowChoice");
	guiManager->setButton(queryValues.at(it).second.buttonName, gui::ButtonCharacteristic::coords, std::make_pair(100, 60 + it*40));
	guiManager->setButtonAtr(queryValues.at(it).second.buttonName, "buttonTextAnswer", gui::ButtonAtrCharacteristic::text, queryValues.at(it).second.queryQuestion);
	guiManager->addToGroup(groupName, queryValues.at(it).second.buttonName);

	guiManager->setGroupAtr(groupName, gui::ButtonCharacteristic::isVisible, false);
}

int QueryWindow::getResult_int(std::string queryID)
{
	for(auto& query : queryValues)
	{
		if(query.first == queryID)
		{
			return utl::asInt(query.second.queryAnswer);
		}
	}
	return 0;
}

std::string QueryWindow::getResult_string(std::string queryID)
{
	for(auto& query : queryValues)
	{
		if(query.first == queryID)
		{
			std::string returnStr = query.second.queryAnswer;
			std::replace(returnStr.begin(), returnStr.end(), ' ', '_');
			return returnStr;
		}
	}
	return std::string();
}

std::string QueryWindow::getChoice_string()
{
	if(querySelection != -1)
	{
		return queryValues.at(querySelection).second.queryQuestion;
	}
	return "";
}

void QueryWindow::setActive(bool isActive)
{
	if(isActive != isWindowActive)
	{
		isWindowActive = isActive;
		guiManager->setGroupAtr(groupName, gui::ButtonCharacteristic::isVisible, isActive);
	}
}
bool QueryWindow::isActive()
{
	return isWindowActive;
}

bool QueryWindow::isDone()
{
	return isWindowDone;
}
bool QueryWindow::isWindow(std::string question)
{
	return (question == groupName);
}

void QueryWindow::resetQueries()
{
	querySelection = -1;
	for(auto& queryPair : queryValues)
	{
		Query& selectedQuery = queryPair.second;

		switch(selectedQuery.queryType)
		{
		case QueryType::Input_int:
		case QueryType::Input_string:
			selectedQuery.queryAnswer.clear();
			guiManager->setButtonAtr(selectedQuery.buttonName, "buttonTextAnswer", gui::ButtonAtrCharacteristic::text, selectedQuery.queryAnswer);
			break;
		case QueryType::Choice_string:
			guiManager->setButtonAtr(selectedQuery.buttonName, "buttonHover", gui::ButtonAtrCharacteristic::percentage, 0);
			break;
		}
	}
}

void QueryWindow::handleInput(std::u32string& input)
{
	//text input and deletion
	if(querySelection != -1 && input.size() > 0 && input[0] != ' ')
	{
		Query& selectedQuery = queryValues.at(querySelection).second;
		if(selectedQuery.queryType == QueryType::Input_int || selectedQuery.queryType == QueryType::Input_string)
		{
			if(input[0] == 8)
			{
				if(selectedQuery.queryAnswer.size() > 0)
				{
					selectedQuery.queryAnswer.erase(selectedQuery.queryAnswer.end()-1, selectedQuery.queryAnswer.end());
				}
			}
			else
			{
				switch(selectedQuery.queryType)
				{
				case QueryType::Input_int:
					{
						if(utl::isNumber(std::to_string(input[0])) && input[0] != '-')
						{
							selectedQuery.queryAnswer += input[0];
						}
					}
					break;
				case QueryType::Input_string:
					selectedQuery.queryAnswer += input[0];
					break;
				default:
					break;
				}

			}
			guiManager->setButtonAtr(selectedQuery.buttonName, "buttonTextAnswer", gui::ButtonAtrCharacteristic::text, selectedQuery.queryAnswer);
		}
	}
	//TODO more elegant scrolling solution
	if(scrollWindow)
	{
		if(input[0] == 'w')
		{
			scrollAmount += 10;
			for(auto& queryPair : queryValues)
			{
				Query& selectedQuery = queryPair.second;
				guiManager->setButton(selectedQuery.buttonName, gui::ButtonCharacteristic::scroll_y, scrollAmount);
			}
		}
		if(input[0] == 's')
		{
			scrollAmount -= 10;
			for(auto& queryPair : queryValues)
			{
				Query& selectedQuery = queryPair.second;
				guiManager->setButton(selectedQuery.buttonName, gui::ButtonCharacteristic::scroll_y, scrollAmount);
			}
		}
	}
}

void QueryWindow::update(MouseListener& mouse)
{
	if(mouse.isMouseReleased(ctr::Input::LMouse))
	{
		if(guiManager->isClicked(button_closeButton))
		{
			setActive(false);
			resetQueries();
		}
		else if(guiManager->isClicked(button_submitButton))
		{
			bool isDone = true;
			bool hasChoice = false;
			for(auto& queryPair : queryValues)
			{
				Query& selectedQuery = queryPair.second;
				switch(selectedQuery.queryType)
				{
				case QueryType::Input_int:
				case QueryType::Input_string:
					{
						if(selectedQuery.queryAnswer.size() < 1)
						{
							isDone = false;
						}
					}
					break;
				case QueryType::Choice_string:
					hasChoice = true;
					break;
				}

			}
			if(isDone)
			{
				if(!hasChoice || (hasChoice && querySelection != -1))
				{
					setActive(false);
					isWindowDone = true;
					handleRes();
				}
			}
			else
			{
				isWindowDone = false;
			}
		}
		else
		{
			for(unsigned int it = 0; it != queryValues.size(); it++)
			{
				if(guiManager->isClicked(queryValues.at(it).second.buttonName))
				{
					querySelection = it;

					//highlight selected query for choice
					if(queryValues.at(it).second.queryType == QueryType::Choice_string)
					{
						for(auto& queryPair : queryValues)
						{
							Query& selectedQuery = queryPair.second;
							if(selectedQuery.queryType == QueryType::Choice_string)
							{
								guiManager->setButtonAtr(selectedQuery.buttonName, "buttonHover", gui::ButtonAtrCharacteristic::percentage, 0);
							}
						}
						guiManager->setButtonAtr(queryValues.at(it).second.buttonName, "buttonHover", gui::ButtonAtrCharacteristic::percentage, 100);
					}
				}
			}
		}
	}
}
