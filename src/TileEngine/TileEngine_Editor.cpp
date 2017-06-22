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
	/*
	*tileEngine.currentMap = std::move(SV_TileEngine(resourceManager).openMap(mapName, window, TileMap::TextureMode::All, TileMap::RenderMode::Sprite));

	//2 vertices
	gridLines.resize((tileEngine.currentMap->getWidth() + 1) * 2 + ((tileEngine.currentMap->getHeight() + 1) * 2));
	//vertex count
	unsigned int vertexCount = 0;
	//vertex
	sf::Vertex vertex;
	vertex.color = sf::Color::Black;
	//horizontal lines
	for(unsigned int it = 0; it != (tileEngine.currentMap->getHeight() + 1); it++)
	{
		vertex.position = sf::Vector2f(0, it * 64);
		gridLines[vertexCount] = vertex;
		vertexCount++;
		vertex.position = sf::Vector2f(tileEngine.currentMap->getWidth() * 64, it * 64);
		gridLines[vertexCount] = vertex;
		vertexCount++;
	}
	//vertical lines
	for(unsigned int it = 0; it != (tileEngine.currentMap->getWidth() + 1); it++)
	{
		vertex.position = sf::Vector2f(it * 64, 0);
		gridLines[vertexCount] = vertex;
		vertexCount++;
		vertex.position = sf::Vector2f(it * 64, tileEngine.currentMap->getHeight() * 64);
		gridLines[vertexCount] = vertex;
		vertexCount++;
	}

	//iterate through tiles, sort by folders
	sortedTiles.clear();
	for(auto& it : tileEngine.currentMap->getTileKey())
	{
		Tile& tile = it.second;
		const std::string& folder = tile.getFolder();

		std::pair<std::string, std::vector<Tile*> >& tileDir = getFolder(folder);
		tileDir.second.push_back(&tile);
	}
	 * */
}
void TileEngine_Editor::closeMap()
{
	tileEngine.closeMap();
	sortedTiles.clear();
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
	/*
	//find boundaries
	int drawMin_x = (tileEngine.cameraPosition.x / 64.f) - (0.5 * tileEngine.tileFit_x) - (tileEngine.currentMap->getMaxTileSize_x() - 1);
	int drawMin_y = (tileEngine.cameraPosition.y / 64.f) - (0.5 * tileEngine.tileFit_y) - (tileEngine.currentMap->getMaxTileSize_y() - 1);
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
	if(drawMax_x > ((int)tileEngine.currentMap->getWidth()-1))
	{
		drawMax_x = (tileEngine.currentMap->getWidth()-1);
	}
	if(drawMax_y > ((int)tileEngine.currentMap->getHeight()-1))
	{
		drawMax_y = (tileEngine.currentMap->getHeight()-1);
	}

	//changed transparency list
	std::vector<int> modifiedTiles = std::vector<int>();

	//iterate map
	for(int tileIt_x = drawMin_x; tileIt_x <= drawMax_x; tileIt_x++)
	{
		for(int tileIt_y = drawMin_y; tileIt_y <= drawMax_y; tileIt_y++)
		{
			//get tile index and id
			const int& currentTileIndex = tileEngine.currentMap->getTileMap()[tileEngine.getTile(tileIt_x, tileIt_y, layer, tileEngine.currentMap)];

			//make sure its visible
			if(currentTileIndex != 0)
			{
				//get actual tile
				Tile& currentTile = tileEngine.currentMap->getTileKey().at(currentTileIndex);

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
	 * */
}

void TileEngine_Editor::overrideMap()
{
	/*
	saveFile.saveMap(tileEngine.currentMap->getName(),
	                 tileEngine.currentMap->getTileMap(),
	                 tileEngine.currentMap->getWidth(),
	                 tileEngine.currentMap->getHeight(),
	                 tileEngine.currentMap->getLayers(),
	                 tileEngine.currentMap->getTileKey());
					  * */
}

void TileEngine_Editor::replaceTile(int newTile, int x, int y, int layer)
{
	/*
	if((x >= 0  && x < (int)tileEngine.currentMap->getWidth())
	        && (y >= 0 && y < (int)tileEngine.currentMap->getHeight())
	        && (layer >= 0 && layer < (int)tileEngine.currentMap->getLayers())
			&& newTile != -7)
	{
		tileEngine.currentMap->getTileMap().at(tileEngine.getTile(x, y, layer, tileEngine.currentMap)) = newTile;
	}
	 * */
}

void TileEngine_Editor::resetTileAlpha()
{
	/*
	for(auto& it : tileEngine.currentMap->getTileKey())
	{
		Tile& tile = it.second;
		tile.setTransparency(100);
	}
	 * */
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
	/*
	for(auto& it : tileEngine.currentMap->getTileKey())
	{
		Tile& tile = it.second;
		if(tile.isInTile(x, y))
		{
			return &tile;
		}
	}
	return nullptr;
	 * */
}

int TileEngine_Editor::getTileID(const std::string& source)
{
	/*
	for(auto& tile : tileEngine.currentMap->getTileKey())
	{
		if(tile.second.getDisplay() == source)
		{
			return tile.first;
		}
	}
	//TODO log error
	return -1;
	 * */
}

unsigned int TileEngine_Editor::getMapWidth()
{
	//return tileEngine.currentMap->getWidth();
}
unsigned int TileEngine_Editor::getMapHeight()
{
	//return tileEngine.currentMap->getHeight();
}
unsigned int TileEngine_Editor::getMapLayers()
{
	//return tileEngine.currentMap->getLayers();
}
void TileEngine_Editor::changeMapName(const std::string& newName)
{
	//tileEngine.currentMap->setName(newName);
}