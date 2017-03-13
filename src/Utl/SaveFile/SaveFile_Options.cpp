#include "QuestiaEng/Utl/SaveFile/SaveFile_Options.h"

SaveFile_Options::SaveFile_Options()
{
    saveFile.setFilePath("options.cfg");

    if(!utl::doesExist("options.cfg"))
    {
        writeOptions();
    }

    reloadOptions();
}

SaveFile_Options::~SaveFile_Options()
{

}

void SaveFile_Options::reloadOptions()
{
    saveFile.readFile();
    if(utl::isNumber(saveFile.getItem(windowMode_name)))
    {
        windowMode = utl::asInt(saveFile.getItem(windowMode_name));
    }
    if(utl::isNumber(saveFile.getItem(fps_name)))
    {
        fps = utl::asInt(saveFile.getItem(fps_name));
    }
    if(saveFile.getItem(font_name) != "nil")
    {
        font = saveFile.getItem(font_name);
    }
    if(utl::isNumber(saveFile.getItem(musicVolume_name)))
    {
        musicVolume = utl::asInt(saveFile.getItem(musicVolume_name));
    }
    if(saveFile.getItem(guiPack_name) != "nil")
    {
        guiPack = saveFile.getItem(guiPack_name);
    }
    if(utl::isNumber(saveFile.getItem(key_moveUp_name)))
    {
        key_moveUp = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_moveUp_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_moveDown_name)))
    {
        key_moveDown = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_moveDown_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_moveLeft_name)))
    {
        key_moveLeft = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_moveLeft_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_moveRight_name)))
    {
        key_moveRight = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_moveRight_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_skill_1_name)))
    {
        key_skill_1 = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_skill_1_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_skill_2_name)))
    {
        key_skill_2 = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_skill_2_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_skill_3_name)))
    {
        key_skill_3 = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_skill_3_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_skill_4_name)))
    {
        key_skill_4 = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_skill_4_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_skill_5_name)))
    {
        key_skill_5 = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_skill_5_name)));
    }
    if(utl::isNumber(saveFile.getItem(key_skill_6_name)))
    {
        key_skill_6 = static_cast<ctr::Input>(utl::asInt(saveFile.getItem(key_skill_6_name)));
    }
}

