#include "QuestiaEng/TileEngine/SaveFile_TileEngine.h"

SaveFile_TileEngine::SaveFile_TileEngine(ResourceManager& _resourceManager):
	resourceManager(_resourceManager)
	, textureAtlas(_resourceManager)
{

}
SaveFile_TileEngine::~SaveFile_TileEngine()
{
	//dtor
}

TileMap SaveFile_TileEngine::openMap(std::string mapName, sf::RenderWindow& window, TileMap::TextureMode textureMode, TileMap::TileMode tileMode)
{
	//create map
	TileMap mapData;

	//make sure directory exists
	if(!utl::doesExist(utl::conjoinString( {"Maps/", mapName})))
	{
		throw std::runtime_error(utl::conjoinString( {"ERROR: Reading map- Maps/", mapName, " <Utl/SaveFile/SaveFile_TileEngine.cpp>"}));
		return mapData;
	}

	mapData.setName(mapName);
	mapData.setTextureMode(textureMode);
	mapData.setTileMode(tileMode);

	//reads mapInfo file
	SaveFile saveFile_mapInfo;
	saveFile_mapInfo.setFilePath("Maps/" + mapName + "/mapInfo.txt");
	saveFile_mapInfo.readFile();
	mapData.setWidth(utl::asInt(saveFile_mapInfo.getItem(name_width)));
	mapData.setHeight(utl::asInt(saveFile_mapInfo.getItem(name_height)));
	mapData.setLayers(utl::asInt(saveFile_mapInfo.getItem(name_layers)));
	unsigned int totalTiles = mapData.getWidth() * mapData.getHeight() * mapData.getLayers();

	//get all tile textures locations
	std::vector <std::pair <int, std::string> > tileLocations = getTileLocations("Maps/" + mapName, textureMode);

	///load tile textures and info into resourceManager, tileData into tileStorage
	loadTiles(tileLocations, mapData, window);

	///tileMap
	mapData.getTileMap().resize(totalTiles);
	//fill as empty
	std::fill(mapData.getTileMap().begin(), mapData.getTileMap().end(), 0);
	//open map file
	SaveFile saveFile_map;
	saveFile_map.setFilePath("Maps/" + mapName + "/map.txt");
	saveFile_map.readFile();
	//get tiles saved in map
	std::vector <std::string> tileVector = saveFile_map.getItemList();
	unsigned int tileNumberIterator = 0; // number of tiles in the map
	for(const std::string& tileLine : tileVector)
	{
		std::vector <std::string> tiles = utl::separateString(tileLine, '|');
		for(std::string& tile : tiles)
		{
			//make sure it is not empty
			if(utl::isNumber(tile) && utl::asInt(tile) != 0)
			{
				mapData.getTileMap()[tileNumberIterator] = utl::asInt(tile);
			}
			if(tile.length() > 0 && tile.at(0) != '\n')
			{
				tileNumberIterator++;
			}
		}
	}

	//loads chunks if in batch mode, validates all sprites in sprite mode
	loadRenderData(mapData);
	mapData.setLoaded();
	return mapData;
}

