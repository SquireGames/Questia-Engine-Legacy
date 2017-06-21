#include "QuestiaEng/TileEngine/TileEngine.h"

TileEngine::TileEngine(sf::RenderWindow& window, ResourceManager& resourceManager):
	window(window)
	, resourceManager(resourceManager)
	, currentMap(&maps[0])
	, nextMap(&maps[1])
{

}

TileEngine::~TileEngine()
{

}

void TileEngine::loadMap(const std::string& mapName, TileMap::TextureMode textureMode, TileMap::RenderMode tileMode)
{
	this->textureMode = textureMode;
	this->tileMode = tileMode;

	loadMap(currentMap, mapName, textureMode, tileMode);
}

void TileEngine::loadMap(TileMap* map, const std::string& mapName, TileMap::TextureMode textureMode, TileMap::RenderMode tileMode)
{
	*map = std::move(SV_TileEngine(resourceManager).openMap(mapName, window, textureMode, tileMode));
	//TODO fix initialize
	map->initialize();
}


void TileEngine::closeMap()
{
	*currentMap = TileMap();
	//kill tile atlas if it exists
	resourceManager.kill(std::string("TILESTORAGE_") + "Demo_1");
	resourceManager.kill(std::string("TILESTORAGE_") + "Demo_2");
}

void TileEngine::draw()
{
	switch(currentMap->getRenderMode())
	{
	case TileMap::RenderMode::Batch:
		drawMap();
		break;
	case TileMap::RenderMode::Sprite:
		drawTiles();
		break;
	}
}

void TileEngine::drawMap()
{
	{
		//find boundaries
		int drawMin_x = (cameraPosition.x / 64.f) - (0.5 * tileFit_x) - (currentMap->getMaxTileSize_x() - 1);
		int drawMin_y = (cameraPosition.y / 64.f) - (0.5 * tileFit_y) - (currentMap->getMaxTileSize_y() - 1);
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
		if(drawMax_x > ((int)currentMap->getWidth()-1))
		{
			drawMax_x = (currentMap->getWidth()-1);
		}
		if(drawMax_y > ((int)currentMap->getHeight()-1))
		{
			drawMax_y = (currentMap->getHeight()-1);
		}

		if(!(drawMin_x > ((int)currentMap->getWidth()-1) || drawMax_x < 0 || drawMin_y > ((int)currentMap->getHeight()-1) || drawMax_y < 0))
		{
			//floor rounds down to find correct chunk
			int minChunk_x = std::floor((float)drawMin_x / 8.f);
			int minChunk_y = std::floor((float)drawMin_y / 8.f);
			int maxChunk_x = std::floor((float)drawMax_x / 8.f);
			int maxChunk_y = std::floor((float)drawMax_y / 8.f);

			//draw chunks
			const unsigned int layers = currentMap->getLayers();
			sf::Texture* atlasPtr = currentMap->getAtlas();
			std::vector<sf::VertexArray> const& chunkVector = currentMap->getChunks();
			for(unsigned int it_layer = 0; it_layer < layers; it_layer++)
			{
				for(int it_y = minChunk_y; it_y < (maxChunk_y + 1); it_y++)
				{
					for(int it_x = minChunk_x; it_x < (maxChunk_x + 1); it_x++)
					{
						window.draw(chunkVector[getChunk(it_x, it_y, it_layer, currentMap)], atlasPtr);
					}
				}
			}
		}
	}

	//if second map
	if(nextMap->isLoaded())
	{
		utl::Vector2i proxyCameraPosition = cameraPosition;
		proxyCameraPosition.x = proxyCameraPosition.x - (currentMap->getWidth() * 64);

		//find boundaries
		int drawMin_x = (proxyCameraPosition.x / 64.f) - (0.5 * tileFit_x) - (nextMap->getMaxTileSize_x() - 1);
		int drawMin_y = (proxyCameraPosition.y / 64.f) - (0.5 * tileFit_y) - (nextMap->getMaxTileSize_y() - 1);
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
		if(drawMax_x > ((int)nextMap->getWidth()-1))
		{
			drawMax_x = (nextMap->getWidth()-1);
		}
		if(drawMax_y > ((int)nextMap->getHeight()-1))
		{
			drawMax_y = (nextMap->getHeight()-1);
		}

		if(!((drawMin_x > ((int)nextMap->getWidth()-1)) || (drawMax_x < 0) || (drawMin_y > ((int)nextMap->getHeight()-1)) || (drawMax_y < 0)))
		{
			//floor rounds down to find correct chunk
			int minChunk_x = std::floor((float)drawMin_x / 8.f);
			int minChunk_y = std::floor((float)drawMin_y / 8.f);
			int maxChunk_x = std::floor((float)drawMax_x / 8.f);
			int maxChunk_y = std::floor((float)drawMax_y / 8.f);

			//draw chunks
			const unsigned int layers = nextMap->getLayers();
			sf::Texture* atlasPtr = nextMap->getAtlas();
			std::vector<sf::VertexArray> const& chunkVector = nextMap->getChunks();
			
			sf::View view = window.getView();
			sf::View transView = window.getView();
			transView.move(utl::Vector2f(-1 * ((int)currentMap->getWidth() * 64), 0).sf());
			window.setView(transView);
	
			for(unsigned int it_layer = 0; it_layer < layers; it_layer++)
			{
				for(int it_y = minChunk_y; it_y < (maxChunk_y + 1); it_y++)
				{
					for(int it_x = minChunk_x; it_x < (maxChunk_x + 1); it_x++)
					{
						window.draw(chunkVector[getChunk(it_x, it_y, it_layer, nextMap)], atlasPtr);
					}
				}
			}
			
			window.setView(view);
		}
	}
}

