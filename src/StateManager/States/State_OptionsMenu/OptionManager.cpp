#include "QuestiaEng/StateManager/States/State_OptionsMenu/OptionManager.h"

OptionManager::OptionManager(GuiManager* _guiManager):
    guiManager(_guiManager)
{
    if(guiManager == nullptr)
    {
        throw std::runtime_error ("OptionManager does not have access to GuiManager");
    }
}

OptionManager::~OptionManager()
{
    //do nothing with option_base
}

void OptionManager::addOption(Option_Base* option)
{
    optionVector.push_back(std::make_pair(option, OptionData()));
}

void OptionManager::initLists()
{
    for(auto& option : optionVector)
    {
        if(option.first->getList() != "nil")
        {
            std::string groupName = guiManager->createListEntry(option.first->getList());

            option.second.leftButton.buttonName =     guiManager->getGroupEntry(groupName, "leftButton");
            option.second.rightButton.buttonName =    guiManager->getGroupEntry(groupName, "rightButton");
            option.second.functionButton.buttonName = guiManager->getGroupEntry(groupName, "funcButton");

            switch (option.first->getType())
            {
            case OptionType::choice:
                option.second.leftButton.isActive =     (option.second.leftButton.buttonName !=     "nil");
                option.second.rightButton.isActive =    (option.second.rightButton.buttonName !=    "nil");
                break;
            case OptionType::functional:
            case OptionType::input:
                option.second.functionButton.isActive = (option.second.functionButton.buttonName != "nil");
                break;
            }

            option.second.textButtonName = guiManager->getGroupEntry(groupName, "optionValue");
            option.second.displayString  = option.first->getDisplayString();

            std::string optionButtonName = guiManager->getGroupEntry(groupName, "optionName");

            guiManager->setButtonAtr(option.second.textButtonName, "text", gui::ButtonAtrCharacteristic::text, *option.second.displayString);
            guiManager->setButtonAtr(optionButtonName, "text", gui::ButtonAtrCharacteristic::text, option.first->getOptionName());
        }
    }
    initSprites();
    updateArrows();
}

bool OptionManager::handleGui()
{
    //triggered on click
    for(const auto& option : optionVector)
    {
        if(option.second.leftButton.isActive)
        {
            if(guiManager->isClicked(option.second.leftButton.buttonName))
            {
                option.first->iterateBackward();
                guiManager->setButtonAtr(option.second.textButtonName, "text", gui::ButtonAtrCharacteristic::text, *option.second.displayString);

                updateArrows();
                return true;
            }
        }
        if(option.second.rightButton.isActive)
        {
            if(guiManager->isClicked(option.second.rightButton.buttonName))
            {
                option.first->iterateForward();
                guiManager->setButtonAtr(option.second.textButtonName, "text", gui::ButtonAtrCharacteristic::text, *option.second.displayString);

                updateArrows();
                return true;
            }
        }
        if(option.second.functionButton.isActive)
        {
            if(assignInput == option.second.textButtonName)
            {
                if(!guiManager->isClicked(option.second.functionButton.buttonName))
                {
                    assignInput = "nil";
                    return true;
                }
            }
            else if(guiManager->isClicked(option.second.functionButton.buttonName))
            {
                assignInput = option.second.textButtonName;
                return true;
            }
        }
    }
    return false;
}

void OptionManager::initSprites()
{
    for(const auto& option : optionVector)
    {
        switch(option.first->getType())
        {
        case OptionType::choice:
            guiManager->setButton(option.second.functionButton.buttonName, gui::ButtonCharacteristic::isActive, false);
            guiManager->setButtonAtr(option.second.functionButton.buttonName,  "funcSprite", gui::ButtonAtrCharacteristic::transparency, 0);
            break;
        case OptionType::functional:
        case OptionType::input:
            guiManager->setButton(option.second.leftButton.buttonName, gui::ButtonCharacteristic::isActive, false);
            guiManager->setButton(option.second.rightButton.buttonName, gui::ButtonCharacteristic::isActive, false);
            guiManager->setButtonAtr(option.second.leftButton.buttonName,  "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 0);
            guiManager->setButtonAtr(option.second.rightButton.buttonName, "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 0);
            break;
        }
    }
}

void OptionManager::updateArrows()
{
    for(const auto& option : optionVector)
    {
        if(option.first->getType() == OptionType::choice)
        {
            if(option.first->isBegin() && option.first->isEnd())
            {
                guiManager->setButtonAtr(option.second.leftButton.buttonName,  "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 0);
                guiManager->setButtonAtr(option.second.rightButton.buttonName, "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 0);
            }
            else if(option.first->isBegin())
            {
                guiManager->setButtonAtr(option.second.leftButton.buttonName,  "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 0);
                guiManager->setButtonAtr(option.second.rightButton.buttonName, "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 100);
            }
            else if(option.first->isEnd())
            {
                guiManager->setButtonAtr(option.second.leftButton.buttonName,  "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 100);
                guiManager->setButtonAtr(option.second.rightButton.buttonName, "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 0);
            }
            else
            {
                guiManager->setButtonAtr(option.second.leftButton.buttonName,  "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 100);
                guiManager->setButtonAtr(option.second.rightButton.buttonName, "arrowSprite", gui::ButtonAtrCharacteristic::transparency, 100);
            }
        }
    }
}

void OptionManager::saveOptions(SaveFile_Options& saveFile)
{
    for(const auto& it : optionVector)
    {
        if(it.first->isChanged())
        {
            saveFile.saveOption(it.first, it.first->getOptionName());
        }
    }
}

void OptionManager::handleInput(ctr::Input input)
{
    if(assignInput != "nil")
    {
        for(auto& option : optionVector)
        {
            if((option.second.textButtonName == assignInput) &&
                    option.first->getType() == OptionType::input &&
                    guiManager->isClicked(option.second.functionButton.buttonName))
            {
                option.first->setInput(input);
                guiManager->setButtonAtr(option.second.textButtonName, "text", gui::ButtonAtrCharacteristic::text, *option.second.displayString);
                return;
            }
        }
        assignInput = "nil";
    }
}

bool OptionManager::isMouseOverAssignedInput()
{
    if(assignInput != "nil")
    {
        for(const auto& option : optionVector)
        {
            if(option.second.functionButton.isActive)
            {
                if(assignInput == option.second.textButtonName)
                {
                    if(guiManager->isClicked(option.second.functionButton.buttonName))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void OptionManager::setMouseReleased()
{
    if(isMouseOverAssignedInput())
    {
        mouseReleases++;
        return;
    }
    mouseReleases = 0;
}

void OptionManager::checkMouseInput()
{
    if(mouseReleases >= 1 && (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)||
                              sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)||
                              sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)||
                              sf::Mouse::isButtonPressed(sf::Mouse::Button::XButton1)||
                              sf::Mouse::isButtonPressed(sf::Mouse::Button::XButton2)))
    {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            handleInput(ctr::Input::LMouse);
        }
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            handleInput(ctr::Input::RMouse);
        }
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
        {
            handleInput(ctr::Input::MMouse);
        }
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Button::XButton1))
        {
            handleInput(ctr::Input::Macro1);
        }
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Button::XButton2))
        {
            handleInput(ctr::Input::Macro2);
        }
        mouseReleases = 0;
    }
}