void SaveFile_TileEngine::loadRenderData(TileMap& mapData)
{
	//validate all sprites loaded into the map
	if(mapData.getTileMode() == TileMap::TileMode::Sprite)
	{
		//for sprite renderer, make sure all tiles exist
		for(unsigned int it = 0; it != mapData.getTileMap().size(); it++)
		{
			int tileID = mapData.getTileMap()[it];
			//if does not exist, replace with -1 and give error message
			if(tileID != 0 && !mapData.getTileKey().count(tileID))
			{
				mapData.getTileMap()[it] = -1;
				std::cout << "SVF TILEENGINE - TILE NUMBER: " << tileID << " DOES NOT EXIST " << std::endl;
			}
		}
	}
	//load all chunks into TileMap
	else if(mapData.getTileMode() == TileMap::TileMode::Batch)
	{
		//get max tile size
		for(unsigned int it = 0; it != mapData.getTileMap().size(); it++)
		{
			int tileID = mapData.getTileMap()[it];
			//if does not exist, replace with -1 and give error message
			if(tileID != -1 && tileID != 0)
			{
				if(mapData.getTileKey().at(tileID).tileSize.x > (int)mapData.getMaxTileSize_x())
				{
					mapData.setMaxTileSize_x(mapData.getTileKey().at(tileID).tileSize.x);
				}
				if(mapData.getTileKey().at(tileID).tileSize.y > (int)mapData.getMaxTileSize_y())
				{
					mapData.setMaxTileSize_y(mapData.getTileKey().at(tileID).tileSize.y);
				}
			}
		}


		//get chunk size
		int remainder_x = mapData.getWidth() % 8;
		int remainder_y = mapData.getHeight() % 8;
		//exact chunk tiles
		if(remainder_x == 0)
		{
			mapData.setChunks_x(mapData.getWidth() / 8);
		}
		//incomplete chunk = new chunk
		else
		{
			mapData.setChunks_x(((mapData.getWidth() - remainder_x) / 8) + 1);
		}
		if(remainder_y == 0)
		{
			mapData.setChunks_y(mapData.getHeight() / 8);
		}
		//incomplete chunk = new chunk
		else
		{
			mapData.setChunks_y(((mapData.getHeight() - remainder_y) / 8) + 1);
		}

		//set size of chunk vector
		std::vector<sf::VertexArray>& chunkVector = mapData.getChunks();
		chunkVector.resize(mapData.getChunks_x() * mapData.getChunks_y() * mapData.getLayers());

		sf::VertexArray emptyChunk;
		//4 vertices per tile, 8 x 8 tiles
		emptyChunk.setPrimitiveType(sf::PrimitiveType::Quads);

		//fill chunk vector with chunks
		for(unsigned int it = 0; it != chunkVector.size(); it++)
		{
			chunkVector[it] = emptyChunk;
		}

		//overriding the map fixes the need to clear it
		mapData.getTileKey() = mapData.getTileKey();

		//filling the chunks with tiles
		//iterate through all chunks
		for(unsigned int it_chunk_x = 0; it_chunk_x != mapData.getWidth(); it_chunk_x++)
		{
			for(unsigned int it_chunk_y = 0; it_chunk_y != mapData.getHeight(); it_chunk_y++)
			{
				for(unsigned int it_layer = 0; it_layer != mapData.getLayers(); it_layer++)
				{
					//chunk tile number
					int chunkOrigin_x = 8 * it_chunk_x;
					int chunkOrigin_y = 8 * it_chunk_y;

					//iterate through tiles, 8x8
					for(unsigned int it_tile_x = 0; it_tile_x != 8 && it_tile_x + chunkOrigin_x < mapData.getWidth(); it_tile_x++)
					{
						for(unsigned int it_tile_y = 0; it_tile_y != 8 && it_tile_y + chunkOrigin_y < mapData.getHeight(); it_tile_y++)
						{
							//get tile number
							int currentTile_x = chunkOrigin_x + it_tile_x;
							int currentTile_y = chunkOrigin_y + it_tile_y;

							//get tile index and id
							const int& tileID = mapData.getTileMap()[getTile(currentTile_x, currentTile_y, it_layer, mapData)];

							if(tileID != 0)
							{
								const Tile& tileData = mapData.getTileKey().count(tileID) ? mapData.getTileKey().at(tileID) : mapData.getTileKey().at(-1);

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
								chunkVector[getChunk(it_chunk_x, it_chunk_y, it_layer, mapData)].append(sf::Vertex(sf::Vector2f(posVerticies[0].x, posVerticies[0].y), sf::Vector2f(textureVerticies[0].x, textureVerticies[0].y)));
								//top right
								chunkVector[getChunk(it_chunk_x, it_chunk_y, it_layer, mapData)].append(sf::Vertex(sf::Vector2f(posVerticies[1].x, posVerticies[1].y), sf::Vector2f(textureVerticies[1].x, textureVerticies[1].y)));
								//bottom right
								chunkVector[getChunk(it_chunk_x, it_chunk_y, it_layer, mapData)].append(sf::Vertex(sf::Vector2f(posVerticies[2].x, posVerticies[2].y), sf::Vector2f(textureVerticies[2].x, textureVerticies[2].y)));
								//bottom left
								chunkVector[getChunk(it_chunk_x, it_chunk_y, it_layer, mapData)].append(sf::Vertex(sf::Vector2f(posVerticies[3].x, posVerticies[3].y), sf::Vector2f(textureVerticies[3].x, textureVerticies[3].y)));
							}
						}
					}
				}
			}
		}
	}
}

