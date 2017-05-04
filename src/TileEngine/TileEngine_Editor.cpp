#include "QuestiaEng/TileEngine/TileEngine_Editor.h"

//ctor
TileEngine_Editor::TileEngine_Editor(sf::RenderWindow& _window, ResourceManager& _resourceManager):
    TileEngine(_window, _resourceManager)
    , gridLines()
{
    gridLines.setPrimitiveType(sf::PrimitiveType::Lines);
}

void TileEngine_Editor::createMap(std::string mapName, unsigned int width, unsigned int height, unsigned int layers)
{
    saveFile.createMap(mapName, width, height, layers);
}

void TileEngine_Editor::loadMap(std::string _mapName)
{
    TileEngine::loadMap(_mapName, TileMap::TextureMode::All, TileMap::TileMode::Sprite);

    //2 vertices
    gridLines.resize((mapWidth + 1) * 2 + ((mapHeight + 1) * 2));
    //vertex count
    unsigned int vertexCount = 0;
    //vertex
    sf::Vertex vertex;
    vertex.color = sf::Color::Black;
    //horizontal lines
    for(unsigned int it = 0; it != (mapHeight + 1); it++)
    {
        vertex.position = sf::Vector2f(0, it * 64);
        gridLines[vertexCount] = vertex;
        vertexCount++;
        vertex.position = sf::Vector2f(mapWidth * 64, it * 64);
        gridLines[vertexCount] = vertex;
        vertexCount++;
    }
    //vertical lines
    for(unsigned int it = 0; it != (mapWidth + 1); it++)
    {
        vertex.position = sf::Vector2f(it * 64, 0);
        gridLines[vertexCount] = vertex;
        vertexCount++;
        vertex.position = sf::Vector2f(it * 64, mapHeight * 64);
        gridLines[vertexCount] = vertex;
        vertexCount++;
    }

    //iterate through tiles, sort by folders
    for(auto& it : tileStorage)
    {
        Tile& tile = it.second;
        std::string& folder = tile.folder;

        std::pair<std::string, std::vector<Tile*> >& tileDir = getFolder(folder);
        tileDir.second.push_back(&tile);
    }
}
std::pair<std::string, std::vector<Tile*> >& TileEngine_Editor::getFolder(const std::string& dir)
{
    for(auto& it : sortedTiles)
    {
        std::string& tileDir = it.first;
        if(dir == tileDir)
        {
            return it;
        }
    }
    sortedTiles.push_back(std::make_pair(dir, std::vector<Tile*>()));
    return sortedTiles.back();
}

void TileEngine_Editor::drawMap()
{
    TileEngine::drawTiles();
}

void TileEngine_Editor::drawTiles(sf::Font& font)
{
    int previousTileHeightMax = 0;
    int traversedHeight = 0;
    for(unsigned int it_folder = 0; it_folder != sortedTiles.size(); it_folder++)
    {
        traversedHeight += previousTileHeightMax;
        previousTileHeightMax = 0;

        int distance_x = 0;

        std::string& tileDir = sortedTiles[it_folder].first;
        std::vector<Tile*>& tiles = sortedTiles[it_folder].second;

        sf::Text dirText = sf::Text(tileDir, font, 20);
        dirText.setFillColor(sf::Color::Black);
        dirText.setPosition(0, (64 * (it_folder + traversedHeight)) - 30);
        window.draw(dirText);

        for(unsigned int it_tile = 0; it_tile != tiles.size(); it_tile++)
        {
            Tile& tile = *tiles[it_tile];
            previousTileHeightMax = std::max(previousTileHeightMax, tile.tileSize.y);
            tile.setPosition(distance_x, it_folder + traversedHeight);
            distance_x += tile.tileSize.x;
            tile.drawTile();
        }
    }
}

