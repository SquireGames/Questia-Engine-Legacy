#include "QuestiaEng/TileEngine/TileEngine.h"

TileEngine::TileEngine(sf::RenderWindow& window, ResourceManager& resourceManager):
	window(window)
	, resourceManager(resourceManager)
{

}

void TileEngine::setMode(TileMap::TextureMode textureMode, TileMap::RenderMode renderMode)
{
	this->textureMode = textureMode;
	this->renderMode = renderMode;
}

void TileEngine::loadMap(const std::string& mapName)
{
	if(mapLoaded(mapName))
	{
		return;
	}

	//create the map
	maps.emplace_back(SV_TileEngine(resourceManager).openMap(mapName, window, textureMode, renderMode));
	lastMap = &maps.back();

	lastMap->setID(++mapCount);
	activeMaps.push_back(mapCount);
	mapID[mapName] = mapCount;

	if(maps.size() == 1)
	{
		currentMapID = mapCount;
	}
	lastMap->initialize();
}


void TileEngine::closeMaps()
{
	for(auto& map : maps)
	{
		resourceManager.kill(std::string("TILESTORAGE_") + map.getName());
	}
	maps.clear();
	activeMaps.clear();
	mapID.clear();
	currentMapID = -1;
}

void TileEngine::draw()
{
	if(currentMapID == -1)
	{
		return;
	}

	TileMap* currentMap = getMap(currentMapID);
	drawMap(currentMap, utl::Direction::none);

	for(char d = 0; d < (char)utl::Direction::DirectionCount; d++)
	{
		utl::Direction dir = (utl::Direction)d;
		switch(dir)
		{
		case utl::Direction::up:
		case utl::Direction::down:
		case utl::Direction::left:
		case utl::Direction::right:
			{
				const std::string& borderMapName = currentMap->getBorderMap(dir);
				if(borderMapName.size() > 1)
				{
					TileMap* borderMap = getMap(borderMapName);
					if(borderMap == nullptr)
					{
						loadMap(borderMapName);
						borderMap = lastMap;
					}
					drawMap(borderMap, dir, currentMap, currentMap->getBorderMapOffset(dir));
					//if up or down, draw upleft, upright, downleft, downright
					if(dir == utl::Direction::up || dir == utl::Direction::down)
					{
						const std::string& leftMap  = borderMap->getBorderMap(utl::Direction::left);
						const std::string& rightMap = borderMap->getBorderMap(utl::Direction::right);
						if(leftMap.size() > 1)
						{
							TileMap* sideMap = getMap(leftMap);
							if(sideMap == nullptr)
							{
								loadMap(leftMap);
								sideMap = lastMap;
							}
							drawMap(sideMap, dir, currentMap
							        , (-1 * sideMap->getWidth()) + currentMap->getBorderMapOffset(dir)
							        , borderMap->getBorderMapOffset(utl::Direction::left)
							        + ((dir == utl::Direction::down) ? 0 : -1 * (borderMap->getHeight() - sideMap->getHeight())));
						}
						if(rightMap.size() > 1)
						{
							TileMap* sideMap = getMap(rightMap);
							if(sideMap == nullptr)
							{
								loadMap(rightMap);
								sideMap = lastMap;
							}
							drawMap(sideMap, dir, currentMap
							        , borderMap->getWidth() + currentMap->getBorderMapOffset(dir)
							        , borderMap->getBorderMapOffset(utl::Direction::right)
							        + ((dir == utl::Direction::down) ? 0 : -1 * (borderMap->getHeight() - sideMap->getHeight())));
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

void TileEngine::drawMap(TileMap* map, utl::Direction dir, TileMap* refMap, int tilesOffset, int otherAxisOffset)
{
	utl::Vector2f cameraDisplacement = utl::Vector2f(0,0);
	if(refMap != nullptr)
	{
		switch(dir)
		{
		case utl::Direction::none:
			break;
		case utl::Direction::right:
			cameraDisplacement.x -= refMap->getWidth() * 64.f;
			cameraDisplacement.x -= otherAxisOffset * 64.f;
			cameraDisplacement.y -= tilesOffset * 64.f;
			break;
		case utl::Direction::left:
			cameraDisplacement.x += map->getWidth() * 64.f;
			cameraDisplacement.x -= otherAxisOffset * 64.f;
			cameraDisplacement.y -= tilesOffset * 64.f;
			break;
		case utl::Direction::up:
			cameraDisplacement.y += map->getHeight() * 64.f;
			cameraDisplacement.y -= otherAxisOffset * 64.f;
			cameraDisplacement.x -= tilesOffset * 64.f;
			break;
		case utl::Direction::down:
			cameraDisplacement.y -= refMap->getHeight() * 64.f;
			cameraDisplacement.y -= otherAxisOffset * 64.f;
			cameraDisplacement.x -= tilesOffset * 64.f;
			break;
		default:
			//TODO do all cases
			break;
		}
	}
	utl::Vector2f proxyCameraPosition = utl::Vector2f(cameraPosition.x, cameraPosition.y) + cameraDisplacement;

	//find boundaries
	int drawMin_x = (proxyCameraPosition.x / 64.f) - (0.5 * tileFit_x) - (map->getMaxTileSize_x() - 1);
	int drawMin_y = (proxyCameraPosition.y / 64.f) - (0.5 * tileFit_y) - (map->getMaxTileSize_y() - 1);
	int drawMax_x = (proxyCameraPosition.x / 64.f) + (0.5 * tileFit_x);
	int drawMax_y = (proxyCameraPosition.y / 64.f) + (0.5 * tileFit_y);

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

	if(!((drawMin_x > ((int)map->getWidth()-1)) || (drawMax_x < 0) || (drawMin_y > ((int)map->getHeight()-1)) || (drawMax_y < 0)))
	{
		switch(map->getRenderMode())
		{
		case TileMap::RenderMode::Batch:
			{
				//floor rounds down to find correct chunk
				int minChunk_x = std::floor((float)drawMin_x / 8.f);
				int minChunk_y = std::floor((float)drawMin_y / 8.f);
				int maxChunk_x = std::floor((float)drawMax_x / 8.f);
				int maxChunk_y = std::floor((float)drawMax_y / 8.f);

				//draw chunks
				const unsigned int layers = map->getLayers();
				sf::Texture* atlasPtr = map->getAtlas();
				std::vector<sf::VertexArray> const& chunkVector = map->getChunks();

				sf::View view = window.getView();
				sf::View transView = window.getView();
				transView.move(cameraDisplacement.sf());
				window.setView(transView);

				for(unsigned int it_layer = 0; it_layer < layers; it_layer++)
				{
					for(int it_y = minChunk_y; it_y < (maxChunk_y + 1); it_y++)
					{
						for(int it_x = minChunk_x; it_x < (maxChunk_x + 1); it_x++)
						{
							window.draw(chunkVector[getChunk(it_x, it_y, it_layer, map)], atlasPtr);
						}
					}
				}
				window.setView(view);
			}
			break;
		case TileMap::RenderMode::Sprite:
			{
				if(layerSelection >= (int)map->getLayers())
				{
					return;
				}

				const unsigned int firstLayer = (layerSelection == -1) ?  0 : layerSelection;
				const unsigned int endLayer = (layerSelection == -1) ? map->getLayers() : (layerSelection + 1);

				sf::View view = window.getView();
				sf::View transView = window.getView();
				transView.move(cameraDisplacement.sf());
				window.setView(transView);

				//holds all changed transparencies
				std::vector<int> modifiedTiles = std::vector<int>();

				for(unsigned int layerIt = firstLayer; layerIt < endLayer; layerIt++)
				{
					for(int tileIt_y = drawMin_y; tileIt_y <= drawMax_y; tileIt_y++)
					{
						for(int tileIt_x = drawMin_x; tileIt_x <= drawMax_x; tileIt_x++)
						{
							//get tile index and id
							const int& currentTileIndex = map->getTileMap()[getTile(tileIt_x, tileIt_y, layerIt, map)];

							//make sure its visible
							if(currentTileIndex != 0)
							{
								//get actual tile
								Tile& currentTile = map->getTileKey().at(currentTileIndex);

								//change transparency if tile not yet changed
								if(layerTransparency != -1 && std::find(modifiedTiles.begin(), modifiedTiles.end(), currentTileIndex) == modifiedTiles.end())
								{
									modifiedTiles.push_back(currentTileIndex);
									currentTile.setTransparency(layerTransparency);
								}

								//move to correct position and draw
								currentTile.setPosition(tileIt_x, tileIt_y);
								currentTile.drawTile();
							}
						}
					}
				}
				window.setView(view);
			}
			break;
		default:
			break;
		}

	}
}

void TileEngine::setViewportSize(float width, float height)
{
	tileFit_x = (width  / 64.f) + 2; // +2 for transitioning tiles
	tileFit_y = (height / 64.f) + 2; // +2 for transitioning tiles
}

void TileEngine::setPosition(utl::Vector2f& pos)
{
	cameraPosition = pos;
	if(currentMapID == -1)
	{
		return;
	}

	if(pos.y < 0
	        && getMap(currentMapID)->getBorderMap(utl::Direction::up).size() > 1)
	{
		if(!mapLoaded(getMap(currentMapID)->getBorderMap(utl::Direction::up)))
		{
			TileMap* currentMap = getMap(currentMapID);
			//load new map
			loadMap(currentMap->getBorderMap(utl::Direction::up));
			//close old ones
			TileMap* downMap = getMap(currentMap->getBorderMap(utl::Direction::down));
			if(downMap != nullptr)
			{
				closeMap(downMap->getBorderMap(utl::Direction::left));
				closeMap(downMap->getBorderMap(utl::Direction::right));
				closeMap(downMap->getName());
			}
		}
		pos.y += (float)getMap(currentMapID)->getHeight() * 64.f;
		pos.x -= (float)getMap(currentMapID)->getBorderMapOffset(utl::Direction::up) * 64.f;
		currentMapID = getMap(getMap(currentMapID)->getBorderMap(utl::Direction::up))->getID();
		cameraPosition = pos;
	}
	else if(pos.y > getMap(currentMapID)->getHeight() * 64
	        && getMap(currentMapID)->getBorderMap(utl::Direction::down).size() > 1)
	{
		if(!mapLoaded(getMap(currentMapID)->getBorderMap(utl::Direction::down)))
		{
			TileMap* currentMap = getMap(currentMapID);
			//load new map
			loadMap(currentMap->getBorderMap(utl::Direction::down));
			//close old ones
			TileMap* upMap = getMap(currentMap->getBorderMap(utl::Direction::up));
			if(upMap != nullptr)
			{
				closeMap(upMap->getBorderMap(utl::Direction::left));
				closeMap(upMap->getBorderMap(utl::Direction::right));
				closeMap(upMap->getName());
			}
		}
		pos.y -= (float)getMap(currentMapID)->getHeight() * 64.f;
		pos.x -= (float)getMap(currentMapID)->getBorderMapOffset(utl::Direction::down) * 64.f;
		currentMapID = getMap(getMap(currentMapID)->getBorderMap(utl::Direction::down))->getID();
		cameraPosition = pos;
	}
	else if(pos.x < 0
	        && getMap(currentMapID)->getBorderMap(utl::Direction::left).size() > 1)
	{
		if(!mapLoaded(getMap(currentMapID)->getBorderMap(utl::Direction::left)))
		{
			TileMap* currentMap = getMap(currentMapID);
			//load new map
			loadMap(currentMap->getBorderMap(utl::Direction::left));
			//close old ones
			TileMap* rightMap = getMap(currentMap->getBorderMap(utl::Direction::right));
			TileMap* upMap = getMap(currentMap->getBorderMap(utl::Direction::up));
			TileMap* downMap = getMap(currentMap->getBorderMap(utl::Direction::down));
			if(rightMap != nullptr)
			{
				closeMap(rightMap->getName());
			}
			if(upMap != nullptr)
			{
				closeMap(upMap->getBorderMap(utl::Direction::right));
			}
			if(downMap != nullptr)
			{
				closeMap(downMap->getBorderMap(utl::Direction::right));
			}
		}
		pos.x += (float)getMap(currentMapID)->getWidth() * 64.f;
		pos.y -= (float)getMap(currentMapID)->getBorderMapOffset(utl::Direction::left) * 64.f;
		currentMapID = getMap(getMap(currentMapID)->getBorderMap(utl::Direction::left))->getID();
		cameraPosition = pos;
	}
	else if(pos.x > getMap(currentMapID)->getWidth() * 64
	        && getMap(currentMapID)->getBorderMap(utl::Direction::right).size() > 1)
	{
		if(!mapLoaded(getMap(currentMapID)->getBorderMap(utl::Direction::right)))
		{
			TileMap* currentMap = getMap(currentMapID);
			//load new map
			loadMap(currentMap->getBorderMap(utl::Direction::right));
			//close old ones
			TileMap* leftMap = getMap(currentMap->getBorderMap(utl::Direction::left));
			TileMap* upMap = getMap(currentMap->getBorderMap(utl::Direction::up));
			TileMap* downMap = getMap(currentMap->getBorderMap(utl::Direction::down));
			if(leftMap != nullptr)
			{
				closeMap(leftMap->getName());
			}
			if(upMap != nullptr)
			{
				closeMap(upMap->getBorderMap(utl::Direction::left));
			}
			if(downMap != nullptr)
			{
				closeMap(downMap->getBorderMap(utl::Direction::left));
			}
		}
		pos.x -= (float)getMap(currentMapID)->getWidth() * 64.f;
		pos.y -= (float)getMap(currentMapID)->getBorderMapOffset(utl::Direction::right) * 64.f;
		currentMapID = getMap(getMap(currentMapID)->getBorderMap(utl::Direction::right))->getID();
		cameraPosition = pos;
	}
}

//helpers
int TileEngine::getTile(unsigned int x, unsigned int y, unsigned int layer, TileMap* map)
{
	return x + (map->getWidth() * y) + (layer * map->getWidth() * map->getHeight());
}
int TileEngine::getChunk(unsigned int x, unsigned int y, unsigned int layer, TileMap* map)
{
	return x + (map->getChunks_x() * y) + (layer * map->getChunks_x() * map->getChunks_y());
}

void TileEngine::closeMap(const std::string& mapName)
{
	if(mapName.size() > 1)
	{
		closeMap(mapID.at(mapName));
	}
}
void TileEngine::closeMap(int mapID)
{
	int idCount = 0;
	for(auto id : activeMaps)
	{
		if(id == mapID)
		{
			idCount++;
		}
	}
	if(idCount == 0)
	{
		return;
	}
	else if(idCount == 1)
	{
		resourceManager.kill(std::string("TILESTORAGE_") + getMap(mapID)->getName());

		//TODO reuse vector slots rather than erasing
		auto idIt = std::find(activeMaps.begin(), activeMaps.end(), mapID);
		std::iter_swap(idIt, activeMaps.end() - 1);
		activeMaps.erase(activeMaps.end() - 1);

		auto it = std::find_if(maps.begin(), maps.end(), [&](const TileMap& map)
		{
			return map.getID() == mapID;
		});
		std::iter_swap(it, maps.end() - 1);
		maps.erase(maps.end() - 1);


	}
	else
	{
		activeMaps.erase(std::find(activeMaps.begin(), activeMaps.end(), mapID));
	}
}
