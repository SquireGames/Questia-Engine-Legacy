#include "QuestiaEng/StateManager/States/State_TileMapEditor.h"

///ctor + dtor

State_TileMapEditor::State_TileMapEditor(sf::RenderWindow &window, std::string mapName):
    window(window)
    , resourceManager()
    , tileEngineEditor(window, resourceManager)
    , guiManager(window, resourceManager)
    , saveFile_options()
    , window_newMap(guiManager, guiLoader)
    , window_openMap(guiManager, guiLoader)
    , window_saveAs(guiManager, guiLoader)
{
    ///gui
    //font
    guiManager.setFont(Data_Desktop::getInstance().font1);
    //loader
    guiLoader.setGuiPack(saveFile_options.getGuiPack());
    guiLoader.loadGui(guiManager, "tileMapEditor");

    //map view
    mapView.setSize(1920,1080);
    mapView.setCenter(0,0);

    //tile view
    tileView.setSize(1920,1080);
    tileView.setCenter(0,0);

    //gui view
    overlayView.setSize(1920,1080);
    overlayView.setCenter(1920/2,1080/2);

    //load map
    if(mapName == "NULL")
    {
        tileEngineEditor.createMap("NULL", 1, 1, 1);
        tileEngineEditor.loadMap(mapName);
        showGridLines = utl::Toggler(false);
        showHover = false;
    }
    else
    {
        tileEngineEditor.loadMap(mapName);
    }

    tileEngineEditor.setPosition(0,0);

    //map data
    guiManager.setButtonAtr("mapDataText", "width", gui::ButtonAtrCharacteristic::text, utl::asString(tileEngineEditor.getMapWidth()));
    guiManager.setButtonAtr("mapDataText", "height", gui::ButtonAtrCharacteristic::text, utl::asString(tileEngineEditor.getMapHeight()));
    guiManager.setButtonAtr("mapDataText", "layers", gui::ButtonAtrCharacteristic::text, utl::asString(tileEngineEditor.getMapLayers()));

    //layers
    for(unsigned int it = 0; it != tileEngineEditor.getMapLayers(); it++)
    {
        std::string groupName    = guiManager.createListEntry("layers");
        std::string bottomButton = guiManager.getGroupEntry(groupName, "layerButtonTemplate");
        std::string topButton    = guiManager.getGroupEntry(groupName, "layerSelectionTemplate");

        guiManager.setButtonAtr(bottomButton, "buttonText", gui::ButtonAtrCharacteristic::text, " " + utl::asString(it));

        layerData.push_back(LayerData(bottomButton, topButton));
    }

    //main buttons
    //file
    file_new    = guiManager.getGroupEntry(guiManager.createListEntry("stack_file"), "stackButtonTemplate");
    guiManager.setButtonAtr(file_new, "buttonText", gui::ButtonAtrCharacteristic::text, "New map");
    file_open   = guiManager.getGroupEntry(guiManager.createListEntry("stack_file"), "stackButtonTemplate");
    guiManager.setButtonAtr(file_open, "buttonText", gui::ButtonAtrCharacteristic::text, "Open map");
    file_save   = guiManager.getGroupEntry(guiManager.createListEntry("stack_file"), "stackButtonTemplate");
    guiManager.setButtonAtr(file_save, "buttonText", gui::ButtonAtrCharacteristic::text, "Save");
    file_saveAs = guiManager.getGroupEntry(guiManager.createListEntry("stack_file"), "stackButtonTemplate");
    guiManager.setButtonAtr(file_saveAs, "buttonText", gui::ButtonAtrCharacteristic::text, "Save as");
    guiManager.setListAtr("stack_file", gui::ButtonCharacteristic::isVisible, false);
    //edit
    edit_undo = guiManager.getGroupEntry(guiManager.createListEntry("stack_edit"), "stackButtonTemplate");
    guiManager.setButtonAtr(edit_undo, "buttonText", gui::ButtonAtrCharacteristic::text, "TODO: Undo");
    edit_redo = guiManager.getGroupEntry(guiManager.createListEntry("stack_edit"), "stackButtonTemplate");
    guiManager.setButtonAtr(edit_redo, "buttonText", gui::ButtonAtrCharacteristic::text, "TODO: Redo");
    guiManager.setListAtr("stack_edit", gui::ButtonCharacteristic::isVisible, false);
    //view
    view_grid = guiManager.getGroupEntry(guiManager.createListEntry("stack_view"), "stackButtonTemplate");
    guiManager.setButtonAtr(view_grid, "buttonText", gui::ButtonAtrCharacteristic::text, "Toggle Grid");
    guiManager.setListAtr("stack_view", gui::ButtonCharacteristic::isVisible, false);


    //'select' layer 0
    LayerData& changeLayer = layerData.at(0);
    guiManager.setButtonAtr(changeLayer.topButton, "buttonSprite", gui::ButtonAtrCharacteristic::color, sf::Color(0,255,0,38));


    ///query windows
    window_newMap.addQuery("map_name", "Map Name: ", QueryWindow::QueryType::Input_string);
    window_newMap.addQuery("map_x", "Width: ",  QueryWindow::QueryType::Input_int);
    window_newMap.addQuery("map_y", "Height: ", QueryWindow::QueryType::Input_int);
    window_newMap.addQuery("map_z", "Layers: ", QueryWindow::QueryType::Input_int);
    window_newMap.initQuery("New Map");

    for(auto& it : utl::getFiles("Maps", false))
    {
        if(it != "NULL")
        {
            window_openMap.addQuery("choice", it, QueryWindow::QueryType::Choice_string);
        }
    }
    window_openMap.initQuery("Open Map");

    window_saveAs.addQuery("map_name", "Map Name: ",QueryWindow::QueryType::Input_string);
    window_saveAs.initQuery("Save As");

    windows.push_back(&window_newMap);
    windows.push_back(&window_openMap);
    windows.push_back(&window_saveAs);
}
State_TileMapEditor::~State_TileMapEditor()
{
    //dtor
}