std::vector <std::pair <int, std::string> > SaveFile_TileEngine::getTileLocations(std::string filePath, TileMap::TextureMode textureMode)
{
	//to get block datas
	filePath = filePath + file_tilesUsed;

	//return vector
	std::vector<std::pair <int, std::string> > returnTiles = std::vector<std::pair <int, std::string> >();

	SaveFile saveFile_tileLocations;
	saveFile_tileLocations.setFilePath(filePath);
	saveFile_tileLocations.readFile();
	std::vector<std::pair<std::string, std::string>> tileLocations = saveFile_tileLocations.getSaveList();
	for(std::pair<std::string, std::string> tileLocation : tileLocations)
	{
		returnTiles.push_back(std::make_pair(utl::asInt(tileLocation.first), tileLocation.second));
	}

	//adds missing texture if it does not exist
	{
		bool isMissingTexture = false;
		for(std::pair<std::string, std::string> tileLocation : tileLocations)
		{
			if(tileLocation.second == "Media/Image/Game/Tiles/Debug/Missing.png")
			{
				isMissingTexture = true;
			}
		}
		if(!isMissingTexture)
		{
			returnTiles.push_back(std::make_pair(-1,"Media/Image/Game/Tiles/Debug/Missing.png"));
		}
	}

	switch(textureMode)
	{
	case TileMap::TextureMode::Map:
		//tiles are already loaded in
		break;
	case TileMap::TextureMode::All:
		{
			//for tile indexing, turns ID to above the maximum of existing id's
			std::vector<std::string> tileNumbers_str = saveFile_tileLocations.getItemList();
			std::vector<int> tileNumbers = std::vector<int>();
			for(const std::string& tileStr : tileNumbers_str)
			{
				tileNumbers.push_back(utl::asInt(tileStr));
			}
			int currentID = 1;
			if(tileNumbers.size() > 0)
			{
				currentID = std::max(1, *std::max_element(tileNumbers.begin(), tileNumbers.end()) + 1);
			}

			//get all files
			std::vector<std::string> tilesDirs = utl::getAllFiles("Media/Image/Game/Tiles", true);

			for(std::string& tileDir : tilesDirs)
			{
				//filter out all .png and .txt
				std::vector<std::string> tilesStrs = utl::filterFiles(utl::getFiles(tileDir, true), ".png");
				std::vector<std::string> tilesStrs_txt = utl::filterFiles(utl::getFiles(tileDir, true), ".txt");
				tilesStrs.insert(tilesStrs.end(), tilesStrs_txt.begin(), tilesStrs_txt.end());

				//iterate through tile file locations
				for(std::string& tileStr : tilesStrs)
				{
					//continues if file already exists
					bool doesFileExist = false;
					for(std::pair<int, std::string> tileLocation : returnTiles)
					{
						if(tileLocation.second == tileStr)
						{
							doesFileExist = true;
						}
					}
					if(!doesFileExist)
					{
						returnTiles.push_back(std::make_pair(currentID++, tileStr));
					}
				}
			}
		}
		break;
	}

	//get rid of overwritten tiles
	returnTiles.erase(std::remove_if(returnTiles.begin(), returnTiles.end(), [&](std::pair <int, std::string> tilePair)
	{
		if(tilePair.second.substr(tilePair.second.length() - 4) == ".png")
		{
			std::string childTile = tilePair.second;
			childTile.resize(childTile.length() - 4);
			childTile = utl::conjoinString( {childTile,".txt"});
			for(auto& it : returnTiles)
			{
				if(it.second == childTile)
				{
					return true;
				}
			}
		}
		return false;
	}), returnTiles.end());

	for(auto& returnTile : returnTiles)
	{
		std::cout << "TILE: (" << returnTile.first << ") - " << returnTile.second << std::endl;
	}

	return returnTiles;
}

