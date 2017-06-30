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

	//load enough space for the maximum possible maps loaded, as pointers
	//to tileMap objects are used (sortedTiles) and resizing invalidates pointers
	tileEngine.maps.reserve(12);

	if(tileEngine.mapLoaded(mapName))
	{
		return;
	}

	//create the map
	tileEngine.maps.emplace_back(SV_TileEngine(resourceManager).openMap(mapName, window, tileEngine.textureMode, tileEngine.renderMode));
	TileMap* map = &tileEngine.maps.back();

	tileEngine.mapCount++;
	int newMapID = tileEngine.mapCount;
	map->setID(newMapID);

	tileEngine.activeMaps.push_back(newMapID);
	tileEngine.mapID[mapName] = newMapID;

	if(tileEngine.maps.size() != 1)
	{
		return;
	}

	tileEngine.currentMapID = newMapID;

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
	tileEngine.setCamera(utl::Vector2f(x, y));
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
	if(areBordersDrawn)
	{
		tileEngine.draw();
		return;
	}

	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return;
	}
	tileEngine.drawMap(map, utl::Direction::none);
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

		const std::string& tileDir = sortedTiles[it_folder].first;
		std::vector<Tile*>& tiles = sortedTiles[it_folder].second;

		sf::Text dirText = sf::Text(tileDir, font, 20);
		dirText.setFillColor(sf::Color::Black);
		dirText.setPosition(0, (64 * (it_folder + traversedHeight)) - 30);
		window.draw(dirText);

		for(unsigned int it_tile = 0; it_tile < tiles.size(); it_tile++)
		{
			Tile* tile = &*tiles[it_tile];
			previousTileHeightMax = std::max(previousTileHeightMax, tile->getSize_y());
			tile->setPosition(distance_x, it_folder + traversedHeight);
			distance_x += tile->getSize_x();
			tile->drawTile();
		}
	}
}

void TileEngine_Editor::drawLayer(int layer, int transparency)
{
	tileEngine.layerSelection = layer;
	tileEngine.layerTransparency = transparency;
	tileEngine.draw();
	tileEngine.layerSelection = -1;
	tileEngine.layerTransparency = -1;
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

	for(int i = 0; i < 4; i++)
	{
		utl::Direction dir = (i == 0) ? utl::Direction::up :
		                     (i == 1) ? utl::Direction::right :
		                     (i == 2) ? utl::Direction::down :
		                     utl::Direction::left;

		TileMap* borderMap = tileEngine.getMap(map->getBorderMap(dir));
		if(borderMap == nullptr)
		{
			continue;
		}
		for(auto& it : borderMap->getTileKey())
		{
			Tile& tile = it.second;
			tile.setTransparency(100);
		}
		if(dir == utl::Direction::up || dir == utl::Direction::down)
		{
			TileMap* leftMap = tileEngine.getMap(borderMap->getBorderMap(utl::Direction::left));
			TileMap* rightMap = tileEngine.getMap(borderMap->getBorderMap(utl::Direction::right));
			if(leftMap != nullptr)
			{
				for(auto& it : leftMap->getTileKey())
				{
					Tile& tile = it.second;
					tile.setTransparency(100);
				}
			}
			if(rightMap != nullptr)
			{
				for(auto& it : rightMap->getTileKey())
				{
					Tile& tile = it.second;
					tile.setTransparency(100);
				}
			}
		}
	}
}

void TileEngine_Editor::drawGridLines()
{
	if(isGridDrawn)
	{
		window.draw(gridLines);
	}
}

void TileEngine_Editor::printTiles()
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
	TileMap* newMap = tileEngine.getMap(newName);
	if(newMap != nullptr)
	{
		return;
	}

	map->setName(newName);
}

void TileEngine_Editor::changeBorderMap(utl::Direction dir, const std::string& mapName)
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr || mapName == map->getName())
	{
		return;
	}

	utl::Direction flipDir = (dir == utl::Direction::up) ? utl::Direction::down :
	                         (dir == utl::Direction::down) ? utl::Direction::up :
	                         (dir == utl::Direction::left) ? utl::Direction::right :
	                         utl::Direction::left;

	//change old bordering map to empty
	const std::string& oldMapName = map->getBorderMap(dir);
	if(oldMapName.size() > 1)
	{
		TileMap* borderMap = tileEngine.getMap(oldMapName);
		if(borderMap == nullptr)
		{
			tileEngine.loadMap(oldMapName);
			borderMap = tileEngine.getMap(oldMapName);
		}
		if(borderMap->getBorderMap(flipDir) == map->getName())
		{
			borderMap->setBorderMapOffset(0, flipDir);
			borderMap->setBorderMap(std::string(), flipDir);
		}
		saveFile.saveMap(borderMap);
	}
	tileEngine.closeMap(oldMapName);

	map->setBorderMap(mapName, dir);
	map->setBorderMapOffset(0, dir);
	saveFile.saveMap(map);

	//change border of new map
	if(mapName.size() > 1)
	{
		TileMap* borderMap = tileEngine.getMap(mapName);
		if(borderMap == nullptr)
		{
			tileEngine.loadMap(mapName);
			borderMap = tileEngine.getMap(mapName);
		}
		borderMap->setBorderMapOffset(0, flipDir);
		borderMap->setBorderMap(map->getName(), flipDir);
		saveFile.saveMap(borderMap);
	}
}

void TileEngine_Editor::changeBorderOffset(utl::Direction dir, int offset)
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return;
	}

	map->setBorderMapOffset(offset, dir);
	saveFile.saveMap(map);

	//change border of new map
	const std::string& mapName = map->getBorderMap(dir);
	if(mapName.size() > 1)
	{
		TileMap* borderMap = tileEngine.getMap(mapName);
		if(borderMap == nullptr)
		{
			tileEngine.loadMap(mapName);
			borderMap = tileEngine.getMap(mapName);
		}
		borderMap->setBorderMapOffset(-1 * offset,
		                              (dir == utl::Direction::up) ? utl::Direction::down :
		                              (dir == utl::Direction::down) ? utl::Direction::up :
		                              (dir == utl::Direction::left) ? utl::Direction::right :
		                              utl::Direction::left);
		saveFile.saveMap(borderMap);
	}
}

std::string TileEngine_Editor::getBorderMap(utl::Direction dir)
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return std::string();
	}
	return map->getBorderMap(dir);
}

std::string TileEngine_Editor::getMapName()
{
	TileMap* map = tileEngine.getMap(tileEngine.currentMapID);
	if(map == nullptr)
	{
		return std::string();
	}
	return map->getName();
}