///state functions

void State_TileMapEditor::processImput(sf::Keyboard::Key key, bool isPressed)
{
    pendingInput = true;
    if(key == sf::Keyboard::Escape && isPressed)
    {
        tileEngineEditor.overrideMap();

        //StateManager::getInstance().changeeState(new State_Transition(window, GameState::State_MainMenu));
    }
    else if(key == sf::Keyboard::W)
    {
        is_key_up = isPressed;
    }
    else if(key == sf::Keyboard::S)
    {
        is_key_down = isPressed;
    }
    else if(key == sf::Keyboard::A)
    {
        is_key_left = isPressed;
    }
    else if(key == sf::Keyboard::D)
    {
        is_key_right = isPressed;
    }

    else if(key == sf::Keyboard::Subtract || key == sf::Keyboard::Dash)
    {
        is_key_minus = isPressed;
    }
    else if(key == sf::Keyboard::Add || key == sf::Keyboard::Equal)
    {
        is_key_plus = isPressed;
    }
    //layers
    else if(isPressed)
    {
        if(key == sf::Keyboard::Tilde)
        {
            setLayerSelection(0);
        }
        for(unsigned int keyIt = sf::Keyboard::Num0; keyIt < (sf::Keyboard::Num0 + tileEngineEditor.getMapLayers()); keyIt++)
        {
            if(key == keyIt)
            {
                setLayerSelection(keyIt - sf::Keyboard::Num0);
                break;
            }
        }
    }
}
void State_TileMapEditor::update(sf::Time)
{
    mouseListener.update();
    executor.processTasks();

    //gui
    guiManager.setMousePosition(std::make_pair(Data_Desktop::getInstance().getScaledMousePosition(window).x,Data_Desktop::getInstance().getScaledMousePosition(window).y));

    //tickers
    ticker_overlayToggle.tick();
    for(LayerData& layer : layerData)
    {
        layer.ticker.tick();
    }

    //handling states
    switch(editorState)
    {
    case EditorState::Idle:
        updateState_idle();
        break;
    case EditorState::Tile:
        updateState_tile();
        break;
    case EditorState::View:
        updateState_view();
        break;
    default:
        editorState = EditorState::Idle;
        break;
    }
}
void State_TileMapEditor::displayTextures()
{
    //handling states
    switch(editorState)
    {
    case EditorState::Idle:
    {
        window.setView(mapView);

        //draw map
        for(unsigned int it = 0; it != layerData.size(); it++)
        {
            tileEngineEditor.drawLayer(it, layerData.at(it).alpha);
        }
        tileEngineEditor.resetTileAlpha();

        //draw grid
        if(showGridLines.getState())
        {
            tileEngineEditor.drawGridLines();
        }

        //show selected tile

        switch(selectionType)
        {
        case SelectionType::none:
            break;
        case SelectionType::tile:
            guiManager.setButtonAtr("selectedTile", "text", gui::ButtonAtrCharacteristic::text, "Selected: (" + utl::asString(selectedTile.x) + ", " + utl::asString(selectedTile.y) + ")");
            if(!(selectedTile.x < 0) && !(selectedTile.y < 0) && !(selectedTile.x > tileEngineEditor.getMapWidth() - 1) && !(selectedTile.y > tileEngineEditor.getMapHeight() - 1))
            {
                if(!isWindowActive && showHover)
                {
                    tileEngineEditor.hoverTile(selectedTile.x, selectedTile.y);
                }
            }
            break;
        case SelectionType::span:
            guiManager.setButtonAtr("selectedTile", "text", gui::ButtonAtrCharacteristic::text, "Selected: (" + utl::asString(selectedTile.x) + ", " + utl::asString(selectedTile.y) + ")" + "("
                                    + utl::asString(selectionSize.x) + ", " + utl::asString(selectionSize.y) + ")");
            if(!(selectedTile.x < 0) && !(selectedTile.y < 0) && !(selectedTile.x > tileEngineEditor.getMapWidth() - 1) && !(selectedTile.y > tileEngineEditor.getMapHeight() - 1))
            {
                if(!isWindowActive && showHover)
                {
                    tileEngineEditor.hoverSpan(selectedTile.x, selectedTile.y, selectionSize.x, selectionSize.y);
                }
            }
            break;
        }

        //debug
        guiManager.setButtonAtr("fpsText", "fps", gui::ButtonAtrCharacteristic::text, utl::asString(Data_Desktop::getInstance().get_FPS()));
    }
    break;
    case EditorState::View:
    {
        window.setView(mapView);
        tileEngineEditor.drawMap();
    }
    break;
    case EditorState::Tile:
    {
        window.setView(tileView);

        //draw tiles
        tileEngineEditor.drawTiles();


        //show selected tile
        if(currentTile == nullptr)
        {
            guiManager.setButtonAtr("selectedTile", "text", gui::ButtonAtrCharacteristic::text, std::string("Selected: \"") + "NONE" + "\"");
        }
        else
        {
            guiManager.setButtonAtr("selectedTile", "text", gui::ButtonAtrCharacteristic::text, std::string("Selected: \"") + currentTile->source + "\"");
            sf::Sprite tempSprite = sf::Sprite(resourceManager.getTexture(currentTile->source));
        }

        //debug
        guiManager.setButtonAtr("fpsText", "fps", gui::ButtonAtrCharacteristic::text, utl::asString(Data_Desktop::getInstance().get_FPS()));
    }
    break;
    default:
        editorState = EditorState::Idle;
        break;
    }

    //gui
    window.setView(overlayView);
    guiManager.drawButtons();
}

