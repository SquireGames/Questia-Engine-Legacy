#include "QuestiaEng/StateManager/States/State_OptionsMenu/State_OptionsMenu.h"

State_OptionsMenu::State_OptionsMenu(sf::RenderWindow &window):
	window(window)
	, resourceManager()
	, guiManager(window, resourceManager)
	, optionManager(&guiManager)
	, saveFile()
	, activeSelector("displayList")
	, key_up_isPressed(false)
	, key_down_isPressed(false)
{
	///gui
	//font
	guiManager.setFont(Data_Desktop::getInstance().font1);
	//loader
	guiLoader.setGuiPack(saveFile.getGuiPack());
	guiLoader.loadGui(guiManager, "options");

	///options
	///display
	//windowMode
	option_windowMode.setType(OptionType::choice);
	option_windowMode.setList("displayList");
	option_windowMode.setOptionName(saveFile.getWindowMode_name());
	option_windowMode.addChoice(std::make_pair("Fullscreen", 0));
	option_windowMode.addChoice(std::make_pair("Windowed",   1));
	//fps
	option_fps.setType(OptionType::choice);
	option_fps.setList("displayList");
	option_fps.setOptionName(saveFile.getFps_name());
	option_fps.addChoice(std::make_pair("V-Sync", 0));
	option_fps.addChoice(std::make_pair("60  Fps", 60));
	option_fps.addChoice(std::make_pair("128 Fps", 128));
	option_fps.addChoice(std::make_pair("144 Fps", 144));
	option_fps.addChoice(std::make_pair("256 Fps", 256));
	option_fps.addChoice(std::make_pair("Unlimited", -1));
	//font
	option_font.setType(OptionType::choice);
	option_font.setList("displayList");
	option_font.setOptionName(saveFile.getFont_name());
	option_font.addChoice(std::make_pair("Lato Regular", "Lato-Regular.ttf"));
	option_font.addChoice(std::make_pair("Lato Medium", "Lato-Medium.ttf"));
	option_font.addChoice(std::make_pair("Open Sans Regular", "OpenSans-Regular.ttf"));
	option_font.addChoice(std::make_pair("acidstructure", "acidstructure.ttf"));
	//guiPack
	option_guiPack.setType(OptionType::choice);
	option_guiPack.setList("displayList");
	option_guiPack.setOptionName(saveFile.getGuiPack_name());
	for(const auto& it : utl::getFiles("Data/Gui Pack", false))
	{
		option_guiPack.addChoice(std::make_pair(it, it));
	}

	///audio
	//music volume
	option_musicVolume.setType(OptionType::choice);
	option_musicVolume.setList("audioList");
	option_musicVolume.setOptionName(saveFile.getMusicVolume_name());
	option_musicVolume.setChoices( {std::make_pair("Muted", 0), std::make_pair("10%", 10), std::make_pair("20%", 20), std::make_pair("30%", 30), std::make_pair("40%", 40), std::make_pair("50%", 50),
	                                std::make_pair("60%", 60), std::make_pair("70%", 70), std::make_pair("80%", 80), std::make_pair("90%", 90), std::make_pair("100%", 100)
	                               });

	///input
	//move up key
	option_key_moveUp.setType(OptionType::input);
	option_key_moveUp.setList("inputList");
	option_key_moveUp.setOptionName(saveFile.getKey_moveUp_name());
	//move down key
	option_key_moveDown.setType(OptionType::input);
	option_key_moveDown.setList("inputList");
	option_key_moveDown.setOptionName(saveFile.getKey_moveDown_name());
	//move left key
	option_key_moveLeft.setType(OptionType::input);
	option_key_moveLeft.setList("inputList");
	option_key_moveLeft.setOptionName(saveFile.getKey_moveLeft_name());
	//move right key
	option_key_moveRight.setType(OptionType::input);
	option_key_moveRight.setList("inputList");
	option_key_moveRight.setOptionName(saveFile.getKey_moveRight_name());
	//skill 1 key
	option_key_skill_1.setType(OptionType::input);
	option_key_skill_1.setList("inputList");
	option_key_skill_1.setOptionName(saveFile.getKey_skill_1_name());
	//skill 2 key
	option_key_skill_2.setType(OptionType::input);
	option_key_skill_2.setList("inputList");
	option_key_skill_2.setOptionName(saveFile.getKey_skill_2_name());
	//skill 3 key
	option_key_skill_3.setType(OptionType::input);
	option_key_skill_3.setList("inputList");
	option_key_skill_3.setOptionName(saveFile.getKey_skill_3_name());
	//skill 4 key
	option_key_skill_4.setType(OptionType::input);
	option_key_skill_4.setList("inputList");
	option_key_skill_4.setOptionName(saveFile.getKey_skill_4_name());
	//skill 5 key
	option_key_skill_5.setType(OptionType::input);
	option_key_skill_5.setList("inputList");
	option_key_skill_5.setOptionName(saveFile.getKey_skill_5_name());
	//skill 6 key
	option_key_skill_6.setType(OptionType::input);
	option_key_skill_6.setList("inputList");
	option_key_skill_6.setOptionName(saveFile.getKey_skill_6_name());

	//starting values
	option_windowMode.init(saveFile.getWindowMode());
	option_fps.init(saveFile.getFps());
	option_font.init(saveFile.getFont());
	option_musicVolume.init(saveFile.getMusicVolume());
	option_guiPack.init(saveFile.getGuiPack());
	option_key_moveUp.init(static_cast<ctr::Input>(saveFile.getKey_moveUp()));
	option_key_moveDown.init(static_cast<ctr::Input>(saveFile.getKey_moveDown()));
	option_key_moveLeft.init(static_cast<ctr::Input>(saveFile.getKey_moveLeft()));
	option_key_moveRight.init(static_cast<ctr::Input>(saveFile.getKey_moveRight()));
	option_key_skill_1.init(static_cast<ctr::Input>(saveFile.getKey_skill_1()));
	option_key_skill_2.init(static_cast<ctr::Input>(saveFile.getKey_skill_2()));
	option_key_skill_3.init(static_cast<ctr::Input>(saveFile.getKey_skill_3()));
	option_key_skill_4.init(static_cast<ctr::Input>(saveFile.getKey_skill_4()));
	option_key_skill_5.init(static_cast<ctr::Input>(saveFile.getKey_skill_5()));
	option_key_skill_6.init(static_cast<ctr::Input>(saveFile.getKey_skill_6()));

	//storing
	optionManager.addOption(&option_windowMode);
	optionManager.addOption(&option_fps);
	optionManager.addOption(&option_font);
	optionManager.addOption(&option_musicVolume);
	optionManager.addOption(&option_guiPack);
	optionManager.addOption(&option_key_moveUp);
	optionManager.addOption(&option_key_moveDown);
	optionManager.addOption(&option_key_moveLeft);
	optionManager.addOption(&option_key_moveRight);
	optionManager.addOption(&option_key_skill_1);
	optionManager.addOption(&option_key_skill_2);
	optionManager.addOption(&option_key_skill_3);
	optionManager.addOption(&option_key_skill_4);
	optionManager.addOption(&option_key_skill_5);
	optionManager.addOption(&option_key_skill_6);

	//initialization
	optionManager.initLists();

	///hiding of unneeded lists
	guiManager.setListAtr("displayList", gui::ButtonCharacteristic::isVisible, true);
	guiManager.setListAtr("audioList"  , gui::ButtonCharacteristic::isVisible, false);
	guiManager.setListAtr("gameList"   , gui::ButtonCharacteristic::isVisible, false);
	guiManager.setListAtr("inputList"  , gui::ButtonCharacteristic::isVisible, false);
}

