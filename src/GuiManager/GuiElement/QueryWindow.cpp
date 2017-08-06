#include "QuestiaEng/GuiManager/GuiElement/QueryWindow.h"

QueryWindow::QueryWindow()
{

}

QueryWindow::~QueryWindow()
{
	//dtor
}

void QueryWindow::addQuery(const std::string& queryID, const std::string& queryQuestion, QueryType queryType)
{
	queryValues.push_back(std::make_pair(queryID, Query(queryQuestion, queryType)));
}

void QueryWindow::init(const std::string& windowName, GuiManager& guiManager, std::function<void()> handle)
{
	this->guiManager = &guiManager;

	GuiBuilder& guiBuilder = guiManager.edit();

	guiManager.loadGui("queryWindow");

	handleRes = handle;

	for(unsigned int i = 0; i < queryValues.size(); i++)
	{
		windowSize_y += 40;
		windowPos_y -= 20;
	}

	guiBuilder.createGroupFromTemplate(windowName, "queryWindowTemplate");
	groupName = windowName;
	button_windowBox   = guiBuilder.getGroupEntryID(windowName, "windowBox");
	button_closeButton = guiBuilder.getGroupEntryID(windowName, "windowClose");
	guiBuilder.setBtnAtr(button_windowBox, "windowSprite", gui::BtnAtrChar::size, std::make_pair(windowSize_x, windowSize_y));

	//add queries
	for(unsigned int it = 0; it < queryValues.size(); it++)
	{
		switch(queryValues.at(it).second.queryType)
		{
		case QueryType::Input_int:
		case QueryType::Input_string:
			queryValues.at(it).second.buttonName = groupName + "-" + std::to_string(it);
			guiBuilder.createButton(queryValues.at(it).second.buttonName, "textInputBox");
			guiBuilder.setButton(queryValues.at(it).second.buttonName, gui::BtnChar::coords, std::make_pair(250, 60 + it*40));
			guiBuilder.setBtnAtr(queryValues.at(it).second.buttonName, "buttonText", gui::BtnAtrChar::text, queryValues.at(it).second.queryQuestion);
			guiBuilder.addToGroup(groupName, queryValues.at(it).second.buttonName);
			break;
		case QueryType::Choice_string:
			queryValues.at(it).second.buttonName = groupName + "-" + std::to_string(it);
			guiBuilder.createButton(queryValues.at(it).second.buttonName, "windowChoice");
			guiBuilder.setButton(queryValues.at(it).second.buttonName, gui::BtnChar::coords, std::make_pair(100, 60 + it*40));
			guiBuilder.setBtnAtr(queryValues.at(it).second.buttonName, "buttonTextAnswer", gui::BtnAtrChar::text, queryValues.at(it).second.queryQuestion);
			guiBuilder.addToGroup(groupName, queryValues.at(it).second.buttonName);
			break;
		}
	}

	//add title
	guiBuilder.createButton(groupName + "_title", "windowTitle");
	guiBuilder.setBtnAtr(groupName + "_title", "buttonText", gui::BtnAtrChar::text, windowName);
	guiBuilder.addToGroup(groupName, groupName + "_title");

	//add submit button
	button_submitButton = groupName + "_submit";
	guiBuilder.createButton(button_submitButton, "windowSubmit");
	guiBuilder.setButton(button_submitButton, gui::BtnChar::coords, std::make_pair(350-25, windowSize_y - 35));
	guiBuilder.addToGroup(groupName, button_submitButton);

	//choice - when more than 24 options add scrolling and move OK button
	if(queryValues.size() > 24)
	{
		guiBuilder.setButton(button_submitButton, gui::BtnChar::coords, std::make_pair(700-50-10, 1060-35));
		guiBuilder.setBtnAtr(groupName + "_title", "buttonText", gui::BtnAtrChar::text, "");
		scrollWindow = true;
	}

	//set position and not visible
	guiBuilder.setGroupAtr(gui::BtnChar::coords_group, std::make_pair(windowPos_x, windowPos_y));
	guiBuilder.setGroupAtr(groupName, gui::BtnChar::isVisible, false);
}

void QueryWindow::reInit()
{
	isWindowActive = false;
	isWindowDone = false;
	scrollWindow = false;
	scrollAmount = 0;
	windowSize_x = 700;
	windowSize_y = 100;
	windowPos_x =  610;
	windowPos_y =  500;
	querySelection = -1;

	guiManager->edit().deleteGroup(groupName);

	init(groupName, *guiManager, handleRes);
}

int QueryWindow::getResult_int(const std::string& queryID)
{
	for(auto& query : queryValues)
	{
		if(query.first == queryID)
		{
			return utl::toInt(query.second.queryAnswer);
		}
	}
	return 0;
}

std::string QueryWindow::getResult_string(const std::string& queryID)
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
		guiManager->edit().setGroupAtr(groupName, gui::BtnChar::isVisible, isActive);
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
bool QueryWindow::isWindow(const std::string& question)
{
	return (question == groupName);
}

void QueryWindow::resetQueries()
{
	GuiBuilder& guiBuilder = guiManager->edit();
	querySelection = -1;
	for(auto& queryPair : queryValues)
	{
		Query& selectedQuery = queryPair.second;

		switch(selectedQuery.queryType)
		{
		case QueryType::Input_int:
		case QueryType::Input_string:
			selectedQuery.queryAnswer.clear();
			guiBuilder.setBtnAtr(selectedQuery.buttonName, "buttonTextAnswer", gui::BtnAtrChar::text, selectedQuery.queryAnswer);
			break;
		case QueryType::Choice_string:
			guiBuilder.setBtnAtr(selectedQuery.buttonName, "buttonHover", gui::BtnAtrChar::percentage, 0);
			break;
		}
	}
}

void QueryWindow::handleInput(std::u32string& input)
{
	GuiBuilder& guiBuilder = guiManager->edit();
	//text input and deletion
	if(querySelection != -1 && input.size() > 0 && input[0] != ' ' && input[0] != 9)
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
						if(utl::isNumber(std::string(1, input[0])))
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
			guiBuilder.setBtnAtr(selectedQuery.buttonName, "buttonTextAnswer", gui::BtnAtrChar::text, selectedQuery.queryAnswer);
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
				guiBuilder.setButton(selectedQuery.buttonName, gui::BtnChar::scroll_y, scrollAmount);
			}
		}
		if(input[0] == 's')
		{
			scrollAmount -= 10;
			for(auto& queryPair : queryValues)
			{
				Query& selectedQuery = queryPair.second;
				guiBuilder.setButton(selectedQuery.buttonName, gui::BtnChar::scroll_y, scrollAmount);
			}
		}
	}
}

void QueryWindow::update(MouseListener& mouse)
{
	GuiBuilder& guiBuilder = guiManager->edit();
	if(mouse.isMousePressed(ctr::Input::LMouse))
	{
		if(guiManager->isHovered(button_closeButton))
		{
			setActive(false);
			resetQueries();
		}
		else if(guiManager->isHovered(button_submitButton))
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
				if(guiManager->isHovered(queryValues.at(it).second.buttonName))
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
								guiBuilder.setBtnAtr(selectedQuery.buttonName, "buttonHover", gui::BtnAtrChar::percentage, 0);
							}
						}
						guiBuilder.setBtnAtr(queryValues.at(it).second.buttonName, "buttonHover", gui::BtnAtrChar::percentage, 100);
					}
				}
			}
		}
	}
}