///states

void State_TileMapEditor::updateState_view()
{
    //handle gui elements
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if(guiManager.isClicked("overlayToggle"))
        {
            if(ticker_overlayToggle.isDone())
            {
                editorState = EditorState::Idle;
                setOverlayStatus(true);
            }
        }
    }

    //handle movement and zoom
    moveCamera_map();
}
void State_TileMapEditor::updateState_idle()
{
    //get tile hover position
    utl::Vector2f mousePos = Data_Desktop::getInstance().getScaledMousePosition(window);
    mousePos = (mousePos * mapZoomRatio) - (utl::Vector2f(1920/2, 1080/2) * mapZoomRatio) + utl::Vector2f(mapView.getCenter());
    mousePos = mousePos / 64.f;
    mousePos.x = std::floor(mousePos.x);
    mousePos.y = std::floor(mousePos.y);
    //selection area
    switch(selectionType)
    {
    case SelectionType::none:
        break;
    case SelectionType::tile:
        selectedTile = utl::Vector2i(mousePos.x, mousePos.y);
        break;
    case SelectionType::span:
        if(mouseListener.isMousePressed(sf::Mouse::Left))
        {
            selectedTile = utl::Vector2i(mousePos.x, mousePos.y);
            selectionSize = utl::Vector2i(1,1);
        }
        else if(mouseListener.isMouseReleased(sf::Mouse::Left)  || mouseListener.isMouseHeld(sf::Mouse::Left))
        {
            selectionSize = utl::Vector2i(mousePos.x - selectedTile.x, mousePos.y - selectedTile.y);

            //add the sign to the value
            selectionSize.x += (selectionSize.x >= 0) - (selectionSize.x < 0);
            selectionSize.y += (selectionSize.y >= 0) - (selectionSize.y < 0);
        }
        break;
    }


    //prioritize window input
    for(auto& queryWindow : windows)
    {
        if(queryWindow->isActive())
        {
            isWindowActive = true;
            if(pendingInput)
            {
                queryWindow->checkInput(sf::Mouse::isButtonPressed(sf::Mouse::Left), Data_Desktop::getInstance().getMostRecentChar());
            }
            else
            {
                queryWindow->checkInput(sf::Mouse::isButtonPressed(sf::Mouse::Left), -1);
            }
            pendingInput = false;

            if(queryWindow->isDone())
            {
                handleQueryWindow(queryWindow);
            }
            return;
        }
    }
    isWindowActive = false;
    Data_Desktop::getInstance().getMostRecentChar();
    pendingInput = false;

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        //gui elements
        if(guiManager.isClicked("overlayToggle"))
        {
            if(ticker_overlayToggle.isDone())
            {
                editorState = EditorState::View;
                setOverlayStatus(false);
            }
        }
        if(guiManager.isClicked("b_tileScreen"))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                editorState = EditorState::Tile;
                overlayToggler = utl::Toggler(true);
                guiManager.setListAtr("layers", gui::ButtonCharacteristic::isVisible, false);
            }
        }
        //layers
        for(unsigned int layerIt = 0; layerIt != layerData.size(); layerIt++)
        {
            LayerData& layer = layerData.at(layerIt);

            if(guiManager.isClicked(layer.bottomButton))
            {
                setLayerSelection(layerIt);
                break;
            }
        }

        //selection
        if(guiManager.isClicked("selectionSwitch_none"))
        {
            guiManager.setButtonAtr("selectionSwitch_none", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 100);
            guiManager.setButtonAtr("selectionSwitch_tile", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 0);
            guiManager.setButtonAtr("selectionSwitch_span", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 0);
            selectionType = SelectionType::none;
        }
        if(guiManager.isClicked("selectionSwitch_tile"))
        {
            guiManager.setButtonAtr("selectionSwitch_none", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 0);
            guiManager.setButtonAtr("selectionSwitch_tile", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 100);
            guiManager.setButtonAtr("selectionSwitch_span", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 0);
            selectionType = SelectionType::tile;
        }
        if(guiManager.isClicked("selectionSwitch_span"))
        {
            guiManager.setButtonAtr("selectionSwitch_none", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 0);
            guiManager.setButtonAtr("selectionSwitch_tile", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 0);
            guiManager.setButtonAtr("selectionSwitch_span", "buttonPercent", gui::ButtonAtrCharacteristic::percentage, 100);
            selectionType = SelectionType::span;
        }

        //tile change
        if(currentTileID != -7
           && Data_Desktop::getInstance().getScaledMousePosition(window).y > 30 && Data_Desktop::getInstance().getScaledMousePosition(window).y < (1080-30)
           && selectionType != SelectionType::none)
        {
            tileEngineEditor.replaceTile(currentTileID, selectedTile.x, selectedTile.y, selectedLayer);
        }
    }
    if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        for(unsigned int layerIt = 0; layerIt != layerData.size(); layerIt++)
        {
            LayerData& layer = layerData.at(layerIt);

            if(guiManager.isClicked(layer.bottomButton))
            {
                advanceLayerState(layerIt);
                break;
            }
        }

        if(Data_Desktop::getInstance().getScaledMousePosition(window).y > 30 && Data_Desktop::getInstance().getScaledMousePosition(window).y < (1080-30))
        {
            tileEngineEditor.replaceTile(0, selectedTile.x, selectedTile.y, selectedLayer);
        }
    }

    checkMainButtons();

    //handle movement and zoom
    moveCamera_map();
}
void State_TileMapEditor::updateState_tile()
{
    //get tile hover position
    utl::Vector2f mousePos = Data_Desktop::getInstance().getScaledMousePosition(window);
    mousePos = (mousePos * tileZoomRatio) - (utl::Vector2f(1920/2, 1080/2) * tileZoomRatio) + utl::Vector2f(tileView.getCenter());
    mousePos = mousePos / 64.f;
    selectedTile = utl::Vector2i(mousePos.x, mousePos.y);
    if(selectedTile.x >= 0 && selectedTile.y >= 0)
    {
        tileEngineEditor.hoverTile(selectedTile.x, selectedTile.y);
    }

    //handle gui elements
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if(guiManager.isClicked("overlayToggle"))
        {
            if(ticker_overlayToggle.isDone())
            {
                setOverlayStatus(overlayToggler.toggle());
            }
        }
        if(guiManager.isClicked("b_mapScreen"))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                editorState = EditorState::Idle;
                overlayToggler = utl::Toggler(true);
                guiManager.setListAtr("layers", gui::ButtonCharacteristic::isVisible, true);
            }
        }

        //tiles
        if(tileEngineEditor.getTile_tileState(selectedTile.x, selectedTile.y) != nullptr)
        {
            if(!overlayToggler.getState() ||
                    (overlayToggler.getState() && Data_Desktop::getInstance().getScaledMousePosition(window).y > 30 && Data_Desktop::getInstance().getScaledMousePosition(window).y < (1080-30)))
            {
                currentTile = tileEngineEditor.getTile_tileState(selectedTile.x, selectedTile.y);
                currentTileID = tileEngineEditor.getTileID(currentTile->source);
            }
        }
    }
    moveCamera_tiles();
}

