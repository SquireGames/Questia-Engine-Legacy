#include "QuestiaEng/TileEngine/TileEngine.h"

TileEngine::TileEngine(sf::RenderWindow& _window, ResourceManager& _resourceManager):
	saveFile(_resourceManager)
	, window(_window)
	, resourceManager(_resourceManager)
{

}

TileEngine::~TileEngine()
{

}

void TileEngine::loadMap(std::string _mapName, TileMap::TextureMode textureMode, TileMap::TileMode tileMode)
{
	//get the texture atlas
	resourceManager.getTexture("TILESTORAGE");
	//get info
	TileMap mapData = std::move(saveFile.openMap(_mapName, window, textureMode, tileMode));

	//get texture
	textureAtlas = &resourceManager.getBlankTexture("TILESTORAGE");

	//set info
	mapWidth = mapData.getWidth();
	mapHeight = mapData.getHeight();
	mapLayers = mapData.getLayers();
	maxTileSize_x = mapData.getMaxTileSize_x();
	maxTileSize_y = mapData.getMaxTileSize_y();

	//get chunk size
	int remainder_x = mapWidth % 8;
	int remainder_y = mapHeight % 8;
	//exact chunk tiles
	if(remainder_x == 0)
	{
		chunks_x = mapWidth / 8;
	}
	//incomplete chunk = new chunk
	else
	{
		chunks_x = ((mapWidth - remainder_x) / 8) + 1;
	}
	if(remainder_y == 0)
	{
		chunks_y = mapHeight / 8;
	}
	//incomplete chunk = new chunk
	else
	{
		chunks_y = ((mapHeight - remainder_y) / 8) + 1;
	}

	//set size of chunk vector
	chunkVector.resize(chunks_x * chunks_y * mapLayers);

	sf::VertexArray emptyChunk;
	//4 vertices per tile, 8 x 8 tiles
	emptyChunk.setPrimitiveType(sf::PrimitiveType::Quads);

	//fill chunk vector with chunks
	for(unsigned int it = 0; it != chunkVector.size(); it++)
	{
		chunkVector[it] = emptyChunk;
	}

	//overriding the map fixes the need to clear it
	tileMap = mapData.getTileMap();
	tileStorage = mapData.getTileKey();

	//filling the chunks with tiles
	//iterate through all chunks
	for(unsigned int it_chunk_x = 0; it_chunk_x != mapWidth; it_chunk_x++)
	{
		for(unsigned int it_chunk_y = 0; it_chunk_y != mapHeight; it_chunk_y++)
		{
			for(unsigned int it_layer = 0; it_layer != mapLayers; it_layer++)
			{
				//chunk tile number
				int chunkOrigin_x = 8 * it_chunk_x;
				int chunkOrigin_y = 8 * it_chunk_y;

				//iterate through tiles, 8x8
				for(unsigned int it_tile_x = 0; it_tile_x != 8 && it_tile_x + chunkOrigin_x < mapWidth; it_tile_x++)
				{
					for(unsigned int it_tile_y = 0; it_tile_y != 8 && it_tile_y + chunkOrigin_y < mapHeight; it_tile_y++)
					{
						//get tile number
						int currentTile_x = chunkOrigin_x + it_tile_x;
						int currentTile_y = chunkOrigin_y + it_tile_y;

						//get tile index and id
						const int& tileID = tileMap[getTile(currentTile_x, currentTile_y, it_layer)];

						if(tileID != 0)
						{
							const Tile& tileData = tileStorage.count(tileID) ? tileStorage.at(tileID) : tileStorage.at(-1);

							//to prevent texture bleeding
							float offset = 0.001;

							//texture int rect
							const utl::IntRect& texturePosition = tileData.texturePosition;
							//size
							const utl::Vector2i& tileSize = tileData.tileSize;
							//rotation
							const int& rotationDegrees = tileData.degrees;
							//flips
							const char& flip = tileData.flip;

							//size vertices and texture coords for flips and rotations
							utl::Vector2f posVerticies[4] = {utl::Vector2f(0                     + (currentTile_x * 64) - offset, 0                      + (currentTile_y * 64) - offset),
							                                 utl::Vector2f(texturePosition.width + (currentTile_x * 64) + offset, 0                      + (currentTile_y * 64) - offset),
							                                 utl::Vector2f(texturePosition.width + (currentTile_x * 64) + offset, texturePosition.height + (currentTile_y * 64) + offset),
							                                 utl::Vector2f(0                     + (currentTile_x * 64) - offset, texturePosition.height + (currentTile_y * 64) + offset)
							                                };
							utl::Vector2f textureVerticies[4] = {utl::Vector2f(texturePosition.x                         + offset, texturePosition.y                          + offset),
							                                     utl::Vector2f(texturePosition.x + texturePosition.width - offset, texturePosition.y                          + offset),
							                                     utl::Vector2f(texturePosition.x + texturePosition.width - offset, texturePosition.y + texturePosition.height - offset),
							                                     utl::Vector2f(texturePosition.x                         + offset, texturePosition.y + texturePosition.height - offset)
							                                    };
							//apply transformations
							//size
							if(tileSize.x != -1)
							{
								posVerticies[0] = utl::Vector2f(0                + (currentTile_x * 64) - offset, 0               + (currentTile_y * 64) - offset);
								posVerticies[1] = utl::Vector2f(64 * tileSize.x  + (currentTile_x * 64) + offset, 0               + (currentTile_y * 64) - offset);
								posVerticies[2] = utl::Vector2f(64 * tileSize.x  + (currentTile_x * 64) + offset, 64 * tileSize.y + (currentTile_y * 64) + offset);
								posVerticies[3] = utl::Vector2f(0                + (currentTile_x * 64) - offset, 64 * tileSize.y + (currentTile_y * 64) + offset);
							}
							//flip
							if(flip != 'n')
							{
								utl::Vector2f tempVec(0,0);
								if(flip == 'x')
								{
									tempVec = textureVerticies[0];
									textureVerticies[0] = textureVerticies[1];
									textureVerticies[1] = tempVec;
									tempVec = textureVerticies[2];
									textureVerticies[2] = textureVerticies[3];
									textureVerticies[3] = tempVec;
								}
								else if(flip == 'y')
								{
									tempVec = textureVerticies[0];
									textureVerticies[0] = textureVerticies[3];
									textureVerticies[3] = tempVec;
									tempVec = textureVerticies[1];
									textureVerticies[1] = textureVerticies[2];
									textureVerticies[2] = tempVec;
								}
								else if(flip == 'b')
								{
									tempVec = textureVerticies[0];
									textureVerticies[0] = textureVerticies[2];
									textureVerticies[2] = tempVec;
									tempVec = textureVerticies[1];
									textureVerticies[1] = textureVerticies[3];
									textureVerticies[3] = tempVec;
								}
							}
							//rotate
							if(rotationDegrees != 0)
							{
								utl::Vector2f tempVec(0,0);
								if(rotationDegrees == 90)
								{
									tempVec = textureVerticies[0];
									textureVerticies[0] = textureVerticies[3];
									textureVerticies[3] = textureVerticies[2];
									textureVerticies[2] = textureVerticies[1];
									textureVerticies[1] = tempVec;
								}
								else if(rotationDegrees == 180)
								{
									tempVec = textureVerticies[0];
									textureVerticies[0] = textureVerticies[2];
									textureVerticies[2] = tempVec;
									tempVec = textureVerticies[1];
									textureVerticies[1] = textureVerticies[3];
									textureVerticies[3] = tempVec;
								}
								else if(rotationDegrees == 270)
								{
									tempVec = textureVerticies[0];
									textureVerticies[0] = textureVerticies[1];
									textureVerticies[1] = textureVerticies[2];
									textureVerticies[2] = textureVerticies[3];
									textureVerticies[3] = tempVec;
								}
							}

							//set the coords
							//top left
							chunkVector[getChunk(it_chunk_x, it_chunk_y, it_layer)].append(sf::Vertex(sf::Vector2f(posVerticies[0].x, posVerticies[0].y), sf::Vector2f(textureVerticies[0].x, textureVerticies[0].y)));
							//top right
							chunkVector[getChunk(it_chunk_x, it_chunk_y, it_layer)].append(sf::Vertex(sf::Vector2f(posVerticies[1].x, posVerticies[1].y), sf::Vector2f(textureVerticies[1].x, textureVerticies[1].y)));
							//bottom right
							chunkVector[getChunk(it_chunk_x, it_chunk_y, it_layer)].append(sf::Vertex(sf::Vector2f(posVerticies[2].x, posVerticies[2].y), sf::Vector2f(textureVerticies[2].x, textureVerticies[2].y)));
							//bottom left
							chunkVector[getChunk(it_chunk_x, it_chunk_y, it_layer)].append(sf::Vertex(sf::Vector2f(posVerticies[3].x, posVerticies[3].y), sf::Vector2f(textureVerticies[3].x, textureVerticies[3].y)));
						}
					}
				}
			}
		}
	}


	//for sprite renderer, make sure all tiles exist
	for(unsigned int it = 0; it != tileMap.size(); it++)
	{
		int tileID = tileMap[it];
		//if does not exist, replace with -1 and give error message
		if(tileID != 0 && !tileStorage.count(tileID))
		{
			tileMap[it] = -1;
			std::cout << "TILEENGINE - TILE NUMBER: " << tileID << " DOES NOT EXIST " << std::endl;
		}
	}
	//get max tile size
	for(unsigned int it = 0; it != tileMap.size(); it++)
	{
		int tileID = tileMap[it];
		//if does not exist, replace with -1 and give error message
		if(tileID != -1 && tileID != 0)
		{
			if(tileStorage.at(tileID).tileSize.x > (int)maxTileSize_x)
			{
				maxTileSize_x = tileStorage.at(tileID).tileSize.x;
			}
			if(tileStorage.at(tileID).tileSize.y > (int)maxTileSize_y)
			{
				maxTileSize_y = tileStorage.at(tileID).tileSize.y;
			}
		}
	}

	//saving map names
	mapName = _mapName;
}

