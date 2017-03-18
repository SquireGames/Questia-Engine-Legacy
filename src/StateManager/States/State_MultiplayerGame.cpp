#include "QuestiaEng/StateManager/States/State_MultiplayerGame.h"

State_MultiplayerGame::State_MultiplayerGame(sf::RenderWindow &window):
    window(window)
    //managers
    , resourceManager()
    , newGuiManager(window, resourceManager)
    , entityManager (resourceManager)
    , multiplayerManager("Temporary server name")

    , playerAngle(0)

    //keys
    , moveKey0(0)
    , moveKey1(1)
    , moveKey2(2)
    , moveKey3(3)
    , skillKey4(4)
    , skillKey5(5)
    , skillKey6(6)

    , talkKey(20)
    , toggleTalkKey(21)

    //etc
    , pause(false)
{
    ///views
    gameView.setSize(1920,1080);
    gameView.setCenter(960,540);
    gameView.zoom(0.5);

    overlayView.setSize(1920,1080);
    overlayView.setCenter(960,540);
    overlayView.zoom(1);

    ///loading the map

    ///rebindable keys
    moveKey0.keyType = 0;
    //moveKey0.keyInput = Data_Desktop::getInstance().getKey(0);
    keybindVector.push_back(moveKey0);
    moveKey1.keyType = 0;
    //moveKey1.keyInput = Data_Desktop::getInstance().getKey(1);
    keybindVector.push_back(moveKey1);
    moveKey2.keyType = 0;
    //moveKey2.keyInput = Data_Desktop::getInstance().getKey(2);
    keybindVector.push_back(moveKey2);
    moveKey3.keyType = 0;
    //moveKey3.keyInput = Data_Desktop::getInstance().getKey(3);
    keybindVector.push_back(moveKey3);
    skillKey4.keyType = 0;
    //skillKey4.keyInput = Data_Desktop::getInstance().getKey(4);
    keybindVector.push_back(skillKey4);
    skillKey5.keyType = 0;
    //skillKey5.keyInput = Data_Desktop::getInstance().getKey(5);
    keybindVector.push_back(skillKey5);
    skillKey6.keyType = 0;
    //skillKey6.keyInput = Data_Desktop::getInstance().getKey(6);
    keybindVector.push_back(skillKey6);
    talkKey.keyType = 0;
    talkKey.keyInput = sf::Keyboard::Return;
    keybindVector.push_back(talkKey);
    toggleTalkKey.keyType = 0;
    toggleTalkKey.keyInput = sf::Keyboard::RControl;
    keybindVector.push_back(toggleTalkKey);

    //alignment.setTexture(resourceManager.getTexture("Media/Image/Alignment.png"));

    newGuiManager.setFont(Data_Desktop::getInstance().font1);

}

State_MultiplayerGame::~State_MultiplayerGame()
{
    std::cout<<"DEBUG: State_MultiplayerGame Destroyed"<<std::endl;

    multiplayerManager.terminateHost();
    while(!multiplayerManager.isServerDead())
    {
        multiplayerManager.update();
        sf::sleep(sf::Time(sf::milliseconds(sf::Int32(100))));
    }

    multiplayerManager.terminateClient();
    while(!multiplayerManager.isClientDead())
    {
        multiplayerManager.update();
        sf::sleep(sf::Time(sf::milliseconds(sf::Int32(100))));
    }
}

void State_MultiplayerGame::processImput(sf::Keyboard::Key key,bool isPressed)
{
    if(pause)
    {
        if(key == sf::Keyboard::Escape)
        {
//            if(guiManager.buttonTimer())
            //        {
            newGuiManager.setGroupAtr("pauseMenu", gui::ButtonCharacteristic::isVisible, false);
            pause = false;
            //          }
        }
    }
    else
    {
        int currentKey;
        for(unsigned char x = 0; x < keybindVector.size(); x++)
        {
            if(keybindVector[x].keyType == 0)
            {
                if(key == keybindVector[x].keyInput)
                {
                    keybindVector[x].activated = isPressed;
                    currentKey = keybindVector[x].keyNumber;
                    x = keybindVector.size();
                }
            }
        }
        if(key == sf::Keyboard::Escape)
        {
            newGuiManager.setGroupAtr("pauseMenu", gui::ButtonCharacteristic::isVisible, true);
            pause = true;

        }
    }

    if(key == sf::Keyboard::Escape)
    {

    }
}

void State_MultiplayerGame::update(sf::Time elapsedTime)
{
    //newGuiManager.setMousePosition(std::make_pair(Data_Desktop::getInstance().getScaledMousePosition(window).x,Data_Desktop::getInstance().getScaledMousePosition(window).y));


    if(!pause)
    {
        //Angle
        float angle = atan2(Data_Desktop::getInstance().getScaledMousePosition(window).y - 540, Data_Desktop::getInstance().getScaledMousePosition(window).x - 960);
        angle = angle * 180 / 3.14159265;
        if(angle<0)
        {
            angle = angle*-1;
        }
        else
        {
            angle = angle -360;
            angle = angle * -1;
        }
        playerAngle = angle;
    }

    multiplayerManager.sendData(entityManager);
    multiplayerManager.update();
    multiplayerManager.receiveData(entityManager);


    gameView.setCenter(sf::Vector2f(1920/2, 1080/2));

    /* Gui */

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if(newGuiManager.isClicked("mainMenu"))
        {
            //StateManager::getInstance().changeeState(new State_Transition(window, 1));
        }
        else if(newGuiManager.isClicked("exitGame"))
        {
            window.close();
        }
    }
}

void State_MultiplayerGame::displayTextures()
{
    window.setView(gameView);

    window.setView(overlayView);

    newGuiManager.drawButtons();

    //window.draw(alignment);
}