void TileEngine::drawTiles()
{
	//find boundaries
	int drawMin_x = (cameraPosition.x / 64.f) - (0.5 * tileFit_x) - (currentMap->getMaxTileSize_x() - 1);
	int drawMin_y = (cameraPosition.y / 64.f) - (0.5 * tileFit_y) - (currentMap->getMaxTileSize_y() - 1);
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
	if(drawMax_x > ((int)currentMap->getWidth()-1))
	{
		drawMax_x = (currentMap->getWidth()-1);
	}
	if(drawMax_y > ((int)currentMap->getHeight()-1))
	{
		drawMax_y = (currentMap->getHeight()-1);
	}

	//iterate map
	const unsigned int layers = currentMap->getLayers();
	for(unsigned int layerIt = 0; layerIt < layers; layerIt++)
	{
		for(int tileIt_y = drawMin_y; tileIt_y <= drawMax_y; tileIt_y++)
		{
			for(int tileIt_x = drawMin_x; tileIt_x <= drawMax_x; tileIt_x++)
			{
				//get tile index and id
				const int& currentTileIndex = currentMap->getTileMap()[getTile(tileIt_x, tileIt_y, layerIt, currentMap)];

				//make sure its visible
				if(currentTileIndex != 0)
				{
					//get actual tile
					Tile& currentTile = currentMap->getTileKey().at(currentTileIndex);

					//move to correct position and draw
					currentTile.setPosition(tileIt_x, tileIt_y);
					currentTile.drawTile();
				}
			}
		}
	}
}
void TileEngine::setViewportSize(float width, float height)
{
	tileFit_x = (width  / 64.f) + 2; // +2 for transitioning tiles
	tileFit_y = (height / 64.f) + 2; // +2 for transitioning tiles
}

void TileEngine::setPosition(int x, int y)
{
	cameraPosition.x = x;
	cameraPosition.y = y;

	//TODO implement next map check
	//if(loadNextMap(x, y))
	if(!nextMap->isLoaded())
	{
		//TODO launch thread
		loadMap(nextMap, "Demo_2", textureMode, tileMode);
	}
	//else if(crossedBorder())
	else if(false)
	{
		//std::swap(currentMap, nextMap);
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
