#include "QuestiaEng/GuiManager/GuiLoader.h"

#include "QuestiaEng/GuiManager/GuiManager.h"

#include "QuestiaEng/Utl/Logger.h"

GuiLoader::GuiLoader(GuiManager& guiManager) noexcept:
	guiManager(guiManager)
{
	//ctor
}

void GuiLoader::setGuiPack(const std::string& guiPack) noexcept
{
	primaryPack = guiPack;
}

void GuiLoader::loadGui(const std::string& gui) noexcept
{
	GuiBuilder& guiBuilder = guiManager.edit();

	if(isLoadedGui(gui))
	{
		return;
	}
	loadedGuis.push_back(gui);

	std::cout << "Loading GUI: " << gui << std::endl;

	std::string filePath = "Data/Gui Pack/" + primaryPack + "/" + gui + ".txt";
	if(!utl::doesExist(filePath))
	{
		filePath = "Data/Gui Pack/" + secondaryPack + "/" + gui + ".txt";
	}

	guiFile.setFilePath(filePath);

	if(!guiFile.readFile('^'))
	{
		LOG("FATAL ERROR - Guiloader failed to read file path:" + filePath);
		return;
	}
	else
	{
		std::vector<std::string> guiCommands = guiFile.getItemKeyList();

		for(unsigned int it = 0; it != guiCommands.size(); it++)
		{
			std::vector<std::string> command = utl::separateString(guiCommands[it], '|');
			const unsigned int& commands = command.size();

			//change to language pack text if necessary
			bool isUniText = false;
			std::u32string uniText;
			if(commands == 6 || commands == 4)
			{
				std::string& buttonAtr = (commands == 6) ? command[4] : command[2];
				std::string& val = (commands == 6) ? command[5] : command[3];

				if(buttonAtr == "Text" && val.size() >= 3 && val.substr(0,2) == "T_")
				{
					uniText = guiBuilder.getText(val);
					isUniText = true;
				}
			}

			if(commands >= 3)
			{
				if(command[0] == "create")
				{
					if(command[1] == "Button")
					{
						if(commands == 3)
						{
							//create button name
							guiBuilder.createButton(command[2]);
						}
						else if(commands == 4)
						{
							if(utl::isNumber(command[3]))
							{
								//create button name layer
								guiBuilder.createButton(command[2], utl::toInt(command[3]));
							}
							else
							{
								//create button copy original
								guiBuilder.createButton(command[2], command[3]);
							}

						}
					}
					else if(command[1] == "ButtonTemplate")
					{
						if(commands == 3)
						{
							//create button-template name
							guiBuilder.createButtonTemplate(command[2]);
						}
						else if(commands == 4)
						{
							if(utl::isNumber(command[3]))
							{
								//create button-template name layer
								guiBuilder.createButtonTemplate(command[2], utl::toInt(command[3]));
							}
							else
							{
								//create button-template copy original
								guiBuilder.createButtonTemplate(command[2], command[3]);
							}

						}
					}
					else if(command[1] == "ButtonAtr")
					{
						if(commands == 4)
						{
							//create buttonAtr atrName atrType
							guiBuilder.createBtnAtr(command[2], getBtnAtr(command[3]));
						}
						else if(commands == 5)
						{
							//create buttonAtr buttonName atrName atrType
							guiBuilder.createBtnAtr(command[2], command[3],  getBtnAtr(command[4]));
						}
					}
					else if(command[1] == "Group")
					{
						//create group groupName
						guiBuilder.createGroup(command[2]);
					}
					else if(command[1] == "GroupTemplate")
					{
						//create group-template template-name
						guiBuilder.createGroupTemplate(command[2]);
					}
					else if(command[1] == "GroupFromTemplate")
					{
						if(commands == 4)
						{
							//create groupFromTemplate groupName templateName
							guiBuilder.createGroupFromTemplate(command[2], command[3]);
						}
					}
					else if(command[1] == "List")
					{
						//create list listName
						guiBuilder.createList(command[2]);
					}
					else if(command[1] == "ListEntry")
					{
						//create listEntry listName
						guiBuilder.createListEntry(command[2]);
					}
				}
				else if(command[0] == "set")
				{
					if(command[1] == "ButtonLayer")
					{
						if(commands == 3)
						{
							//set ButtonLayer Layer
							guiBuilder.setButtonLayer(utl::toInt(command[2]));
						}
						else if(commands == 4)
						{
							//set ButtonLayer Button Layer
							guiBuilder.setButtonLayer(command[2], utl::toInt(command[3]));
						}
					}
					else if(command[1] == "ListSpacing")
					{
						if(commands == 4)
						{
							//set ListSpacing Spacing
							guiBuilder.setListSpacing(utl::toInt(command[2]), utl::toInt(command[3]));
						}
						else if(commands == 5)
						{
							//set ListSpacing List Spacing
							guiBuilder.setListSpacing(command[2], utl::toInt(command[3]), utl::toInt(command[4]));
						}
					}
					else if(command[1] == "ListTemplate")
					{
						if(commands == 3)
						{
							//set ListTemplate Template
							guiBuilder.setListTemplate(command[2]);
						}
						else if(commands == 4)
						{
							//set ListTemplate List Template
							guiBuilder.setListTemplate(command[2], command[3]);
						}
					}
					else if(command[1] == "ListPosition")
					{
						if(commands == 4)
						{
							//set ListPosition x y
							guiBuilder.setListPosition(std::make_pair(utl::toInt(command[2]), utl::toInt(command[3])));
						}
						else if(commands == 5)
						{
							//set ListPosition List x y
							guiBuilder.setListPosition(command[2], std::make_pair(utl::toInt(command[3]), utl::toInt(command[4])));
						}
					}
					else if(command[1] == "Button")
					{
						if(commands == 4)
						{
							if(utl::isNumber(command[3]))
							{
								//set Button buttonCharacteristic int
								guiBuilder.setButton(getBtnChar(command[2]), utl::toInt(command[3]));
							}
							else if(getBtnChar(command[2]) == gui::BtnChar::isVisible)
							{
								//set Button buttonCharacteristic bool
								if(command[3] == "t")
								{
									guiBuilder.setButton(getBtnChar(command[2]), true);
								}
								else
								{
									guiBuilder.setButton(getBtnChar(command[2]), false);
								}
							}
							else
							{
								//set Button buttonCharacteristic string
								guiBuilder.setButton(getBtnChar(command[2]), command[3]);
							}
						}
						else if(commands == 5)
						{
							if(utl::isNumber(command[3]) && utl::isNumber(command[4]))
							{
								//set Button buttonCharacteristic int int
								guiBuilder.setButton(getBtnChar(command[2]), std::make_pair(utl::toInt(command[3]), utl::toInt(command[4])));
							}
							else if(utl::isNumber(command[4]))
							{
								//set Button buttonName buttonCharacteristic int
								guiBuilder.setButton(command[2], getBtnChar(command[3]), utl::toInt(command[4]));
							}
							else if(getBtnChar(command[3]) == gui::BtnChar::isVisible)
							{
								//set Button buttonName buttonCharacteristic bool
								if(command[4] == "t")
								{
									guiBuilder.setButton(command[2],getBtnChar(command[3]), true);
								}
								else
								{
									guiBuilder.setButton(command[2],getBtnChar(command[3]), false);
								}
							}
							else
							{
								//set Button ButtonName buttonCharacteristic String
								guiBuilder.setButton(command[2],getBtnChar(command[3]), command[4]);
							}
						}
						else if(commands == 6)
						{
							if(utl::isNumber(command[4]) && utl::isNumber(command[5]))
							{
								//set Button ButtonName buttonCharacteristic int int
								guiBuilder.setButton(command[2],getBtnChar(command[3]), std::make_pair(utl::toInt(command[4]), utl::toInt(command[5])));
							}
						}
					}
					else if(command[1] == "ButtonAtr")
					{
						if(commands == 4)
						{
							if(utl::isNumber(command[3]))
							{
								//set BtnAtr AtrCharacteristic int
								guiBuilder.setBtnAtr(getBtnAtrChar(command[2]), utl::toInt(command[3]));
							}
							else if(getBtnChar(command[2]) == gui::BtnChar::isVisible)
							{
								//set BtnAtr AtrCharacteristic bool
								if(command[3] == "t")
								{
									guiBuilder.setBtnAtr(getBtnAtrChar(command[2]), true);
								}
								else
								{
									guiBuilder.setBtnAtr(getBtnAtrChar(command[2]), false);
								}
							}
							else if(getBtnAtrChar(command[2]) == gui::BtnAtrChar::flip || getBtnAtrChar(command[2]) == gui::BtnAtrChar::direction)
							{
								//set BtnAtr AtrCharacteristic(flip) char
								guiBuilder.setBtnAtr(getBtnAtrChar(command[2]), command[3].at(0));
							}
							else
							{
								//set BtnAtr AtrCharacteristic string
								if(!isUniText)
								{
									guiBuilder.setBtnAtr(getBtnAtrChar(command[2]), command[3]);
								}
								else
								{
									guiBuilder.setBtnAtr(getBtnAtrChar(command[2]), uniText);
								}

							}
						}
						else if(commands == 5)
						{
							if(utl::isNumber(command[3]) && utl::isNumber(command[4]))
							{
								//set BtnAtr AtrCharacteristic int int
								guiBuilder.setBtnAtr(getBtnAtrChar(command[2]), std::make_pair(utl::toInt(command[3]), utl::toInt(command[4])));
							}
						}
						else if(commands == 6)
						{
							//set BtnAtr AtrCharacteristic int int int
							if(utl::isNumber(command[3]) && utl::isNumber(command[4]) && utl::isNumber(command[5]))
							{
								guiBuilder.setBtnAtr(getBtnAtrChar(command[2]), sf::Color(utl::toInt(command[3]), utl::toInt(command[4]),utl::toInt(command[5])));
							}
							else if(utl::isNumber(command[5]))
							{
								//set BtnAtr ButtonName AtrName AtrCharacteristic int
								guiBuilder.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), utl::toInt(command[5]));
							}
							else if(getBtnChar(command[4]) == gui::BtnChar::isVisible)
							{
								//set BtnAtr ButtonName AtrName AtrCharacteristic bool
								if(command[5] == "t")
								{
									guiBuilder.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), true);
								}
								else
								{
									guiBuilder.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), false);
								}
							}
							else if(getBtnAtrChar(command[4]) == gui::BtnAtrChar::flip || getBtnAtrChar(command[4]) == gui::BtnAtrChar::direction)
							{
								//set BtnAtr ButtonName AtrName  AtrCharacteristic(flip) char
								guiBuilder.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), command[5].at(0));
							}
							else
							{
								//set BtnAtr ButtonName AtrName AtrCharacteristic string
								if(!isUniText)
								{
									guiBuilder.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), command[5]);
								}
								else
								{
									guiBuilder.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), uniText);
								}

							}
						}
						else if(commands == 7)
						{
							if(utl::isNumber(command[5]) && utl::isNumber(command[6]))
							{
								////set BtnAtr ButtonName AtrName AtrCharacteristic int int
								guiBuilder.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), std::make_pair(utl::toInt(command[5]), utl::toInt(command[6])));
							}
						}
						else if(commands == 8)
						{
							if(utl::isNumber(command[5]) && utl::isNumber(command[6]) && utl::isNumber(command[7]))
							{
								//set BtnAtr ButtonName AtrName AtrCharacteristic int int int
								guiBuilder.setBtnAtr(command[2], command[3],getBtnAtrChar(command[4]), sf::Color(utl::toInt(command[5]), utl::toInt(command[6]),utl::toInt(command[7])));
							}
						}
					}
					else if(command[1] == "Group")
					{
						if(commands == 4)
						{
							if(utl::isNumber(command[3]))
							{
								//set Group buttonCharacteristic int
								guiBuilder.setGroupAtr(getBtnChar(command[2]), utl::toInt(command[3]));
							}
							else if(getBtnChar(command[2]) == gui::BtnChar::isVisible)
							{
								//set Group buttonCharacteristic bool
								if(command[3] == "t")
								{
									guiBuilder.setGroupAtr(getBtnChar(command[2]), true);
								}
								else
								{
									guiBuilder.setGroupAtr(getBtnChar(command[2]), false);
								}
							}
							else
							{
								//set Group buttonCharacteristic string
								guiBuilder.setGroupAtr(getBtnChar(command[2]), command[3]);
							}
						}
						else if(commands == 5)
						{
							if(utl::isNumber(command[3]) && utl::isNumber(command[4]))
							{
								//set Group buttonCharacteristic int int
								guiBuilder.setGroupAtr(getBtnChar(command[2]), std::make_pair(utl::toInt(command[3]), utl::toInt(command[4])));
							}
							else if(utl::isNumber(command[4]))
							{
								//set Group groupName buttonCharacteristic int
								guiBuilder.setGroupAtr(command[2], getBtnChar(command[3]), utl::toInt(command[4]));
							}
							else if(getBtnChar(command[3]) == gui::BtnChar::isVisible)
							{
								//set Group groupName buttonCharacteristic bool
								if(command[4] == "t")
								{
									guiBuilder.setGroupAtr(command[2],getBtnChar(command[3]), true);
								}
								else
								{
									guiBuilder.setGroupAtr(command[2],getBtnChar(command[3]), false);
								}
							}
							else
							{
								//set Group groupName buttonCharacteristic String
								guiBuilder.setGroupAtr(command[2],getBtnChar(command[3]), command[4]);
							}
						}
						else if(commands == 6)
						{
							if(utl::isNumber(command[4]) && utl::isNumber(command[5]))
							{
								//set Group groupName buttonCharacteristic int int
								guiBuilder.setGroupAtr(command[2],getBtnChar(command[3]), std::make_pair(utl::toInt(command[4]), utl::toInt(command[5])));
							}
						}
					}
					else if(command[1] == "List")
					{
						if(commands == 4)
						{
							if(utl::isNumber(command[3]))
							{
								//set List buttonCharacteristic int
								guiBuilder.setListAtr(getBtnChar(command[2]), utl::toInt(command[3]));
							}
							else if(getBtnChar(command[2]) == gui::BtnChar::isVisible)
							{
								//set List buttonCharacteristic bool
								if(command[3] == "t")
								{
									guiBuilder.setListAtr(getBtnChar(command[2]), true);
								}
								else
								{
									guiBuilder.setListAtr(getBtnChar(command[2]), false);
								}
							}
							else
							{
								//set List buttonCharacteristic string
								guiBuilder.setListAtr(getBtnChar(command[2]), command[3]);
							}
						}
						else if(commands == 5)
						{
							if(utl::isNumber(command[3]) && utl::isNumber(command[4]))
							{
								//set List buttonCharacteristic int int
								guiBuilder.setListAtr(getBtnChar(command[2]), std::make_pair(utl::toInt(command[3]), utl::toInt(command[4])));
							}
							else if(utl::isNumber(command[4]))
							{
								//set List ListName buttonCharacteristic int
								guiBuilder.setListAtr(command[2], getBtnChar(command[3]), utl::toInt(command[4]));
							}
							else if(getBtnChar(command[3]) == gui::BtnChar::isVisible)
							{
								//set List ListName buttonCharacteristic bool
								if(command[4] == "t")
								{
									guiBuilder.setListAtr(command[2],getBtnChar(command[3]), true);
								}
								else
								{
									guiBuilder.setListAtr(command[2],getBtnChar(command[3]), false);
								}
							}
							else
							{
								//set List ListName buttonCharacteristic String
								guiBuilder.setListAtr(command[2],getBtnChar(command[3]), command[4]);
							}
						}
						else if(commands == 6)
						{
							if(utl::isNumber(command[4]) && utl::isNumber(command[5]))
							{
								//set List ListName buttonCharacteristic int int
								guiBuilder.setListAtr(command[2],getBtnChar(command[3]), std::make_pair(utl::toInt(command[4]), utl::toInt(command[5])));
							}
						}
					}
				}
				else if(command[0] == "addTo")
				{
					if(command[1] == "Group")
					{
						if(commands == 3)
						{
							//addTo group entryName
							guiBuilder.addToGroup(command[2]);
						}
						else if(commands == 4)
						{
							//addTo group groupName entryName
							guiBuilder.addToGroup(command[2], command[3]);
						}
					}
				}
			}
			else if(commands == 2)
			{
				if(command[0] == "create")
				{
					if(command[1] == "listEntry")
					{
						//create listEntry
						guiBuilder.createListEntry();
					}
				}
			}
		}
	}
}
bool GuiLoader::isLoadedGui(const std::string& gui) const noexcept
{
	if(std::find(loadedGuis.begin(), loadedGuis.end(), gui) == loadedGuis.end())
	{
		return false;
	}
	return true;
}