///camera

void State_TileMapEditor::moveCamera_map()
{
    //zoom
    const int mouseScroll = Data_Desktop::getInstance().getMouseWheelDelta() - is_key_minus + is_key_plus;

    if(mouseScroll == 1)
    {
        mapView.zoom(0.95);
        tileEngineEditor.setViewportSize(mapView.getSize().x, mapView.getSize().y);
    }
    else if(mouseScroll == -1)
    {
        mapView.zoom(1.05);
        tileEngineEditor.setViewportSize(mapView.getSize().x, mapView.getSize().y);
    }

    //get ratio
    mapZoomRatio = (mapView.getSize().x / 1920.f);

    //calculate movement speed
    moveSpeedModifier = std::max(mapZoomRatio, 0.5f);
    float moveDistance = moveSpeed * moveSpeedModifier;

    //movement
    if(is_key_up)
    {
        cameraPosition_map.y -= moveDistance;
    }
    if(is_key_down)
    {
        cameraPosition_map.y += moveDistance;
    }
    if(is_key_left)
    {
        cameraPosition_map.x -= moveDistance;
    }
    if(is_key_right)
    {
        cameraPosition_map.x += moveDistance;
    }

    //updating position
    mapView.setCenter(cameraPosition_map.x, cameraPosition_map.y);
    tileEngineEditor.setPosition(cameraPosition_map.x, cameraPosition_map.y);
}
void State_TileMapEditor::moveCamera_tiles()
{
    //zoom
    const int mouseScroll = Data_Desktop::getInstance().getMouseWheelDelta();
    if(mouseScroll == 1)
    {
        tileView.zoom(0.95);
    }
    else if(mouseScroll == -1)
    {
        tileView.zoom(1.05);
    }

    //get ratio
    tileZoomRatio = (tileView.getSize().x / 1920.f);

    //calculate movement speed
    moveSpeedModifier = std::max(tileZoomRatio, 0.5f);
    float moveDistance = moveSpeed * moveSpeedModifier;

    //movement
    if(is_key_up)
    {
        cameraPosition_tile.y -= moveDistance;
    }
    if(is_key_down)
    {
        cameraPosition_tile.y += moveDistance;
    }
    if(is_key_left)
    {
        cameraPosition_tile.x -= moveDistance;
    }
    if(is_key_right)
    {
        cameraPosition_tile.x += moveDistance;
    }

    //updating position
    tileView.setCenter(cameraPosition_tile.x, cameraPosition_tile.y);
}

