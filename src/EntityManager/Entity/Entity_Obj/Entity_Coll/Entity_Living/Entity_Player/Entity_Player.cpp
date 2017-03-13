#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Living/Entity_Player/Entity_Player.h"

Entity_Player::Entity_Player(unsigned int id, EntityManager& entityManager, ResourceManager& resourceManager):
    Entity_Living(id, entityManager, resourceManager,
                  Bounds(Rect(utl::Vector2f(24,24), utl::Vector2f(12,12))),
                  Bounds(Circ(13)),
                  utl::Vector2f(0,48))
    , spriteSheet(resourceManager)
{
    collBounds.rel_coords = utl::Vector2f(0, 40);
    hitBounds.rel_coords  = utl::Vector2f(0, 0);

    coords = utl::Vector2f(0,0);

   // atrs[Type::test_attack].atrs[(int)Type_test_attack::destroyStuff] = false;

    spriteSheet.loadSprite("Media/Image/Game/Entity/Player/White.png", 8, 3);

    spriteSheet.setSize(SIZE_X, SIZE_Y);
    spriteSheet.setOrigin(SIZE_X / 4.f, SIZE_Y / 4.f);
}

Entity_Player::~Entity_Player()
{
    //dtor
}

void Entity_Player::processInput(ctr::KeyAction action, bool isPressed)
{
    switch (action)
    {
    case ctr::KeyAction::MoveUp:
        isMovingUp = isPressed;
        break;
    case ctr::KeyAction::MoveDown:
        isMovingDown = isPressed;
        break;
    case ctr::KeyAction::MoveLeft:
        isMovingLeft = isPressed;
        break;
    case ctr::KeyAction::MoveRight:
        isMovingRight = isPressed;
        break;
    default:
        break;
    }
}

void Entity_Player::update()
{
    switch(mov_control)
    {
    //if the player is normally moving (no movement skills)
    case MovementControl::inControl:
    {
        //calculate speed
        float mov_speed = (mov_speedBase + mov_speedModifier) * mov_speedMultiplier;

        float mov_velocity_x = 0;
        float mov_velocity_y = 0;

        //add input forces
        if(isMovingUp && !isMovingDown)
        {
            mov_velocity_y = -mov_speed;
        }
        if(isMovingDown && !isMovingUp)
        {
            mov_velocity_y = mov_speed;
        }
        if(isMovingLeft && !isMovingRight)
        {
            mov_velocity_x = -mov_speed;
        }
        if(isMovingRight && !isMovingLeft)
        {
            mov_velocity_x = mov_speed;
        }

        //find movement vector
        utl::Vector2f mov_velocity;
        if(mov_velocity_x != 0 && mov_velocity_y != 0)
        {
            mov_velocity = (utl::Vector2f(mov_velocity_x, mov_velocity_y) / std::sqrt(mov_velocity_x * mov_velocity_x + mov_velocity_y * mov_velocity_y)) * mov_speed;
        }
        else
        {
            mov_velocity = utl::Vector2f(mov_velocity_x, mov_velocity_y);
        }


        //add external forces
        if(mov_forces_external.size() > 0)
        {
            for(auto& force : mov_forces_external)
            {
                mov_velocity += force;
            }
        }

        //move player
        attemptMove(*this, mov_velocity);
        previousVelocity = std::move(mov_velocity);

        //reset forces
        mov_forces_external.clear();
    }
    break;
    case MovementControl::skillControl:
    {

    }

    default:
    {

    }
    break;
    }
}

void Entity_Player::mouseInput(utl::Vector2f mousePosition)
{
    //temp
    float angleToMouse = std::atan2(mousePosition.y - 540, mousePosition.x - 960);
    angleToMouse = angleToMouse * 180 / 3.141592;
    if(angleToMouse < 0)
    {
        angleToMouse = angleToMouse * -1;
    }
    else
    {
        angleToMouse = angleToMouse - 360;
        angleToMouse = angleToMouse * -1;
    }

    //temp
    int animationDirection;
    if(337.5<angleToMouse || angleToMouse<22.5)
    {
        animationDirection = 4;
    }
    else if(22.5<angleToMouse && angleToMouse<67.5)
    {
        animationDirection = 6;
    }
    else if(67.5<angleToMouse && angleToMouse<112.5)
    {
        animationDirection = 0;
    }
    else if(112.5<angleToMouse && angleToMouse<157.5)
    {
        animationDirection = 7;
    }
    else if(157.5<angleToMouse && angleToMouse<202.5)
    {
        animationDirection = 5;
    }
    else if(202.5<angleToMouse && angleToMouse<247.5)
    {
        animationDirection = 3;
    }
    else if(247.5<angleToMouse && angleToMouse<292.5)
    {
        animationDirection = 1;
    }
    else
    {
        animationDirection = 2;
    }

    spriteSheetIndex.x = animationDirection;
}

void Entity_Player::draw(sf::RenderWindow& window, const DrawLayer& drawLayer)
{
    spriteSheet.getSprite(spriteSheetIndex.x, spriteSheetIndex.y).setPosition(coords.sf());
    window.draw(spriteSheet.getSprite(spriteSheetIndex.x,spriteSheetIndex.y));
}
