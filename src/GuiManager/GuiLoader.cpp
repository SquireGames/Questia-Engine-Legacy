#include "QuestiaEng/GuiManager/GuiLoader.h"

GuiLoader::GuiLoader()
{
    //ctor
}

GuiLoader::~GuiLoader()
{
    //dtor
}

void GuiLoader::setGuiPack(std::string guiPack)
{
    primaryPack = guiPack;
}

void GuiLoader::loadGui(GuiManager& guiManager, std::string gui)
{
    if(guiManager.isLoadedGuiPack(gui))
    {
        return;
    }

    std::string filePath = utl::conjoinString({"Data/Gui Pack/", primaryPack, "/",  gui, ".txt"});
    if(!utl::doesExist(filePath))
    {
        filePath = utl::conjoinString({"Data/Gui Pack/", secondaryPack, "/",  gui, ".txt"});
    }

    guiFile.setFilePath(filePath);

    if(guiFile.readFile('^'))
    {
        std::vector<std::string> guiCommands = guiFile.getItemList();

        for(unsigned int it = 0; it != guiCommands.size(); it++)
        {
            std::vector<std::string> command = utl::separateString(guiCommands[it], '|');
            const unsigned int& commands = command.size();

            if(commands >= 3)
            {
                if(command[0] == "create")
                {
                    if(command[1] == "Button")
                    {
                        if(commands == 3)
                        {
                            //create button name
                            guiManager.createButton(command[2]);
                        }
                        else if(commands == 4)
                        {
                            if(utl::isNumber(command[3]))
                            {
                                //create button name layer
                                guiManager.createButton(command[2], utl::asInt(command[3]));
                            }
                            else
                            {
                                //create button copy original
                                guiManager.createButton(command[2], command[3]);
                            }

                        }
                    }
                    else if(command[1] == "ButtonTemplate")
                    {
                        if(commands == 3)
                        {
                            //create button-template name
                            guiManager.createButtonTemplate(command[2]);
                        }
                        else if(commands == 4)
                        {
                            if(utl::isNumber(command[3]))
                            {
                                //create button-template name layer
                                guiManager.createButtonTemplate(command[2], utl::asInt(command[3]));
                            }
                            else
                            {
                                //create button-template copy original
                                guiManager.createButtonTemplate(command[2], command[3]);
                            }

                        }
                    }
                    else if(command[1] == "ButtonAtr")
                    {
                        if(commands == 4)
                        {
                            //create buttonAtr atrName atrType
                            guiManager.createBtnAtr(command[2], getBtnAtr(command[3]));
                        }
                        else if(commands == 5)
                        {
                            //create buttonAtr buttonName atrName atrType
                            guiManager.createBtnAtr(command[2], command[3],  getBtnAtr(command[4]));
                        }
                    }
                    else if(command[1] == "Group")
                    {
                        //create group groupName
                        guiManager.createGroup(command[2]);
                    }
                    else if(command[1] == "GroupTemplate")
                    {
                        //create group-template template-name
                        guiManager.createGroupTemplate(command[2]);
                    }
                    else if(command[1] == "GroupFromTemplate")
                    {
                        if(commands == 4)
                        {
                            //create groupFromTemplate groupName templateName
                            guiManager.createGroupFromTemplate(command[2], command[3]);
                        }
                    }
                    else if(command[1] == "List")
                    {
                        //create list listName
                        guiManager.createList(command[2]);
                    }
                    else if(command[1] == "ListEntry")
                    {
                        //create listEntry listName
                        guiManager.createListEntry(command[2]);
                    }
                }
                else if(command[0] == "set")
                {
                    if(command[1] == "ButtonLayer")
                    {
                        if(commands == 3)
                        {
                            //set ButtonLayer Layer
                            guiManager.setButtonLayer(utl::asInt(command[2]));
                        }
                        else if(commands == 4)
                        {
                            //set ButtonLayer Button Layer
                            guiManager.setButtonLayer(command[2], utl::asInt(command[3]));
                        }
                    }
                    else if(command[1] == "ListSpacing")
                    {
                        if(commands == 4)
                        {
                            //set ListSpacing Spacing
                            guiManager.setListSpacing(utl::asInt(command[2]), utl::asInt(command[3]));
                        }
                        else if(commands == 5)
                        {
                            //set ListSpacing List Spacing
                            guiManager.setListSpacing(command[2], utl::asInt(command[3]), utl::asInt(command[4]));
                        }
                    }
                    else if(command[1] == "ListTemplate")
                    {
                        if(commands == 3)
                        {
                            //set ListTemplate Template
                            guiManager.setListTemplate(command[2]);
                        }
                        else if(commands == 4)
                        {
                            //set ListTemplate List Template
                            guiManager.setListTemplate(command[2], command[3]);
                        }
                    }
                    else if(command[1] == "ListPosition")
                    {
                        if(commands == 4)
                        {
                            //set ListPosition x y
                            guiManager.setListPosition(std::make_pair(utl::asInt(command[2]), utl::asInt(command[3])));
                        }
                        else if(commands == 5)
                        {
                            //set ListPosition List x y
                            guiManager.setListPosition(command[2], std::make_pair(utl::asInt(command[3]), utl::asInt(command[4])));
                        }
                    }
                    else if(command[1] == "Button")
                    {
                        if(commands == 4)
                        {
                            if(utl::isNumber(command[3]))
                            {
                                //set Button buttonCharacteristic int
                                guiManager.setButton(getBtnChar(command[2]), utl::asInt(command[3]));
                            }
                            else if(getBtnChar(command[2]) == gui::BtnChar::isVisible)
                            {
                                //set Button buttonCharacteristic bool
                                if(command[3] == "t")
                                {
                                    guiManager.setButton(getBtnChar(command[2]), true);
                                }
                                else
                                {
                                    guiManager.setButton(getBtnChar(command[2]), false);
                                }
                            }
                            else
                            {
                                //set Button buttonCharacteristic string
                                guiManager.setButton(getBtnChar(command[2]), command[3]);
                            }
                        }
                        else if(commands == 5)
                        {
                            if(utl::isNumber(command[3]) && utl::isNumber(command[4]))
                            {
                                //set Button buttonCharacteristic int int
                                guiManager.setButton(getBtnChar(command[2]), std::make_pair(utl::asInt(command[3]), utl::asInt(command[4])));
                            }
                            else if(utl::isNumber(command[4]))
                            {
                                //set Button buttonName buttonCharacteristic int
                                guiManager.setButton(command[2], getBtnChar(command[3]), utl::asInt(command[4]));
                            }
                            else if(getBtnChar(command[3]) == gui::BtnChar::isVisible)
                            {
                                //set Button buttonName buttonCharacteristic bool
                                if(command[4] == "t")
                                {
                                    guiManager.setButton(command[2],getBtnChar(command[3]), true);
                                }
                                else
                                {
                                    guiManager.setButton(command[2],getBtnChar(command[3]), false);
                                }
                            }
                            else
                            {
                                //set Button ButtonName buttonCharacteristic String
                                guiManager.setButton(command[2],getBtnChar(command[3]), command[4]);
                            }
                        }
                        else if(commands == 6)
                        {
                            if(utl::isNumber(command[4]) && utl::isNumber(command[5]))
                            {
                                //set Button ButtonName buttonCharacteristic int int
                                guiManager.setButton(command[2],getBtnChar(command[3]), std::make_pair(utl::asInt(command[4]), utl::asInt(command[5])));
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
                                guiManager.setBtnAtr(getBtnAtrChar(command[2]), utl::asInt(command[3]));
                            }
                            else if(getBtnChar(command[2]) == gui::BtnChar::isVisible)
                            {
                                //set BtnAtr AtrCharacteristic bool
                                if(command[3] == "t")
                                {
                                    guiManager.setBtnAtr(getBtnAtrChar(command[2]), true);
                                }
                                else
                                {
                                    guiManager.setBtnAtr(getBtnAtrChar(command[2]), false);
                                }
                            }
                            else if(getBtnAtrChar(command[2]) == gui::BtnAtrChar::flip || getBtnAtrChar(command[2]) == gui::BtnAtrChar::direction)
                            {
                                //set BtnAtr AtrCharacteristic(flip) char
                                guiManager.setBtnAtr(getBtnAtrChar(command[2]), command[3].at(0));
                            }
                            else
                            {
                                //set BtnAtr AtrCharacteristic string
                                guiManager.setBtnAtr(getBtnAtrChar(command[2]), command[3]);
                            }
                        }
                        else if(commands == 5)
                        {
                            if(utl::isNumber(command[3]) && utl::isNumber(command[4]))
                            {
                                //set BtnAtr AtrCharacteristic int int
                                guiManager.setBtnAtr(getBtnAtrChar(command[2]), std::make_pair(utl::asInt(command[3]), utl::asInt(command[4])));
                            }
                        }
                        else if(commands == 6)
                        {
                            //set BtnAtr AtrCharacteristic int int int
                            if(utl::isNumber(command[3]) && utl::isNumber(command[4]) && utl::isNumber(command[5]))
                            {
                                guiManager.setBtnAtr(getBtnAtrChar(command[2]), sf::Color(utl::asInt(command[3]), utl::asInt(command[4]),utl::asInt(command[5])));
                            }
                            else if(utl::isNumber(command[5]))
                            {
                                //set BtnAtr ButtonName AtrName AtrCharacteristic int
                                guiManager.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), utl::asInt(command[5]));
                            }
                            else if(getBtnChar(command[4]) == gui::BtnChar::isVisible)
                            {
                                //set BtnAtr ButtonName AtrName AtrCharacteristic bool
                                if(command[5] == "t")
                                {
                                    guiManager.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), true);
                                }
                                else
                                {
                                    guiManager.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), false);
                                }
                            }
                            else if(getBtnAtrChar(command[4]) == gui::BtnAtrChar::flip || getBtnAtrChar(command[4]) == gui::BtnAtrChar::direction)
                            {
                                //set BtnAtr ButtonName AtrName  AtrCharacteristic(flip) char
                                guiManager.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), command[5].at(0));
                            }
                            else
                            {
                                //set BtnAtr ButtonName AtrName AtrCharacteristic string
                                guiManager.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), command[5]);
                            }
                        }
                        else if(commands == 7)
                        {
                            if(utl::isNumber(command[5]) && utl::isNumber(command[6]))
                            {
                                ////set BtnAtr ButtonName AtrName AtrCharacteristic int int
                                guiManager.setBtnAtr(command[2], command[3], getBtnAtrChar(command[4]), std::make_pair(utl::asInt(command[5]), utl::asInt(command[6])));
                            }
                        }
                        else if(commands == 8)
                        {
                            if(utl::isNumber(command[5]) && utl::isNumber(command[6]) && utl::isNumber(command[7]))
                            {
                                //set BtnAtr ButtonName AtrName AtrCharacteristic int int int
                                guiManager.setBtnAtr(command[2], command[3],getBtnAtrChar(command[4]), sf::Color(utl::asInt(command[5]), utl::asInt(command[6]),utl::asInt(command[7])));
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
                                guiManager.setGroupAtr(getBtnChar(command[2]), utl::asInt(command[3]));
                            }
                            else if(getBtnChar(command[2]) == gui::BtnChar::isVisible)
                            {
                                //set Group buttonCharacteristic bool
                                if(command[3] == "t")
                                {
                                    guiManager.setGroupAtr(getBtnChar(command[2]), true);
                                }
                                else
                                {
                                    guiManager.setGroupAtr(getBtnChar(command[2]), false);
                                }
                            }
                            else
                            {
                                //set Group buttonCharacteristic string
                                guiManager.setGroupAtr(getBtnChar(command[2]), command[3]);
                            }
                        }
                        else if(commands == 5)
                        {
                            if(utl::isNumber(command[3]) && utl::isNumber(command[4]))
                            {
                                //set Group buttonCharacteristic int int
                                guiManager.setGroupAtr(getBtnChar(command[2]), std::make_pair(utl::asInt(command[3]), utl::asInt(command[4])));
                            }
                            else if(utl::isNumber(command[4]))
                            {
                                //set Group groupName buttonCharacteristic int
                                guiManager.setGroupAtr(command[2], getBtnChar(command[3]), utl::asInt(command[4]));
                            }
                            else if(getBtnChar(command[3]) == gui::BtnChar::isVisible)
                            {
                                //set Group groupName buttonCharacteristic bool
                                if(command[4] == "t")
                                {
                                    guiManager.setGroupAtr(command[2],getBtnChar(command[3]), true);
                                }
                                else
                                {
                                    guiManager.setGroupAtr(command[2],getBtnChar(command[3]), false);
                                }
                            }
                            else
                            {
                                //set Group groupName buttonCharacteristic String
                                guiManager.setGroupAtr(command[2],getBtnChar(command[3]), command[4]);
                            }
                        }
                        else if(commands == 6)
                        {
                            if(utl::isNumber(command[4]) && utl::isNumber(command[5]))
                            {
                                //set Group groupName buttonCharacteristic int int
                                guiManager.setGroupAtr(command[2],getBtnChar(command[3]), std::make_pair(utl::asInt(command[4]), utl::asInt(command[5])));
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
                                guiManager.setListAtr(getBtnChar(command[2]), utl::asInt(command[3]));
                            }
                            else if(getBtnChar(command[2]) == gui::BtnChar::isVisible)
                            {
                                //set List buttonCharacteristic bool
                                if(command[3] == "t")
                                {
                                    guiManager.setListAtr(getBtnChar(command[2]), true);
                                }
                                else
                                {
                                    guiManager.setListAtr(getBtnChar(command[2]), false);
                                }
                            }
                            else
                            {
                                //set List buttonCharacteristic string
                                guiManager.setListAtr(getBtnChar(command[2]), command[3]);
                            }
                        }
                        else if(commands == 5)
                        {
                            if(utl::isNumber(command[3]) && utl::isNumber(command[4]))
                            {
                                //set List buttonCharacteristic int int
                                guiManager.setListAtr(getBtnChar(command[2]), std::make_pair(utl::asInt(command[3]), utl::asInt(command[4])));
                            }
                            else if(utl::isNumber(command[4]))
                            {
                                //set List ListName buttonCharacteristic int
                                guiManager.setListAtr(command[2], getBtnChar(command[3]), utl::asInt(command[4]));
                            }
                            else if(getBtnChar(command[3]) == gui::BtnChar::isVisible)
                            {
                                //set List ListName buttonCharacteristic bool
                                if(command[4] == "t")
                                {
                                    guiManager.setListAtr(command[2],getBtnChar(command[3]), true);
                                }
                                else
                                {
                                    guiManager.setListAtr(command[2],getBtnChar(command[3]), false);
                                }
                            }
                            else
                            {
                                //set List ListName buttonCharacteristic String
                                guiManager.setListAtr(command[2],getBtnChar(command[3]), command[4]);
                            }
                        }
                        else if(commands == 6)
                        {
                            if(utl::isNumber(command[4]) && utl::isNumber(command[5]))
                            {
                                //set List ListName buttonCharacteristic int int
                                guiManager.setListAtr(command[2],getBtnChar(command[3]), std::make_pair(utl::asInt(command[4]), utl::asInt(command[5])));
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
                            guiManager.addToGroup(command[2]);
                        }
                        else if(commands == 4)
                        {
                            //addTo group groupName entryName
                            guiManager.addToGroup(command[2], command[3]);
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
                        guiManager.createListEntry();
                    }
                }
            }
        }
    }
    else
    {
        throw std::runtime_error(utl::conjoinString({"FATAL ERROR - Guiloader failed to read file path:", filePath}));
    }
}

gui::BtnAtr GuiLoader::getBtnAtr(std::string buttonAtr)
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

gui::BtnAtrChar GuiLoader::getBtnAtrChar(std::string buttonAtrCharacteristic)
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

gui::BtnChar GuiLoader::getBtnChar(std::string buttonCharacteristic)
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
