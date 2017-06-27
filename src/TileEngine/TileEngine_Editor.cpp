#include "QuestiaEng/TileEngine/TileEngine_Editor.h"

//ctor
TileEngine_Editor::TileEngine_Editor(sf::RenderWindow& window, ResourceManager& resourceManager, TileEngine& tileEng):
	window(window)
	, resourceManager(resourceManager)
	, tileEngine(tileEng)
	, gridLines()
	, saveFile(resourceManager)
{
	gridLines.setPrimitiveType(sf::PrimitiveType::Lines);
}

void TileEngine_Editor::createMap(const std::string& mapName, unsigned int width, unsigned int height, unsigned int layers)
{
	saveFile.createMap(mapName, width, height, layers);
}

void TileEngine_Editor::loadMap(const std::string& mapName)
{
	//sprite rendering to allow easy tile modification
	tileEngine.setMode(TileMap::TextureMode::All, TileMap::RenderMode::Sprite);

	if(tileEngine.mapLoaded(mapName))
	{
		return;
	}

	//create the map
	tileEngine.maps.emplace_back(std::move(SV_TileEngine(resourceManager).openMap(mapName, window, tileEngine.textureMode, tileEngine.renderMode)));
	TileMap* map = &tileEngine.maps.back();

	int mapID = ++tileEngine.mapCount;
	map->setID(mapID);

	tileEngine.activeMaps.push_back(mapID);
	tileEngine.mapID[mapName] = mapID;

	if(tileEngine.maps.size() == 1)
	{
		tileEngine.currentMapID = mapID;
	}

	//2 vertices
	gridLines.resize((map->getWidth() + 1) * 2 + ((map->getHeight() + 1) * 2));
	//vertex count
	unsigned int vertexCount = 0;
	//vertex
	sf::Vertex vertex;
	vertex.color = sf::Color::Black;
	//horizontal lines
	for(unsigned int it = 0; it != (map->getHeight() + 1); it++)
	{
		vertex.position = sf::Vector2f(0, it * 64);
		gridLines[vertexCount] = vertex;
		vertexCount++;
		vertex.position = sf::Vector2f(map->getWidth() * 64, it * 64);
		gridLines[vertexCount] = vertex;
		vertexCount++;
	}
	//vertical lines
	for(unsigned int it = 0; it != (map->getWidth() + 1); it++)
	{
		vertex.position = sf::Vector2f(it * 64, 0);
		gridLines[vertexCount] = vertex;
		vertexCount++;
		vertex.position = sf::Vector2f(it * 64, map->getHeight() * 64);
		gridLines[vertexCount] = vertex;
		vertexCount++;
	}

	//iterate through tiles, sort by folders
	sortedTiles.clear();
	for(auto& it : map->getTileKey())
	{
		Tile& tile = it.second;
		const std::string& folder = tile.getFolder();

		std::pair<std::string, std::vector<Tile*> >& tileDir = getFolder(folder);
		tileDir.second.push_back(&tile);
	}
}
void TileEngine_Editor::closeMap()
{
	tileEngine.closeMaps();
	sortedTiles.clear();
	gridLines.clear();
}

void TileEngine_Editor::setPosition(int x, int y)
{
	utl::Vector2f pos = utl::Vector2f(x, y);
	tileEngine.setPosition(pos);
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

void TileEngine_Editor::draw()
{
	tileEngine.drawTiles();
}

void TileEngine_Editor::drawTiles(sf::Font& font)
{
	int previousTileHeightMax = 0;
	int traversedHeight = 0;
	for(unsigned int it_folder = 0; it_folder < sortedTiles.size(); it_folder++)
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
			previousTileHeightMax = std::max(previousTileHeightMax, tile.getSize_y());
			tile.setPosition(distance_x, it_folder + traversedHeight);
			distance_x += tile.getSize_x();
			tile.drawTile();
		}
	}
}

void TileEngine_Editor::drawLayer(int layer, int transparency)
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return;
	}

	//find boundaries
	int drawMin_x = (tileEngine.cameraPosition.x / 64.f) - (0.5 * tileEngine.tileFit_x) - (map->getMaxTileSize_x() - 1);
	int drawMin_y = (tileEngine.cameraPosition.y / 64.f) - (0.5 * tileEngine.tileFit_y) - (map->getMaxTileSize_y() - 1);
	int drawMax_x = (tileEngine.cameraPosition.x / 64.f) + (0.5 * tileEngine.tileFit_x);
	int drawMax_y = (tileEngine.cameraPosition.y / 64.f) + (0.5 * tileEngine.tileFit_y);

	//make sure they are within the map
	if(drawMin_x < 0)
	{
		drawMin_x = 0;
	}
	if(drawMin_y < 0)
	{
		drawMin_y = 0;
	}
	if(drawMax_x > ((int)map->getWidth()-1))
	{
		drawMax_x = (map->getWidth()-1);
	}
	if(drawMax_y > ((int)map->getHeight()-1))
	{
		drawMax_y = (map->getHeight()-1);
	}

	//changed transparency list
	std::vector<int> modifiedTiles = std::vector<int>();

	//iterate map
	for(int tileIt_x = drawMin_x; tileIt_x <= drawMax_x; tileIt_x++)
	{
		for(int tileIt_y = drawMin_y; tileIt_y <= drawMax_y; tileIt_y++)
		{
			//get tile index and id
			const int& currentTileIndex = map->getTileMap()[tileEngine.getTile(tileIt_x, tileIt_y, layer, map)];

			//make sure its visible
			if(currentTileIndex != 0)
			{
				//get actual tile
				Tile& currentTile = map->getTileKey().at(currentTileIndex);

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
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return;
	}
	saveFile.saveMap(map);
}

void TileEngine_Editor::replaceTile(int newTile, int x, int y, int layer)
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return;
	}

	if((x >= 0  && x < (int)map->getWidth())
	        && (y >= 0 && y < (int)map->getHeight())
	        && (layer >= 0 && layer < (int)map->getLayers())
	        && newTile != -7)
	{
		map->getTileMap().at(tileEngine.getTile(x, y, layer, map)) = newTile;
	}
}

void TileEngine_Editor::resetTileAlpha()
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return;
	}
	for(auto& it : map->getTileKey())
	{
		Tile& tile = it.second;
		tile.setTransparency(100);
	}
}

void TileEngine_Editor::drawGridLines()
{
	if(isGridDrawn)
	{
		window.draw(gridLines);
	}
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
			std::cout << "  -" << tile->getTileName() << std::endl;
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
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return nullptr;
	}
	for(auto& it : map->getTileKey())
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
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		//TODO log error
		return -1;
	}
	for(auto& tile : map->getTileKey())
	{
		if(tile.second.getDisplay() == source)
		{
			return tile.first;
		}
	}
	//TODO log error
	return -1;
}

unsigned int TileEngine_Editor::getMapWidth()
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return 0;
	}
	return map->getWidth();
}
unsigned int TileEngine_Editor::getMapHeight()
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return 0;
	}
	return map->getHeight();
}
unsigned int TileEngine_Editor::getMapLayers()
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return 0;
	}
	return map->getLayers();
}
void TileEngine_Editor::changeMapName(const std::string& newName)
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return;
	}
	map->setName(newName);
}