void SaveFile_TileEngine::loadTiles(std::vector <std::pair <int, std::string> >& tileLocations, TileMap& mapData, sf::RenderWindow& window)
{
	//used to hold transform data for each tile
	std::map<int, TileTransform> tileTransform;
	//holds IntRect for every texture in the texture atlas, only used if TileMode is Batch
	TextureAtlasData compiledTexture = TextureAtlasData(nullptr);

	//for getting txt file references to pngs
	std::map<int, int> pngIdDirectory;
	for(auto& tileData : tileLocations)
	{
		pngIdDirectory.emplace(std::make_pair(tileData.first, tileData.first));
	}

	//loads textures into textureAtlas
	if(mapData.getTileMode() == TileMap::TileMode::Batch)
	{
		for(auto& tileData : tileLocations)
		{
			const std::string& filePath = tileData.second;

			if(filePath.substr(filePath.length() - 4) == ".png")
			{
				if(!textureAtlas.addTexture(utl::asString(tileData.first), tileData.second))
				{
					std::cout << "TILEENGINE - Tile: '" << filePath << "' failed to load <Utl/SaveFile/SaveFile_TileEngine.cpp>" << std::endl;
					textureAtlas.addTexture(utl::asString(tileData.first), "Media/Image/Game/Tiles/Debug/Missing.png");
				}
			}
			else if(filePath.substr(filePath.length() - 4) == ".txt")
			{
				std::string texturePath = filePath;
				texturePath.resize(texturePath.length() - 4);
				texturePath = texturePath + ".png";

				//will not do anything if not a valid filepath (not a reference)
				if(textureAtlas.addTexture(utl::asString(tileData.first), texturePath))
				{
					std::cout << "TILEENGINE - Reference texture: " << texturePath << "(" << utl::asString(tileData.first) << ")" << std::endl;
				}
			}
			else
			{
				std::cout << "TILEENGINE - Tile extension of: '" << filePath << "' is not accepted <Utl/SaveFile/SaveFile_TileEngine.cpp>" << std::endl;
				textureAtlas.addTexture(utl::asString(tileData.first), "Media/Image/Game/Tiles/Debug/Missing.png");
			}
		}
	}

	//sets tileTransform data for .png files
	for(auto& tileData : tileLocations)
	{
		const int& tileID = tileData.first;
		const std::string& filePath = tileData.second;

		if(filePath.substr(filePath.length() - 4) == ".png")
		{
			TileTransform tile = TileTransform(filePath);
			tile.tileSize = utl::Vector2i(1,1);
			tile.degrees  = 0;
			tile.flip     = 'n';
			tileTransform.emplace(std::make_pair(tileID, tile));
		}
	}

	//sets tileTransform data for .txt files
	for(auto& tileData : tileLocations)
	{
		const int& tileID = tileData.first;
		const std::string& filePath = tileData.second;

		if(filePath.substr(filePath.length() - 4) == ".txt")
		{
			//tile data default
			std::string texturePath = filePath;

			int size_x = 1;
			int size_y = 1;
			int degrees = 0;
			char flip = 'n';

			//load data from text file
			SaveFile saveFile_tileData(filePath);
			saveFile_tileData.readFile();
			for(std::pair<std::string, std::string> fileTransform : saveFile_tileData.getSaveList())
			{
				if(fileTransform.first == "texture")
				{
					auto tileIt = std::find_if(tileLocations.begin(), tileLocations.end(), [fileTransform](std::pair<int, std::string> it)
					{
						return (it.second == fileTransform.second);
					});

					if(tileIt != tileLocations.end())
					{
						const std::pair<int, std::string>& textureID = *tileIt;
						pngIdDirectory[tileID] = textureID.first;
					}
					else
					{
						//TODO make a function for this

						//make the referenced text file texture ID:
						int newTextureID = 1;
						std::vector<int> tileNumbers;
						for(const std::pair<int, std::string>& tile : tileLocations)
						{
							tileNumbers.push_back(tile.first);
						}
						if(tileNumbers.size() > 0)
						{
							newTextureID = std::max(1, *std::max_element(tileNumbers.begin(), tileNumbers.end()) + 1);
						}

						//if texture is made
						if(textureAtlas.addTexture(utl::asString(newTextureID), fileTransform.second))
						{
							std::cout << "TILEENGINE - Reference texture: " << fileTransform.second << "(" << newTextureID << ")" << std::endl;
							pngIdDirectory[tileID] = newTextureID;

							TileTransform tile = TileTransform(fileTransform.second);
							tile.tileSize = utl::Vector2i(1,1);
							tile.degrees  = 0;
							tile.flip     = 'n';
							tileTransform.emplace(std::make_pair(newTextureID, tile));
						}
						else
						{
							std::cout << "TILEENGINE - Tile: '" << fileTransform.second << "' failed to load <Utl/SaveFile/SaveFile_TileEngine.cpp>" << std::endl;

							pngIdDirectory[tileID] = -1;
						}
					}
				}
				else if(fileTransform.first == "size_x")
				{
					if(utl::isNumber(fileTransform.second))
					{
						size_x = utl::asInt(fileTransform.second);
					}
				}
				else if(fileTransform.first == "size_y")
				{
					if(utl::isNumber(fileTransform.second))
					{
						size_y = utl::asInt(fileTransform.second);
					}
				}
				else if(fileTransform.first == "rotate")
				{
					if(utl::isNumber(fileTransform.second))
					{
						degrees = utl::asInt(fileTransform.second);
					}
				}
				else if(fileTransform.first == "flip")
				{
					if(fileTransform.second.at(0) == 'x' ||
					        fileTransform.second.at(0) == 'y' ||
					        fileTransform.second.at(0) == 'b' ||
					        fileTransform.second.at(0) == 'n')
					{
						flip = fileTransform.second.at(0);
					}
				}
			}

			TileTransform tile = TileTransform(texturePath);
			tile.tileSize = utl::Vector2i(size_x, size_y);
			tile.flip = flip;
			tile.degrees = degrees;

			tileTransform.emplace(std::make_pair(tileID, tile));
		}
		else if(!(filePath.substr(filePath.length() - 4) == ".png"))
		{
			std::cout << "TILEENGINE - Tile extension: '" << filePath << "' is not accepted <Utl/SaveFile/SaveFile_TileEngine.cpp>" << std::endl;
			textureAtlas.addTexture(utl::asString(tileData.first), "Media/Image/Game/Tiles/Debug/Missing.png");
		}
	}


	//now texture exists in resourceManager, compiledTexture only holds a pointer to the texture
	if(mapData.getTileMode() == TileMap::TileMode::Batch)
	{
		compiledTexture = textureAtlas.compileTextures("TILESTORAGE");
		mapData.setAtlasTexture("TILESTORAGE", &resourceManager.getTexture("TILESTORAGE"));
	}

	//load tileInfo into mapData
	for(auto& tileData : tileLocations)
	{
		const int& tileID = tileData.first;

		std::string& texturePath = tileTransform.at(pngIdDirectory[tileID]).texturePath;
		utl::Vector2i& tileSize  = tileTransform.at(tileID).tileSize;
		int& degrees             = tileTransform.at(tileID).degrees;
		char& flip               = tileTransform.at(tileID).flip;

		mapData.getTileKey().emplace(tileID, Tile(window, resourceManager));

		switch(mapData.getTileMode())
		{
		case TileMap::TileMode::Batch:
			{
				mapData.getTileKey().at(tileID).tileType = Tile::TileType::texture;
				mapData.getTileKey().at(tileID).texturePosition = compiledTexture.textureCoords.at(utl::asString(pngIdDirectory[tileID]));
			}
			break;
		case TileMap::TileMode::Sprite:
			{
				mapData.getTileKey().at(tileID).tileType = Tile::TileType::sprite;
				if(resourceManager.getTexture(texturePath).getSize().x > 0)
				{
					mapData.getTileKey().at(tileID).setTexture(texturePath);
					mapData.getTileKey().at(tileID).source = tileTransform.at(tileID).texturePath;
				}
				else
				{
					std::string tempTexturePath = texturePath;
					tempTexturePath.resize(tempTexturePath.length() - 4);
					tempTexturePath = utl::conjoinString( {tempTexturePath,".png"});
					if(resourceManager.getTexture(tempTexturePath).getSize().x > 0)
					{
						mapData.getTileKey().at(tileID).setTexture(tempTexturePath);

						//TODO fix this along with implementation in Tile
						mapData.getTileKey().at(tileID).source = texturePath;

						std::vector<std::string> tileSourceDir = utl::separateString(texturePath, '/');
						mapData.getTileKey().at(tileID).folder   = tileSourceDir[4];
						mapData.getTileKey().at(tileID).tileName = tileSourceDir[5];
					}
					else
					{
						//TODO fix this along with implementation in Tile
						mapData.getTileKey().at(tileID).setTexture("Media/Image/Game/Tiles/Debug/Missing.png");
						mapData.getTileKey().at(tileID).source = texturePath;

						std::vector<std::string> tileSourceDir = utl::separateString(texturePath, '/');
						mapData.getTileKey().at(tileID).folder   = tileSourceDir[4];
						mapData.getTileKey().at(tileID).tileName = tileSourceDir[5];
					}
				}
			}
			break;
		}
		mapData.getTileKey().at(tileID).setSize(tileSize.x,tileSize.y);
		mapData.getTileKey().at(tileID).setRotate(degrees);
		mapData.getTileKey().at(tileID).setFlip(flip);
	}
}