///handle queryWindows
void State_TileMapEditor::handleQueryWindow(QueryWindow* qWindow)
{
    if(qWindow->isWindow("New Map"))
    {
        std::string mapName;
        int x, y, z;

        //get window data
        mapName = qWindow->getResult_string("map_name");
        x = std::max(1, qWindow->getResult_int("map_x"));
        y = std::max(1, qWindow->getResult_int("map_y"));
        z = std::max(1, qWindow->getResult_int("map_z"));

        //create map
        tileEngineEditor.createMap(mapName, x, y, z);

        Data_Desktop::getInstance().mapToEdit = mapName;
        //StateManager::getInstance().changeeState(new State_Transition(window, GameState::State_TileMapEditor));
    }
    else if(qWindow->isWindow("Open Map"))
    {
        Data_Desktop::getInstance().mapToEdit = qWindow->getChoice_string();
        //StateManager::getInstance().changeeState(new State_Transition(window, GameState::State_TileMapEditor));
    }
    else if(qWindow->isWindow("Save As"))
    {
        tileEngineEditor.changeMapName(qWindow->getResult_string("map_name"));
        tileEngineEditor.createMap(qWindow->getResult_string("map_name"), tileEngineEditor.getMapWidth(), tileEngineEditor.getMapHeight(), tileEngineEditor.getMapLayers());
        tileEngineEditor.overrideMap();
        Data_Desktop::getInstance().mapToEdit = qWindow->getResult_string("map_name");
        //StateManager::getInstance().changeeState(new State_Transition(window, GameState::State_TileMapEditor));
    }
}

