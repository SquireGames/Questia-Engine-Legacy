#include "QuestiaEng/StateManager/States/State_MultiplayerSelection.h"

State_MultiplayerSelection::State_MultiplayerSelection(sf::RenderWindow &window):
    window(window)
    , resourceManager()
    , guiManager(window, resourceManager)
{
    ///background
    aMenuImage.setTexture(resourceManager.getTexture("Media/Image/Gui/Menu/aMenu.png"));
}

State_MultiplayerSelection::~State_MultiplayerSelection()
{

}

void State_MultiplayerSelection::processImput(sf::Keyboard::Key key,bool isPressed)
{

}

void State_MultiplayerSelection::update(sf::Time elapsedTime)
{
   // guiManager.setMousePosition(std::make_pair(Data_Desktop::getInstance().getScaledMousePosition(window).x,Data_Desktop::getInstance().getScaledMousePosition(window).y));

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {

    }
}

void State_MultiplayerSelection::displayTextures()
{
    window.draw(aMenuImage);
    guiManager.drawButtons();
}