State_OptionsMenu::~State_OptionsMenu()
{
	std::cout << "Options Menu Destroyed" << std::endl;
}

void State_OptionsMenu::processImput(sf::Keyboard::Key key, bool isPressed)
{
	if(key == sf::Keyboard::Up)
	{
		key_up_isPressed = isPressed;
	}
	else if(key == sf::Keyboard::Down)
	{
		key_down_isPressed = isPressed;
	}
	if(isPressed)
	{
		optionManager.handleInput(static_cast<ctr::Input>(key));
	}
}

void State_OptionsMenu::update(sf::Time elapsedTime)
{
	///update mouse
	guiManager.setMousePosition(std::make_pair(Data_Desktop::getInstance().getScaledMousePosition(window).x,Data_Desktop::getInstance().getScaledMousePosition(window).y));

	///scroll input
	if(!optionManager.isMouseOverAssignedInput())
	{
		if(key_up_isPressed)
		{
			guiManager.setListAtr(activeSelector, gui::ButtonCharacteristic::addToScroll_y, 15);
		}
		if(key_down_isPressed)
		{
			guiManager.setListAtr(activeSelector, gui::ButtonCharacteristic::addToScroll_y, -15);
		}
	}
	const int mouseScroll = Data_Desktop::getInstance().getMouseWheelDelta();
	if(mouseScroll == 1)
	{
		guiManager.setListAtr(activeSelector, gui::ButtonCharacteristic::addToScroll_y, 40);
	}
	else if(mouseScroll == -1)
	{
		guiManager.setListAtr(activeSelector, gui::ButtonCharacteristic::addToScroll_y, -40);
	}

	//mouse input for key binding
	optionManager.checkMouseInput();
	if(Data_Desktop::getInstance().isMouseReleased())
	{
		optionManager.setMouseReleased();
	}

	///gui check
	if(Data_Desktop::getInstance().isMousePressed())
	{
		if(optionManager.handleGui()) {}

		//switches
		if(guiManager.isClicked("displaySwitch"))
		{
			activeSelector = "displayList";
			guiManager.setListAtr("displayList", gui::ButtonCharacteristic::isVisible, true);
			guiManager.setListAtr("audioList",   gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("gameList",    gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("inputList",   gui::ButtonCharacteristic::isVisible, false);
		}
		else if(guiManager.isClicked("audioSwitch"))
		{
			activeSelector = "audioList";
			guiManager.setListAtr("displayList", gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("audioList",   gui::ButtonCharacteristic::isVisible, true);
			guiManager.setListAtr("gameList",    gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("inputList",   gui::ButtonCharacteristic::isVisible, false);
		}
		else if(guiManager.isClicked("gameSwitch"))
		{
			activeSelector = "gameList";
			guiManager.setListAtr("displayList", gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("audioList",   gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("gameList",    gui::ButtonCharacteristic::isVisible, true);
			guiManager.setListAtr("inputList",   gui::ButtonCharacteristic::isVisible, false);
		}
		else if(guiManager.isClicked("inputSwitch"))
		{
			activeSelector = "inputList";
			guiManager.setListAtr("displayList", gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("audioList",   gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("gameList",    gui::ButtonCharacteristic::isVisible, false);
			guiManager.setListAtr("inputList",   gui::ButtonCharacteristic::isVisible, true);
		}
		//regular
		else if(guiManager.isClicked("mainMenu"))
		{
			//StateManager::getInstance().changeeState(new State_Transition(window, GameState::State_MainMenu));
		}
		else if(guiManager.isClicked("apply"))
		{
			optionManager.saveOptions(saveFile);
			saveFile.writeOptions();

			//window modes
			if(option_windowMode.isChanged())
			{
				switch(option_windowMode.getValue())
				{
				case 0:
					window.create(sf::VideoMode(1920, 1080),"QuestiaEng",sf::Style::Fullscreen);
					break;
				case 1:
					window.create(sf::VideoMode(1280, 720),"QuestiaEng",sf::Style::Default);
					break;
				}
				Data_Desktop::getInstance().setDesktopResolution(sf::Vector2i(window.getSize().x,window.getSize().y),window.getPosition());
			}
			//fps
			if(option_fps.getValue() == 0)
			{
				window.setVerticalSyncEnabled(true);
			}
			else if(option_fps.getValue() == -1)
			{
				window.setFramerateLimit(100000);
				window.setVerticalSyncEnabled(false);
			}
			else
			{
				window.setFramerateLimit(option_fps.getValue());
				window.setVerticalSyncEnabled(false);
			}
			//font
			if(option_font.isChanged())
			{
				Data_Desktop::getInstance().changeFont(option_font.getValue());
			}
			//music
			//...

			//StateManager::getInstance().changeeState(new State_Transition(window, GameState::State_OptionsMenu));
		}
	}
}

void State_OptionsMenu::displayTextures()
{
	guiManager.drawButtons();
}