///gui
void State_TileMapEditor::checkMainButtons()
{
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if(guiManager.isClicked(file_new))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                window_newMap.setActive(true);
            }
        }
        else if(guiManager.isClicked(file_open))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                window_openMap.setActive(true);
            }
        }
        else if(guiManager.isClicked(file_save))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                //save map
                tileEngineEditor.overrideMap();
                //display that map was saved
                executor.tryDelete("showSaveText");
                guiManager.setButton("savedMap", gui::ButtonCharacteristic::isVisible, true);
                executor.addTask("showSaveText", utl::Executor::TaskType::Delayed, utl::Ticker(300), [&](float)
                {
                    guiManager.setButton("savedMap", gui::ButtonCharacteristic::isVisible, false);
                });
            }
        }
        else if(guiManager.isClicked(file_saveAs))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                window_saveAs.setActive(true);
            }
        }
        else if(guiManager.isClicked(view_grid))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                showGridLines.toggle();
            }
        }

    }
    if((sf::Mouse::isButtonPressed(sf::Mouse::Left)) || (sf::Mouse::isButtonPressed(sf::Mouse::Right)))
    {
        if(guiManager.isClicked("b_file"))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                if(mainSelection != MainButtonSelection::File)
                {
                    mainSelection = MainButtonSelection::File;
                    stackToggler = utl::Toggler(true);
                    guiManager.setListAtr("stack_file", gui::ButtonCharacteristic::isVisible, true);
                }
                else
                {
                    guiManager.setListAtr("stack_file", gui::ButtonCharacteristic::isVisible, stackToggler.toggle());
                }
                guiManager.setListAtr("stack_edit", gui::ButtonCharacteristic::isVisible, false);
                guiManager.setListAtr("stack_view", gui::ButtonCharacteristic::isVisible, false);
            }
        }
        else if(guiManager.isClicked("b_edit"))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                if(mainSelection != MainButtonSelection::Edit)
                {
                    mainSelection = MainButtonSelection::Edit;
                    stackToggler = utl::Toggler(true);
                    guiManager.setListAtr("stack_edit", gui::ButtonCharacteristic::isVisible, true);
                }
                else
                {
                    guiManager.setListAtr("stack_edit", gui::ButtonCharacteristic::isVisible, stackToggler.toggle());
                }
                guiManager.setListAtr("stack_file", gui::ButtonCharacteristic::isVisible, false);
                guiManager.setListAtr("stack_view", gui::ButtonCharacteristic::isVisible, false);
            }
        }
        else if(guiManager.isClicked("b_view"))
        {
            //TODO add another button ticker here
            if(ticker_overlayToggle.isDone())
            {
                if(mainSelection != MainButtonSelection::View)
                {
                    mainSelection = MainButtonSelection::View;
                    stackToggler = utl::Toggler(true);
                    guiManager.setListAtr("stack_view", gui::ButtonCharacteristic::isVisible, true);
                }
                else
                {
                    guiManager.setListAtr("stack_view", gui::ButtonCharacteristic::isVisible, stackToggler.toggle());
                }
                guiManager.setListAtr("stack_file", gui::ButtonCharacteristic::isVisible, false);
                guiManager.setListAtr("stack_edit", gui::ButtonCharacteristic::isVisible, false);
            }
        }
        else
        {
            mainSelection = MainButtonSelection::None;
            stackToggler = utl::Toggler(false);
            guiManager.setListAtr("stack_file", gui::ButtonCharacteristic::isVisible, false);
            guiManager.setListAtr("stack_edit", gui::ButtonCharacteristic::isVisible, false);
            guiManager.setListAtr("stack_view", gui::ButtonCharacteristic::isVisible, false);
        }
    }
}

