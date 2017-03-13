#include "QuestiaEng/StateManager/States/State_MainMenu.h"

State_MainMenu::State_MainMenu(sf::RenderWindow &window):
    window(window)
    , resourceManager()
    , guiManager(window, resourceManager)
    , saveFile()
{
    ///gui
    //font
    guiManager.setFont(Data_Desktop::getInstance().font1);
    //loader
    guiLoader.setGuiPack(saveFile.getGuiPack());
    guiLoader.loadGui(guiManager, "mainMenu");

    ///music
    if(!musicBuffer.loadFromFile("Media/Sound/Music/gameMenu.ogg")) {}
    musicSound.setBuffer(musicBuffer);
    musicSound.setVolume(saveFile.getMusicVolume());
    musicSound.play();
}

State_MainMenu::~State_MainMenu()
{
    std::cout<<"DEBUG: State_MainMenu Destroyed"<<std::endl;
}

void State_MainMenu::processImput(sf::Keyboard::Key key,bool isPressed)
{

}

void State_MainMenu::update(sf::Time elapsedTime)
{
    guiManager.setMousePosition(std::make_pair(Data_Desktop::getInstance().getScaledMousePosition(window).x,Data_Desktop::getInstance().getScaledMousePosition(window).y));

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if(guiManager.isClicked("play"))
        {
            StateManager::getInstance().changeState(new State_Transition(window, GameState::State_Game));
        }
        else if(guiManager.isClicked("options"))
        {
            StateManager::getInstance().changeState(new State_Transition(window, GameState::State_OptionsMenu));
        }
        else if(guiManager.isClicked("exit"))
        {
            window.close();
        }
        else if(guiManager.isClicked("editor"))
        {
            StateManager::getInstance().changeState(new State_Transition(window, GameState::State_TileMapEditor));
        }
        else if(guiManager.isClicked("multiplayer"))
        {
            StateManager::getInstance().changeState(new State_Transition(window, GameState::State_MultiplayerGame));
        }
    }
}

void State_MainMenu::displayTextures()
{
    guiManager.drawButtons();
}

