/*
#include <iostream>
#include <sstream>
#include <math.h>

#include "QuestiaEng/StateManager/StateManager.h"
#include "QuestiaEng/StateManager/States/State_LANGame.h"
#include "QuestiaEng/StateManager/States/State_Transition.h"

#include "QuestiaEng/TileEngine/TileEngine.h"
#include "QuestiaEng/EntityManager/EntityManager.h"

#include "QuestiaEng/Data/Data_Desktop.h"

State_LANGame::State_LANGame(sf::RenderWindow &mWindow):
    window(mWindow)
    , tileEngine (1)
    , entityManager (sf::Vector2f(800,2400), mWindow)

    , mIsMovingUp(false)
    , mIsMovingDown(false)
    , mIsMovingLeft(false)
    , mIsMovingRight(false)

    , player_Velocity(1,1)
    , player_MapCoordinates(0,0)

{
    std::cout<<"State_Game Initialized"<<std::endl;
    gameView.setSize(1920,1080);
    gameView.setCenter(960,540);
    gameView.zoom(0.575);

    mStatisticsText.setFont(Data_Desktop::getInstance().font1);
    mStatisticsText.setCharacterSize(50);
    mStatisticsText.setColor(sf::Color::Yellow);
    mStatisticsText.setPosition(1700,25);

    mStatisticsText2.setFont(Data_Desktop::getInstance().font1);
    mStatisticsText2.setCharacterSize(50);
    mStatisticsText2.setColor(sf::Color::Yellow);
    mStatisticsText2.setPosition(1700,100);

    statisticsTextEntityCount.setFont(Data_Desktop::getInstance().font1);
    statisticsTextEntityCount.setCharacterSize(50);
    statisticsTextEntityCount.setColor(sf::Color::Yellow);
    statisticsTextEntityCount.setPosition(1700,175);


    statisticsTextHP.setFont(Data_Desktop::getInstance().font1);
    statisticsTextHP.setCharacterSize(30);
    statisticsTextHP.setColor(sf::Color::Red);
    statisticsTextHP.setPosition(25,50);

    statisticsTextMP.setFont(Data_Desktop::getInstance().font1);
    statisticsTextMP.setCharacterSize(30);
    statisticsTextMP.setColor(sf::Color::Blue);
    statisticsTextMP.setPosition(25,100);

    statisticsTextStamina.setFont(Data_Desktop::getInstance().font1);
    statisticsTextStamina.setCharacterSize(30);
    statisticsTextStamina.setColor(sf::Color::Yellow);
    statisticsTextStamina.setPosition(25,150);


    overlayView.setSize(1920,1080);
    overlayView.setCenter(960,540);
    overlayView.zoom(1);
}

State_Game::~State_Game()
{
    std::cout<<"State_Game Destroyed"<<std::endl;
}

void State_Game::processImput(sf::Keyboard::Key key,bool isPressed)
{
    entityManager.handleInput(key, isPressed);

    if(key == sf::Keyboard::M)
    {
        //StateManager::getInstance().changeeState(new State_Transition(window, 1));
    }

    if(key == sf::Keyboard::W || key == sf::Keyboard::Up)
    {
        mIsMovingUp = isPressed;
    }
    if(key == sf::Keyboard::S || key == sf::Keyboard::Down)
    {
        mIsMovingDown = isPressed;
    }
    if(key == sf::Keyboard::A || key == sf::Keyboard::Left)
    {
        mIsMovingLeft = isPressed;
    }
    if(key == sf::Keyboard::D || key == sf::Keyboard::Right)
    {
        mIsMovingRight = isPressed;
    }

    if(key == sf::Keyboard::E)
    {
        entityManager.createEntity(1, sf::Vector2f(entityManager.getPlayerCoordinates().x,entityManager.getPlayerCoordinates().y));
    }
}

void State_Game::update(sf::Time elapsedTime)
{
    //FPS
    os << Data_Desktop::getInstance().get_FPS();
    mStatisticsText.setString(os.str());

    os.str(std::string());

    //Coordinates
    player_MapCoordinates.x = (int)entityManager.getPlayerCoordinates().x/32 + 1;
    player_MapCoordinates.y = (int)entityManager.getPlayerCoordinates().y/32 + 1;

    tempCoords_x = player_MapCoordinates.x-1;
    tempCoords_y = player_MapCoordinates.y-1;

    os << tempCoords_x;
    os << ",";
    os << tempCoords_y;
    mStatisticsText2.setString(os.str());

    os.str(std::string());

    //HP, MP, Stamina
    os << entityManager.getPlayerStats().x;
    statisticsTextHP.setString(os.str());
    os.str(std::string());
    os << entityManager.getPlayerStats().y;
    statisticsTextMP.setString(os.str());
    os.str(std::string());
    os << entityManager.getPlayerStats().z;
    statisticsTextStamina.setString(os.str());
    os.str(std::string());

    //Entity Count
    os << entityManager.getEntityCount();
    statisticsTextEntityCount.setString(os.str());

    os.str(std::string());

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

    os << angle;
    statisticsTextEntityCount.setString(os.str());
    os.str(std::string());

    entityManager.update(tileEngine, player_MapCoordinates, Data_Desktop::getInstance().getScaledMousePosition(window), angle);

    gameView.setCenter(entityManager.getPlayerCoordinates());

}

void State_Game::displayTextures()
{
    window.setView(gameView);
    tileEngine.drawMap(window, player_MapCoordinates);
    entityManager.drawEntity();

    window.setView(overlayView);
    window.draw(mStatisticsText);
    window.draw(mStatisticsText2);
    window.draw(statisticsTextEntityCount);

    window.draw(statisticsTextHP);
    window.draw(statisticsTextMP);
    window.draw(statisticsTextStamina);
}

*/