void State_TileMapEditor::setOverlayStatus(bool isVisible)
{
    const std::string buttonName = "overlayToggle";
    const unsigned int animationTicks = 40;

    if(isVisible)
    {
        executor.tryDelete("overlayClose");
        guiManager.setButtonAtr(buttonName, "buttonSprite", gui::ButtonAtrCharacteristic::flip, 'n');

        executor.addTask("overlayOpen", utl::Executor::TaskType::Continuous, utl::Ticker(animationTicks), [&](float taskPercentage)
        {
            guiManager.setButtonAtr("overlay_up",   "sprite", gui::ButtonAtrCharacteristic::percentage, static_cast<int>(100.f * taskPercentage));
            guiManager.setButtonAtr("overlay_down", "sprite", gui::ButtonAtrCharacteristic::percentage, static_cast<int>(100.f * taskPercentage));

            if(taskPercentage == 1)
            {
                guiManager.setGroupAtr("overlayGroup", gui::ButtonCharacteristic::isVisible, true);
                guiManager.setListAtr("layers", gui::ButtonCharacteristic::isVisible, true);
            }
        });
    }
    else
    {
        executor.tryDelete("overlayOpen");
        guiManager.setButtonAtr(buttonName, "buttonSprite", gui::ButtonAtrCharacteristic::flip, 'y');
        guiManager.setGroupAtr("overlayGroup", gui::ButtonCharacteristic::isVisible, false);
        guiManager.setListAtr("layers", gui::ButtonCharacteristic::isVisible, false);
        guiManager.setListAtr("stack_file", gui::ButtonCharacteristic::isVisible, false);

        executor.addTask("overlayClose", utl::Executor::TaskType::Continuous, utl::Ticker(animationTicks), [&](float taskPercentage)
        {
            guiManager.setButtonAtr("overlay_up",   "sprite", gui::ButtonAtrCharacteristic::percentage, 100 - static_cast<int>(100.f * taskPercentage));
            guiManager.setButtonAtr("overlay_down", "sprite", gui::ButtonAtrCharacteristic::percentage, 100 - static_cast<int>(100.f * taskPercentage));
        });
    }
}

int State_TileMapEditor::advanceLayerState(unsigned int layer)
{
    LayerData& changeLayer = layerData.at(layer);

    if(changeLayer.ticker.isDone())
    {
        changeLayer.alpha = getNextLayerState(changeLayer.alpha);
        if(changeLayer.alpha == 35)
        {
            guiManager.setButtonAtr(changeLayer.topButton, "buttonSprite", gui::ButtonAtrCharacteristic::percentage, 50);
        }
        else
        {
            guiManager.setButtonAtr(changeLayer.topButton, "buttonSprite", gui::ButtonAtrCharacteristic::percentage, changeLayer.alpha);
        }
    }
}

int State_TileMapEditor::getNextLayerState(int alpha)
{
    if(alpha == 100)
    {
        return 35;
    }
    else if(alpha == 35)
    {
        return 0;
    }
    else
    {
        return 100;
    }
}

void State_TileMapEditor::resetLayerStates()
{
    for(LayerData& layer : layerData)
    {
        layer.alpha = 100;
        guiManager.setButtonAtr(layer.topButton, "buttonSprite", gui::ButtonAtrCharacteristic::percentage, 100);
    }
    for(unsigned int it = 0; it != layerData.size(); it++)
    {
        tileEngineEditor.drawLayer(it, 100);
    }
}

void State_TileMapEditor::setLayerSelection(unsigned int layer)
{
    if(selectedLayer != layer)
    {
        LayerData& previousLayer = layerData.at(selectedLayer);
        LayerData& changeLayer = layerData.at(layer);

        selectedLayer = layer;

        guiManager.setButtonAtr(previousLayer.topButton, "buttonSprite", gui::ButtonAtrCharacteristic::color, sf::Color(255,255,0,38));
        guiManager.setButtonAtr(changeLayer.topButton,   "buttonSprite", gui::ButtonAtrCharacteristic::color, sf::Color(0,255,0,38));
    }
}