void TileEngine_Editor::drawLayer(int layer, int transparency)
{
    //find boundaries
    int drawMin_x = (cameraPosition.x / 64.f) - (0.5 * tileFit_x) - (maxTileSize_x - 1);
    int drawMin_y = (cameraPosition.y / 64.f) - (0.5 * tileFit_y) - (maxTileSize_y - 1);
    int drawMax_x = (cameraPosition.x / 64.f) + (0.5 * tileFit_x);
    int drawMax_y = (cameraPosition.y / 64.f) + (0.5 * tileFit_y);

    //make sure they are within the map
    if(drawMin_x < 0)
    {
        drawMin_x = 0;
    }
    if(drawMin_y < 0)
    {
        drawMin_y = 0;
    }
    if(drawMax_x > ((int)mapWidth-1))
    {
        drawMax_x = (mapWidth-1);
    }
    if(drawMax_y > ((int)mapHeight-1))
    {
        drawMax_y = (mapHeight-1);
    }

    //changed transparency list
    std::vector<int> modifiedTiles = std::vector<int>();

    //iterate map
    for(int tileIt_x = drawMin_x; tileIt_x <= drawMax_x; tileIt_x++)
    {
        for(int tileIt_y = drawMin_y; tileIt_y <= drawMax_y; tileIt_y++)
        {
            //get tile index and id
            const int& currentTileIndex = tileMap[getTile(tileIt_x, tileIt_y, layer)];

            //make sure its visible
            if(currentTileIndex != 0)
            {
                //get actual tile
                Tile& currentTile = tileStorage.at(currentTileIndex);

                //change transparency if tile not yet changed
                if(std::find(modifiedTiles.begin(), modifiedTiles.end(), currentTileIndex) == modifiedTiles.end())
                {
                    modifiedTiles.push_back(currentTileIndex);
                    currentTile.setTransparency(transparency);
                }

                //move to correct position and draw
                currentTile.setPosition(tileIt_x, tileIt_y);
                currentTile.drawTile();
            }
        }
    }
}

void TileEngine_Editor::overrideMap()
{
    saveFile.saveMap(mapName, tileMap, mapWidth, mapHeight, mapLayers, tileStorage);
}

void TileEngine_Editor::replaceTile(int newTile, int x, int y, int layer)
{
    if((x >= 0 && x < (int)mapWidth) && (y >= 0 && y < (int)mapHeight) && (layer >= 0 && layer < (int)mapLayers))
    {
        tileMap.at(getTile(x, y, layer)) = newTile;
    }
}

void TileEngine_Editor::resetTileAlpha()
{
    for(auto& it : tileStorage)
    {
        Tile& tile = it.second;
        tile.setTransparency(100);
    }
}

void TileEngine_Editor::drawGridLines()
{
    window.draw(gridLines);
}

void TileEngine_Editor::displayTiles()
{
    std::cout << "-------------------" << std::endl;

    for(auto& it : sortedTiles)
    {
        std::string& tileDir = it.first;
        std::vector<Tile*>& tiles = it.second;

        std::cout << "Folder: " << tileDir << std::endl;
        for(auto& tile : tiles)
        {
            std::cout << "  -" << tile->tileName << std::endl;
        }
    }
    std::cout << "-------------------" << std::endl;
}

void TileEngine_Editor::hoverTile(int x, int y)
{
    sf::RectangleShape hover = sf::RectangleShape(sf::Vector2f(64, 64));
    hover.setFillColor(sf::Color(255,255,0, 70));
    hover.setPosition(x*64, y*64);
    window.draw(hover);
}

void TileEngine_Editor::hoverSpan(int x, int y, int size_x, int size_y)
{
    if(size_x < 0)
    {
        size_x *= -1;
        x -= size_x;
        x++;
    }
    if(size_y < 0)
    {
        size_y *= -1;
        y -= size_y;
        y++;
    }

    sf::RectangleShape hover = sf::RectangleShape(sf::Vector2f(64 * size_x, 64 * size_y));
    hover.setFillColor(sf::Color(255,255,0, 70));
    hover.setPosition(x*64, y*64);
    window.draw(hover);
}

Tile* TileEngine_Editor::getTile_tileState(int x, int y)
{
    for(auto& it : tileStorage)
    {
        Tile& tile = it.second;
        if(tile.isInTile(x, y))
        {
            return &tile;
        }
    }
    return nullptr;
}

int TileEngine_Editor::getTileID(const std::string& source)
{
    for(auto& tile : tileStorage)
    {
        if(tile.second.source == source)
        {
            return tile.first;
        }
    }
	//TODO log error
	return -1;
}

unsigned int TileEngine_Editor::getMapWidth()
{
    return mapWidth;
}
unsigned int TileEngine_Editor::getMapHeight()
{
    return mapHeight;
}
unsigned int TileEngine_Editor::getMapLayers()
{
    return mapLayers;
}
void TileEngine_Editor::changeMapName(std::string newName)
{
    mapName = newName;
}
