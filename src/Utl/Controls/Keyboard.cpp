#include "QuestiaEng/Utl/Controls/Keyboard.h"

std::string ctr::getInputName(const sf::Keyboard::Key& input)
{
    return getInputName(static_cast<ctr::Input>(input));
}
std::string ctr::getInputName(const int& input)
{
    if(input < static_cast<int>(ctr::Input::InputCount))
    {
        return getInputName(static_cast<ctr::Input>(input));
    }
    return "INVALID";
}
std::string ctr::getInputName(const ctr::Input& input)
{
    switch (input)
    {
    case ctr::Input::A:
        return "A";
        break;
    case ctr::Input::B:
        return "B";
        break;
    case ctr::Input::C:
        return "C";
        break;
    case ctr::Input::D:
        return "D";
        break;
    case ctr::Input::E:
        return "E";
        break;
    case ctr::Input::F:
        return "F";
        break;
    case ctr::Input::G:
        return "G";
        break;
    case ctr::Input::H:
        return "H";
        break;
    case ctr::Input::I:
        return "I";
        break;
    case ctr::Input::J:
        return "J";
        break;
    case ctr::Input::K:
        return "K";
        break;
    case ctr::Input::L:
        return "L";
        break;
    case ctr::Input::M:
        return "M";
        break;
    case ctr::Input::N:
        return "N";
        break;
    case ctr::Input::O:
        return "O";
        break;
    case ctr::Input::P:
        return "P";
        break;
    case ctr::Input::Q:
        return "Q";
        break;
    case ctr::Input::R:
        return "R";
        break;
    case ctr::Input::S:
        return "S";
        break;
    case ctr::Input::T:
        return "T";
        break;
    case ctr::Input::U:
        return "U";
        break;
    case ctr::Input::V:
        return "V";
        break;
    case ctr::Input::W:
        return "W";
        break;
    case ctr::Input::X:
        return "X";
        break;
    case ctr::Input::Y:
        return "Y";
        break;
    case ctr::Input::Z:
        return "Z";
        break;
    case ctr::Input::Num0:
        return "0";
        break;
    case ctr::Input::Num1:
        return "1";
        break;
    case ctr::Input::Num2:
        return "2";
        break;
    case ctr::Input::Num3:
        return "3";
        break;
    case ctr::Input::Num4:
        return "4";
        break;
    case ctr::Input::Num5:
        return "5";
        break;
    case ctr::Input::Num6:
        return "6";
        break;
    case ctr::Input::Num7:
        return "7";
        break;
    case ctr::Input::Num8:
        return "8";
        break;
    case ctr::Input::Num9:
        return "9";
        break;
    case ctr::Input::Tilde:
        return "Tilde";
        break;
    case ctr::Input::Dash:
        return "Dash";
        break;
    case ctr::Input::Add:
        return "Add";
        break;
    case ctr::Input::BackSpace:
        return "BackSpace";
        break;
    case ctr::Input::Tab:
        return "Tab";
        break;
    case ctr::Input::LBracket:
        return "LBracket";
        break;
    case ctr::Input::RBracket:
        return "RBracket";
        break;
    case ctr::Input::BackSlash:
        return "BackSlash";
        break;
    case ctr::Input::SemiColon:
        return "SemiColon";
        break;
    case ctr::Input::Quote:
        return "Quote";
        break;
    case ctr::Input::Return:
        return "Return";
        break;
    case ctr::Input::LShift:
        return "LShift";
        break;
    case ctr::Input::Comma:
        return "Comma";
        break;
    case ctr::Input::Period:
        return "Period";
        break;
    case ctr::Input::Slash:
        return "Slash";
        break;
    case ctr::Input::RShift:
        return "RShift";
        break;
    case ctr::Input::LControl:
        return "LControl";
        break;
    case ctr::Input::LAlt:
        return "LAlt";
        break;
    case ctr::Input::Space:
        return "Space";
        break;
    case ctr::Input::RAlt:
        return "RAlt";
        break;
    case ctr::Input::RControl:
        return "RControl";
        break;
    case ctr::Input::Up:
        return "Up";
        break;
    case ctr::Input::Down:
        return "Down";
        break;
    case ctr::Input::Left:
        return "Left";
        break;
    case ctr::Input::Right:
        return "Right";
        break;
    case ctr::Input::Insert:
        return "Insert";
        break;
    case ctr::Input::Home:
        return "Home";
        break;
    case ctr::Input::Delete:
        return "Delete";
        break;
    case ctr::Input::End:
        return "End";
        break;
    case ctr::Input::PageUp:
        return "PageUp";
        break;
    case ctr::Input::PageDown:
        return "PageDown";
        break;
    case ctr::Input::LMouse:
        return "LMouse";
        break;
    case ctr::Input::RMouse:
        return "RMouse";
        break;
    case ctr::Input::MMouse:
        return "MMouse";
        break;
    case ctr::Input::Macro1:
        return "Macro1";
        break;
    case ctr::Input::Macro2:
        return "Macro2";
        break;
    default:
        return "INVALID";
        break;
    }
}

bool ctr::checkInput(const ctr::Input& input)
{
    if(static_cast<int>(input) < static_cast<int>(sf::Keyboard::KeyCount))
    {
        return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(static_cast<int>(input)));
    }
    switch(input)
    {
    case ctr::Input::LMouse:
        return sf::Mouse::isButtonPressed(sf::Mouse::Left);
        break;
    case ctr::Input::RMouse:
        return sf::Mouse::isButtonPressed(sf::Mouse::Right);
        break;
    case ctr::Input::MMouse:
        return sf::Mouse::isButtonPressed(sf::Mouse::Middle);
        break;
    case ctr::Input::Macro1:
        return sf::Mouse::isButtonPressed(sf::Mouse::XButton1);
        break;
    case ctr::Input::Macro2:
        return sf::Mouse::isButtonPressed(sf::Mouse::XButton2);
        break;
    default:
        return false;
        break;
    }
}