gui::BtnAtr GuiLoader::getBtnAtr(const std::string& buttonAtr) const noexcept
{
	if(buttonAtr == "Sprite")
	{
		return gui::BtnAtr::Sprite;
	}
	else if(buttonAtr == "Text")
	{
		return gui::BtnAtr::Text;
	}
	else if(buttonAtr == "Hover")
	{
		return gui::BtnAtr::Hover;
	}
	else if(buttonAtr == "Percent")
	{
		return gui::BtnAtr::Percent;
	}
	//TODO log error
	return gui::BtnAtr::Sprite;
}

gui::BtnAtrChar GuiLoader::getBtnAtrChar(const std::string& buttonAtrCharacteristic) const noexcept
{
	if(buttonAtrCharacteristic == "CharSize")
	{
		return gui::BtnAtrChar::charSize;
	}
	else if(buttonAtrCharacteristic == "Color" || buttonAtrCharacteristic == "Colour")
	{
		return gui::BtnAtrChar::color;
	}
	else if(buttonAtrCharacteristic == "Coords" || buttonAtrCharacteristic == "Pos")
	{
		return gui::BtnAtrChar::coords;
	}
	else if(buttonAtrCharacteristic == "Flip")
	{
		return gui::BtnAtrChar::flip;
	}
	else if(buttonAtrCharacteristic == "Sprite")
	{
		return gui::BtnAtrChar::sprite;
	}
	else if(buttonAtrCharacteristic == "Text")
	{
		return gui::BtnAtrChar::text;
	}
	else if(buttonAtrCharacteristic == "Texture")
	{
		return gui::BtnAtrChar::texture;
	}
	else if(buttonAtrCharacteristic == "Transparency" || buttonAtrCharacteristic == "Alpha")
	{
		return gui::BtnAtrChar::transparency;
	}
	else if(buttonAtrCharacteristic == "Size")
	{
		return gui::BtnAtrChar::size;
	}
	else if(buttonAtrCharacteristic == "Direction")
	{
		return gui::BtnAtrChar::direction;
	}
	else if(buttonAtrCharacteristic == "Percent")
	{
		return gui::BtnAtrChar::percentage;
	}
	//TODO log error
	return gui::BtnAtrChar::size;
}

gui::BtnChar GuiLoader::getBtnChar(const std::string& buttonCharacteristic) const noexcept
{
	if(buttonCharacteristic == "AddToScroll_X")
	{
		return gui::BtnChar::addToScroll_x;
	}
	else if(buttonCharacteristic == "AddToScroll_Y")
	{
		return gui::BtnChar::addToScroll_y;
	}
	else if(buttonCharacteristic == "Bounds")
	{
		return gui::BtnChar::bounds;
	}
	else if(buttonCharacteristic == "Coords")
	{
		return gui::BtnChar::coords;
	}
	else if(buttonCharacteristic == "Coords_Group")
	{
		return gui::BtnChar::coords_group;
	}
	else if(buttonCharacteristic == "IsTemplate")
	{
		return gui::BtnChar::isTemplate;
	}
	else if(buttonCharacteristic == "IsVisible")
	{
		return gui::BtnChar::isVisible;
	}
	else if(buttonCharacteristic == "Scroll_x")
	{
		return gui::BtnChar::scroll_x;
	}
	else if(buttonCharacteristic == "Scroll_y")
	{
		return gui::BtnChar::scroll_y;
	}
	//TODO log error
	return gui::BtnChar::coords;
}