void TileEngine::drawMap()
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

	//floor rounds down to find correct chunk
	int minChunk_x = std::floor((float)drawMin_x / 8.f);
	int minChunk_y = std::floor((float)drawMin_y / 8.f);
	int maxChunk_x = std::floor((float)drawMax_x / 8.f);
	int maxChunk_y = std::floor((float)drawMax_y / 8.f);

	//draw chunks
	for(unsigned int it_layer = 0; it_layer != mapLayers; it_layer++)
	{
		for(int it_x = minChunk_x; it_x != (maxChunk_x + 1); it_x++)
		{
			for(int it_y = minChunk_y; it_y != (maxChunk_y + 1); it_y++)
			{
				window.draw(chunkVector[getChunk(it_x, it_y, it_layer)], textureAtlas);
			}
		}
	}
}
void TileEngine::drawTiles()
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

	//iterate map
	for(unsigned int layerIt = 0; layerIt < mapLayers; layerIt++)
	{
		for(int tileIt_x = drawMin_x; tileIt_x <= drawMax_x; tileIt_x++)
		{
			for(int tileIt_y = drawMin_y; tileIt_y <= drawMax_y; tileIt_y++)
			{
				//get tile index and id
				const int& currentTileIndex = tileMap[getTile(tileIt_x, tileIt_y, layerIt)];

				//make sure its visible
				if(currentTileIndex != 0)
				{
					//get actual tile
					Tile& currentTile = tileStorage.at(currentTileIndex);

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

//helper
int TileEngine::getTile(unsigned int x, unsigned int y, unsigned int layer)
{
	return x + (mapWidth * y) + (layer * mapWidth * mapHeight);
}
int TileEngine::getChunk(unsigned int x, unsigned int y, unsigned int layer)
{
	return x + (chunks_x * y) + (layer * chunks_x * chunks_y);
}