void SaveFile_TileEngine::saveMap(std::string mapName, const std::vector <int>& tileMap, unsigned int width, unsigned int height, unsigned int layers, const std::map<int, Tile>& tilePairs)
{
	if(utl::doesExist(utl::conjoinString( {"Maps/", mapName})))
	{
		std::string filePath = utl::conjoinString( {"Maps/", mapName});

		//create info file
		SaveFile saveFile_mapInfo;
		saveFile_mapInfo.setFilePath(filePath + file_mapInfo);
		saveFile_mapInfo.saveItem(name_width, width);
		saveFile_mapInfo.saveItem(name_height, height);
		saveFile_mapInfo.saveItem(name_layers, layers);
		saveFile_mapInfo.writeFile();

		//create tile info file
		SaveFile saveFile_tilesUsed;
		saveFile_tilesUsed.setFilePath(filePath + file_tilesUsed);

		std::vector<int> tilesUsed = std::vector<int>();
		for(const int& tileID : tileMap)
		{
			if(std::find(tilesUsed.begin(), tilesUsed.end(), tileID) == tilesUsed.end())
			{
				if(tileID != 0)
				{
					tilesUsed.push_back(tileID);
				}
			}
		}

		std::map<int, int> newTileNumbers =  std::map<int, int>();

		int currentTileID = 1;
		for(const int& tileID : tilesUsed)
		{
			if(tilePairs.at(tileID).source == "Media/Image/Game/Tiles/Debug/Missing.png")
			{
				saveFile_tilesUsed.saveItem("-1", "Media/Image/Game/Tiles/Debug/Missing.png");
				newTileNumbers[-1] = -1;
			}
			else
			{
				saveFile_tilesUsed.saveItem(utl::asString(currentTileID), tilePairs.at(tileID).source);
				newTileNumbers[tileID] = currentTileID;
				currentTileID++;
			}
		}
		saveFile_tilesUsed.writeFile();

		//create map save
		SaveFile saveFile_map;
		saveFile_map.setFilePath(filePath + file_map);

		int tileMapIterator = 0;

		//save map
		for(unsigned int itLayers = 0; itLayers != layers; itLayers++)
		{
			for(unsigned int itHeight = 0; itHeight != height; itHeight++)
			{
				//get the map string
				std::vector <std::string> tileLine;
				//-1 to account for end
				for(unsigned int it = 0; it != width - 1; it++)
				{
					tileLine.push_back(utl::asString(newTileNumbers[tileMap.at(tileMapIterator)]) + "|");
					tileMapIterator++;
				}
				tileLine.push_back(utl::asString(newTileNumbers[tileMap.at(tileMapIterator)]));
				tileMapIterator++;
				saveFile_map.addItem(utl::conjoinString(tileLine), "");
			}
			//line between layers
			if(itLayers != layers - 1)
			{
				saveFile_map.addItem("", "");
			}
		}

		saveFile_map.writeFile();
	}
}