void SaveFile_Options::saveOption(Option_Base* option, std::string optionName)
{
    if(optionName == windowMode_name)
    {
        if(utl::isNumber(option->getValueString()))
        {
            windowMode = utl::asInt(option->getValueString());
        }
    }
    else if(optionName == fps_name)
    {
        if(utl::isNumber(option->getValueString()))
        {
            fps = utl::asInt(option->getValueString());
        }
    }
    else if(optionName == font_name)
    {
        font = option->getValueString();
    }
    else if(optionName == musicVolume_name)
    {
        if(utl::isNumber(option->getValueString()))
        {
            musicVolume = utl::asInt(option->getValueString());
        }
    }
    else if(optionName == guiPack_name)
    {
        guiPack = option->getValueString();
    }
    else if(optionName == key_moveUp_name)
    {
        key_moveUp = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_moveDown_name)
    {
        key_moveDown = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_moveLeft_name)
    {
        key_moveLeft = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_moveRight_name)
    {
        key_moveRight = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_skill_1_name)
    {
        key_skill_1 = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_skill_2_name)
    {
        key_skill_2 = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_skill_3_name)
    {
        key_skill_3 = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_skill_4_name)
    {
        key_skill_4 = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_skill_5_name)
    {
        key_skill_5 = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
    else if(optionName == key_skill_6_name)
    {
        key_skill_6 = static_cast<ctr::Input>(utl::asInt(option->getValueString()));
    }
}


void SaveFile_Options::writeOptions()
{
    saveFile.clearSave();

    saveFile.saveItem(windowMode_name, windowMode);
    saveFile.saveItem(fps_name, fps);
    saveFile.saveItem(font_name, font);
    saveFile.saveItem(guiPack_name, guiPack);

    saveFile.saveItem(musicVolume_name, musicVolume);

    saveFile.saveItem(key_moveUp_name,    static_cast <int> (key_moveUp));
    saveFile.saveItem(key_moveDown_name,  static_cast <int> (key_moveDown));
    saveFile.saveItem(key_moveLeft_name,  static_cast <int> (key_moveLeft));
    saveFile.saveItem(key_moveRight_name, static_cast <int> (key_moveRight));
    saveFile.saveItem(key_skill_1_name, static_cast <int> (key_skill_1));
    saveFile.saveItem(key_skill_2_name, static_cast <int> (key_skill_2));
    saveFile.saveItem(key_skill_3_name, static_cast <int> (key_skill_3));
    saveFile.saveItem(key_skill_4_name, static_cast <int> (key_skill_4));
    saveFile.saveItem(key_skill_5_name, static_cast <int> (key_skill_5));
    saveFile.saveItem(key_skill_6_name, static_cast <int> (key_skill_6));

    saveFile.writeFile();
}

int SaveFile_Options::getWindowMode()
{
    return windowMode;
}
int SaveFile_Options::getFps()
{
    return fps;
}
std::string SaveFile_Options::getFont()
{
    return font;
}
int SaveFile_Options::getMusicVolume()
{
    return musicVolume;
}
std::string SaveFile_Options::getGuiPack()
{
    return guiPack;
}
std::string SaveFile_Options::getWindowMode_name()
{
    return windowMode_name;
}
std::string SaveFile_Options::getFps_name()
{
    return fps_name;
}
std::string SaveFile_Options::getFont_name()
{
    return font_name;
}
std::string SaveFile_Options::getMusicVolume_name()
{
    return musicVolume_name;
}
std::string SaveFile_Options::getGuiPack_name()
{
    return guiPack_name;
}
ctr::Input SaveFile_Options::getKey_moveUp()
{
    return key_moveUp;
}
ctr::Input SaveFile_Options::getKey_moveDown()
{
    return key_moveDown;
}
ctr::Input SaveFile_Options::getKey_moveLeft()
{
    return key_moveLeft;
}
ctr::Input SaveFile_Options::getKey_moveRight()
{
    return key_moveRight;
}
ctr::Input SaveFile_Options::getKey_skill_1()
{
    return key_skill_1;
}
ctr::Input SaveFile_Options::getKey_skill_2()
{
    return key_skill_2;
}
ctr::Input SaveFile_Options::getKey_skill_3()
{
    return key_skill_3;
}
ctr::Input SaveFile_Options::getKey_skill_4()
{
    return key_skill_4;
}
ctr::Input SaveFile_Options::getKey_skill_5()
{
    return key_skill_5;
}
ctr::Input SaveFile_Options::getKey_skill_6()
{
    return key_skill_6;
}

std::string SaveFile_Options::getKey_moveUp_name()
{
    return key_moveUp_name;
}
std::string SaveFile_Options::getKey_moveDown_name()
{
    return key_moveDown_name;
}
std::string SaveFile_Options::getKey_moveLeft_name()
{
    return key_moveLeft_name;
}
std::string SaveFile_Options::getKey_moveRight_name()
{
    return key_moveRight_name;
}
std::string SaveFile_Options::getKey_skill_1_name()
{
    return key_skill_1_name;
}
std::string SaveFile_Options::getKey_skill_2_name()
{
    return key_skill_2_name;
}
std::string SaveFile_Options::getKey_skill_3_name()
{
    return key_skill_3_name;
}
std::string SaveFile_Options::getKey_skill_4_name()
{
    return key_skill_4_name;
}
std::string SaveFile_Options::getKey_skill_5_name()
{
    return key_skill_5_name;
}
std::string SaveFile_Options::getKey_skill_6_name()
{
    return key_skill_6_name;
}
