#include "QuestiaEng/TileEngine/TileEngine.h"

TileEngine::TileEngine(sf::RenderWindow& window, ResourceManager& resourceManager):
	window(window)
	, resourceManager(resourceManager)
	, currentMap()
{

}

TileEngine::~TileEngine()
{

}

void TileEngine::loadMap(const std::string& mapName, TileMap::TextureMode textureMode, TileMap::TileMode tileMode)
{
	//loads the current map
	currentMap = std::move(SV_TileEngine(resourceManager).openMap(mapName, window, textureMode, tileMode));
	//TODO fix initialize
	currentMap.initialize();
}

void TileEngine::closeMap()
{
	currentMap = TileMap();
	//kill tile atlas if it exists
	resourceManager.kill("TILESTORAGE");
}

void TileEngine::draw()
{
	switch(currentMap.getTileMode())
	{
	case TileMap::TileMode::Batch:
		drawMap();
		break;
	case TileMap::TileMode::Sprite:
		drawTiles();
		break;
	}
}

void TileEngine::drawMap()
{
	//find boundaries
	int drawMin_x = (cameraPosition.x / 64.f) - (0.5 * tileFit_x) - (currentMap.getMaxTileSize_x() - 1);
	int drawMin_y = (cameraPosition.y / 64.f) - (0.5 * tileFit_y) - (currentMap.getMaxTileSize_y() - 1);
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
	if(drawMax_x > ((int)currentMap.getWidth()-1))
	{
		drawMax_x = (currentMap.getWidth()-1);
	}
	if(drawMax_y > ((int)currentMap.getHeight()-1))
	{
		drawMax_y = (currentMap.getHeight()-1);
	}

	//floor rounds down to find correct chunk
	int minChunk_x = std::floor((float)drawMin_x / 8.f);
	int minChunk_y = std::floor((float)drawMin_y / 8.f);
	int maxChunk_x = std::floor((float)drawMax_x / 8.f);
	int maxChunk_y = std::floor((float)drawMax_y / 8.f);

	//draw chunks
	const unsigned int layers = currentMap.getLayers();
	sf::Texture* atlasPtr = currentMap.getAtlas();
	std::vector<sf::VertexArray> const& chunkVector = currentMap.getChunks();
	for(unsigned int it_layer = 0; it_layer != layers; it_layer++)
	{
		for(int it_y = minChunk_y; it_y != (maxChunk_y + 1); it_y++)
		{
			for(int it_x = minChunk_x; it_x != (maxChunk_x + 1); it_x++)
			{
				window.draw(chunkVector[getChunk(it_x, it_y, it_layer)], atlasPtr);
			}
		}
	}
}
void TileEngine::drawTiles()
{
	//find boundaries
	int drawMin_x = (cameraPosition.x / 64.f) - (0.5 * tileFit_x) - (currentMap.getMaxTileSize_x() - 1);
	int drawMin_y = (cameraPosition.y / 64.f) - (0.5 * tileFit_y) - (currentMap.getMaxTileSize_y() - 1);
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
	if(drawMax_x > ((int)currentMap.getWidth()-1))
	{
		drawMax_x = (currentMap.getWidth()-1);
	}
	if(drawMax_y > ((int)currentMap.getHeight()-1))
	{
		drawMax_y = (currentMap.getHeight()-1);
	}

	//iterate map
	const unsigned int layers = currentMap.getLayers();
	for(unsigned int layerIt = 0; layerIt < layers; layerIt++)
	{
		for(int tileIt_y = drawMin_y; tileIt_y <= drawMax_y; tileIt_y++)
		{
			for(int tileIt_x = drawMin_x; tileIt_x <= drawMax_x; tileIt_x++)
			{
				//get tile index and id
				const int& currentTileIndex = currentMap.getTileMap()[getTile(tileIt_x, tileIt_y, layerIt)];

				//make sure its visible
				if(currentTileIndex != 0)
				{
					//get actual tile
					Tile& currentTile = currentMap.getTileKey().at(currentTileIndex);

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
}

//helpers
int TileEngine::getTile(unsigned int x, unsigned int y, unsigned int layer)
{
	return x + (currentMap.getWidth() * y) + (layer * currentMap.getWidth() * currentMap.getHeight());
}
int TileEngine::getChunk(unsigned int x, unsigned int y, unsigned int layer)
{
	return x + (currentMap.getChunks_x() * y) + (layer * currentMap.getChunks_x() * currentMap.getChunks_y());
}