///editor
bool SaveFile_TileEngine::createMap(std::string mapName, unsigned int width, unsigned int height, unsigned int layers)
{
	//file paths
	std::string filePath = "Maps/" + mapName;

	//make sure not to override any other map
	if(utl::doesExist(filePath))
	{
		return false;
	}
	//make sure directory is created
	if(!utl::createDirectory(filePath))
	{
		return false;
	}

	//create info file
	SaveFile saveFile_mapInfo;
	saveFile_mapInfo.setFilePath(filePath + file_mapInfo);
	saveFile_mapInfo.saveItem(name_width, width);
	saveFile_mapInfo.saveItem(name_height, height);
	saveFile_mapInfo.saveItem(name_layers, layers);
	saveFile_mapInfo.writeFile();

	//create tile info file
	SaveFile saveFile_tilesUsed;
	saveFile_tilesUsed.setFilePath(filePath + file_tilesUsed);
	//temp
	saveFile_tilesUsed.addItem("-1", "Media/Image/Game/Tiles/Debug/Missing.png");
	saveFile_tilesUsed.writeFile();

	//create map save
	SaveFile saveFile_map;
	saveFile_map.setFilePath(filePath + file_map);
	//get the map string
	const std::string nullTile  =    "0|";
	const std::string nullTileEnd  = "0";
	std::vector <std::string> tileLine;
	//-1 to account for end
	for(unsigned int it = 0; it != width - 1; it++)
	{
		tileLine.push_back(nullTile);
	}
	tileLine.push_back(nullTileEnd);
	//save map
	for(unsigned int itLayers = 0; itLayers != layers; itLayers++)
	{
		for(unsigned int itHeight = 0; itHeight != height; itHeight++)
		{
			saveFile_map.addItem(utl::conjoinString(tileLine), "");
		}
		//line between layers
		if(itLayers != layers - 1)
		{
			saveFile_map.addItem("", "");
		}
	}

	saveFile_map.writeFile();

	return true;
}

int SaveFile_TileEngine::getTile(unsigned int x, unsigned int y, unsigned int layer, TileMap& tileMap)
{
	return x + (tileMap.getWidth() * y) + (layer * tileMap.getWidth() * tileMap.getHeight());
}
int SaveFile_TileEngine::getChunk(unsigned int x, unsigned int y, unsigned int layer, TileMap& tileMap)
{
	return x + (tileMap.getChunks_x() * y) + (layer * tileMap.getChunks_x() * tileMap.getChunks_y());
}
