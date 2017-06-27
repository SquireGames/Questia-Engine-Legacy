#include "QuestiaEng/TileEngine/SV_TileEngine.h"

SV_TileEngine::SV_TileEngine(ResourceManager& _resourceManager):
	resourceManager(_resourceManager)
	, textureAtlas(_resourceManager)
{

}
SV_TileEngine::~SV_TileEngine()
{
	//dtor
}

TileMap SV_TileEngine::openMap(const std::string& mapName, sf::RenderWindow& window, TileMap::TextureMode textureMode, TileMap::RenderMode tileMode)
{
	//create map
	TileMap mapData;

	//make sure directory exists
	if(!utl::doesExist("Data/Maps/" + mapName))
	{
		throw std::runtime_error("ERROR: Reading map- Data/Maps/" + mapName + " <Utl/SaveFile/SV_TileEngine.cpp>");
		return mapData;
	}

	mapData.setName(mapName);
	mapData.setTextureMode(textureMode);
	mapData.setRenderMode(tileMode);

	//reads mapInfo file
	SaveFile sv_mapInfo;
	sv_mapInfo.setFilePath("Data/Maps/" + mapName + "/mapInfo.txt");
	sv_mapInfo.readFile();
	mapData.setWidth(utl::toInt(sv_mapInfo.getItem(name_width)));
	mapData.setHeight(utl::toInt(sv_mapInfo.getItem(name_height)));
	mapData.setLayers(utl::toInt(sv_mapInfo.getItem(name_layers)));
	mapData.setBorderMap(sv_mapInfo.getItem(name_map_up), utl::Direction::up);
	mapData.setBorderMap(sv_mapInfo.getItem(name_map_down), utl::Direction::down);
	mapData.setBorderMap(sv_mapInfo.getItem(name_map_left), utl::Direction::left);
	mapData.setBorderMap(sv_mapInfo.getItem(name_map_right), utl::Direction::right);
	mapData.setBorderMapOffset(utl::toInt(sv_mapInfo.getItem(name_map_up_off)), utl::Direction::up);
	mapData.setBorderMapOffset(utl::toInt(sv_mapInfo.getItem(name_map_down_off)), utl::Direction::down);
	mapData.setBorderMapOffset(utl::toInt(sv_mapInfo.getItem(name_map_left_off)), utl::Direction::left);
	mapData.setBorderMapOffset(utl::toInt(sv_mapInfo.getItem(name_map_right_off)), utl::Direction::right);

	unsigned int totalTiles = mapData.getWidth() * mapData.getHeight() * mapData.getLayers();

	//get all tile textures locations
	std::vector <std::pair <int, std::string> > tileLocations = getTileLocations("Data/Maps/" + mapName, textureMode);

	///load tile textures and info into resourceManager, tileData into tileStorage
	loadTiles(tileLocations, mapData, window, mapName);

	///tileMap
	mapData.getTileMap().resize(totalTiles);
	//fill as empty
	std::fill(mapData.getTileMap().begin(), mapData.getTileMap().end(), 0);
	//open map file
	SaveFile saveFile_map;
	saveFile_map.setFilePath("Data/Maps/" + mapName + "/map.txt");
	saveFile_map.readFile();
	//get tiles saved in map
	std::vector <std::string> tileVector = saveFile_map.getItemKeyList();
	unsigned int tileNumberIterator = 0; // number of tiles in the map
	for(const std::string& tileLine : tileVector)
	{
		std::vector <std::string> tiles = utl::separateString(tileLine, '|');
		for(std::string& tile : tiles)
		{
			//make sure it is not empty
			if(utl::isNumber(tile) && utl::toInt(tile) != 0)
			{
				mapData.getTileMap()[tileNumberIterator] = utl::toInt(tile);
			}
			if(tile.size() > 0 && tile.at(0) != '\n')
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

void SV_TileEngine::loadRenderData(TileMap& mapData)
{
	//validate all sprites loaded into the map
	if(mapData.getRenderMode() == TileMap::RenderMode::Sprite)
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
	else if(mapData.getRenderMode() == TileMap::RenderMode::Batch)
	{
		//get max tile size
		for(unsigned int it = 0; it != mapData.getTileMap().size(); it++)
		{
			int tileID = mapData.getTileMap()[it];
			//if does not exist, replace with -1 and give error message
			if(tileID != -1 && tileID != 0)
			{
				if(mapData.getTileKey().at(tileID).getSize_x() > (int)mapData.getMaxTileSize_x())
				{
					mapData.setMaxTileSize_x(mapData.getTileKey().at(tileID).getSize_x());
				}
				if(mapData.getTileKey().at(tileID).getSize_y() > (int)mapData.getMaxTileSize_y())
				{
					mapData.setMaxTileSize_y(mapData.getTileKey().at(tileID).getSize_y());
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
								utl::IntRect texturePosition = tileData.texturePosition;
								//size
								utl::Vector2i tileSize = tileData.getSize();
								//rotation
								int rotationDegrees = tileData.getRotate();
								//flips
								char flip = tileData.getFlip();

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

std::vector <std::pair <int, std::string> > SV_TileEngine::getTileLocations(const std::string& fileDir, TileMap::TextureMode textureMode)
{
	//to get block datas
	std::string filePath = fileDir + file_tilesUsed;

	//return vector
	std::vector<std::pair <int, std::string> > returnTiles = std::vector<std::pair <int, std::string> >();

	SaveFile saveFile_tileLocations;
	saveFile_tileLocations.setFilePath(filePath);
	saveFile_tileLocations.readFile();
	std::vector<std::pair<std::string, std::string>> tileLocations = saveFile_tileLocations.getSaveList();
	for(std::pair<std::string, std::string> tileLocation : tileLocations)
	{
		returnTiles.push_back(std::make_pair(utl::toInt(tileLocation.first), tileLocation.second));
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
			std::vector<std::string> tileNumbers_str = saveFile_tileLocations.getItemKeyList();
			std::vector<int> tileNumbers = std::vector<int>();
			for(const std::string& tileStr : tileNumbers_str)
			{
				tileNumbers.push_back(utl::toInt(tileStr));
			}
			int currentID = 1;
			if(tileNumbers.size() > 0)
			{
				currentID = std::max(1, *std::max_element(tileNumbers.begin(), tileNumbers.end()) + 1);
			}

			//get all files
			std::vector<std::string> tilesDirs = utl::getAllFiles("Media/Image/Game/Tiles", true);

			tilesDirs = utl::filterDirs(tilesDirs);

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
		if(tilePair.second.substr(tilePair.second.size() - 4) == ".png")
		{
			std::string childTile = tilePair.second.substr(0, tilePair.second.size() - 4) + ".txt";
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

void SV_TileEngine::loadTiles(std::vector <std::pair <int, std::string>>& tileLocations, TileMap& mapData, sf::RenderWindow& window, const std::string& mapName)
{
	std::map<int, Tile>& mapTiles = mapData.getTileKey();

	//calculate the largest ID from the given tileLocations, if adding more is necessary
	int largestID = 1;
	{
		std::vector<int> ids;
		std::transform(tileLocations.begin(), tileLocations.end(), std::back_inserter(ids), [](std::pair <int, std::string> t) {return t.first;});
		ids.push_back(1);
		largestID = *std::max_element(ids.begin(), ids.end());
	}

	//makes a tile for all .png files, including those
	//not in tileLocations but referenced in txt files
	for(std::pair <int, std::string>& tileLoc : tileLocations)
	{
		const int& tileID = tileLoc.first;
		const std::string& filePath = tileLoc.second;
		if(filePath.substr(filePath.size() - 4) == ".png")
		{
			//TODO make sure there is defined behavior for new Tile
			mapTiles.emplace(tileID, Tile(window, resourceManager));
			mapTiles.at(tileID).setTexture(filePath);
			mapTiles.at(tileID).setSource(filePath);
		}
	}
	//appends to loop while iterating over it
	for(unsigned int i = 0; i < tileLocations.size(); i++)
	{
		const std::string& filePath = tileLocations.at(i).second;
		if(filePath.substr(filePath.size() - 4) == ".txt")
		{
			//see if there are any pngs to load to be overwritten
			std::string pngPath = filePath.substr(0, filePath.size() - 4) + ".png";
			if(utl::doesExist(pngPath))
			{
				bool isInTileLocs = false;
				for(std::pair <int, std::string>& t : tileLocations)
				{
					if(t.second == pngPath)
					{
						isInTileLocs = true;
						break;
					}
				}
				if(!isInTileLocs)
				{
					mapTiles.emplace(++largestID, Tile(window, resourceManager));
					tileLocations.push_back(std::make_pair(largestID, pngPath));
					mapTiles.at(largestID).setTexture(pngPath);
					mapTiles.at(largestID).setSource(filePath);
					continue;
				}
			}
			//create any referenced textures if necessary
			SaveFile sv_tileData(filePath);
			if(!sv_tileData.readFile())
			{
				continue;
			}
			for(const std::pair<std::string, std::string>& fileTransform : sv_tileData.getSaveList())
			{
				if(fileTransform.first == "texture"
				        && fileTransform.second.size() >= 5
				        && fileTransform.second.substr(fileTransform.second.size() - 4) == ".png"
				        && utl::doesExist(fileTransform.second))
				{
					bool isInTileLocs = false;
					for(std::pair <int, std::string>& t : tileLocations)
					{
						if(t.second == fileTransform.second)
						{
							isInTileLocs = true;
							break;
						}
					}
					if(!isInTileLocs)
					{
						mapTiles.emplace(++largestID, Tile(window, resourceManager));
						tileLocations.push_back(std::make_pair(largestID, fileTransform.second));
						mapTiles.at(largestID).setTexture(fileTransform.second);
						mapTiles.at(largestID).setSource(fileTransform.second);
						break;
					}
				}
			}
		}
	}

	//makes a tile for all .txt files
	for(auto& tileData : tileLocations)
	{
		const int& tileID = tileData.first;
		const std::string& filePath = tileData.second;
		if(filePath.substr(filePath.size() - 4) == ".txt")
		{
			Tile* tile;

			//first see if the txt is overriding a png tile
			auto pairLoc = std::find_if(tileLocations.begin(), tileLocations.end(), [filePath, tileID](std::pair<int, std::string> it)
			{
				return (it.first != tileID)
				       && (it.second.substr(it.second.size() - 4) == ".png")
				       && (it.second.substr(0, it.second.size() - 4) == filePath.substr(0, filePath.size() - 4));
			});
			//if there is an override
			if(pairLoc != tileLocations.end())
			{
				tile = &mapTiles.at((*pairLoc).first);
				tile->setDisplay(filePath);
			}
			else
			{
				mapTiles.emplace(tileID, Tile(window, resourceManager));
				tile = &mapTiles.at(tileID);
				tile->setSource(filePath);
			}

			//load data from text file
			SaveFile sv_tileData(filePath);
			if(!sv_tileData.readFile())
			{
				continue;
			}
			for(const std::pair<std::string, std::string>& fileTransform : sv_tileData.getSaveList())
			{
				if(fileTransform.first == "texture")
				{
					//if texture exists, link the texture to the tile
					auto tileIt = std::find_if(tileLocations.begin(), tileLocations.end(), [fileTransform](std::pair<int, std::string> it)
					{
						return it.second.substr(it.second.size() - 4) == ".png"
						       && (it.second == fileTransform.second);
					});
					if(tileIt != tileLocations.end())
					{
						tile->setTexture((*tileIt).second);
						tile->setDisplay(filePath);
					}
					//TODO add warnings and log problems
				}
				else if(fileTransform.first == "size_x" && utl::isNumber(fileTransform.second))
				{
					tile->setSize_x(utl::toInt(fileTransform.second));
				}
				else if(fileTransform.first == "size_y" && utl::isNumber(fileTransform.second))
				{
					tile->setSize_y(utl::toInt(fileTransform.second));
				}
				else if(fileTransform.first == "rotate" && utl::isNumber(fileTransform.second))
				{
					tile->setRotation(utl::toInt(fileTransform.second));
				}
				else if(fileTransform.first == "flip"
				        && (fileTransform.second.at(0) 	 == 'x'
				            ||fileTransform.second.at(0) == 'y'
				            ||fileTransform.second.at(0) == 'b'
				            ||fileTransform.second.at(0) == 'n'))
				{
					tile->setFlip(fileTransform.second.at(0));
				}
			}
		}//end if(filePath.substr(filePath.size() - 4) == ".txt")
		else if(!(filePath.substr(filePath.size() - 4) == ".png"))
		{
			std::cout << "TILEENGINE - File: '" << filePath << "' has an invalid file extension <Questia/TileEngine/SV_TileEngine.cpp>" << std::endl;
		}
	}

	//load render data
	switch(mapData.getRenderMode())
	{
	case TileMap::RenderMode::Batch:
		{
			//load tiles into textureAtlas
			TextureAtlasData compiledTexture = TextureAtlasData(nullptr);
			for(auto& tileData : tileLocations)
			{
				const std::string& filePath = tileData.second;

				if(filePath.substr(filePath.size() - 4) == ".png")
				{
					if(!textureAtlas.addTexture(std::to_string(tileData.first), tileData.second))
					{
						std::cout << "TILEENGINE - Tile: '" << filePath << "' failed to load <Questia/TileEngine/SV_TileEngine.cpp>" << std::endl;
					}
				}
			}
			compiledTexture = textureAtlas.compileTextures("TILESTORAGE_" + mapName);
			mapData.setAtlasTexture("TILESTORAGE_" + mapName, &resourceManager.getTexture("TILESTORAGE_" + mapName));

			//set texture coordinates for every tile
			for(std::pair<const int, Tile>& tile : mapTiles)
			{
				bool isInTileLocs = false;
				int id;
				for(std::pair <int, std::string>& t : tileLocations)
				{
					if(t.second == tile.second.getTexturePath())
					{
						isInTileLocs = true;
						id = t.first;
						break;
					}
				}
				if(isInTileLocs)
				{
					tile.second.texturePosition = compiledTexture.textureCoords.at(std::to_string(id));
				}
			}
			break;
		}
	case TileMap::RenderMode::Sprite:
		{
			for(std::pair<const int, Tile>& tile : mapTiles)
			{
				tile.second.loadSprite();
			}
		}
		break;
	case TileMap::RenderMode::Unloaded:
		break;
	}
}

void SV_TileEngine::saveMap(TileMap* map)
{
	if(!utl::doesExist("Data/Maps/" + map->getName()))
	{
		return;
	}

	const std::map<int, Tile>& tilePairs = map->getTileKey();
	const std::vector<int>& tileMap = map->getTileMap();
	unsigned int width = map->getWidth();
	unsigned int height = map->getHeight();
	unsigned int layers = map->getLayers();

	std::string filePath = "Data/Maps/" + map->getName();

	//create info file
	SaveFile sv_mapInfo;
	sv_mapInfo.setFilePath(filePath + file_mapInfo);
	sv_mapInfo.saveItem(name_width, width);
	sv_mapInfo.saveItem(name_height, height);
	sv_mapInfo.saveItem(name_layers, layers);

	sv_mapInfo.saveItem(name_map_up, 	map->getBorderMap(utl::Direction::up));
	sv_mapInfo.saveItem(name_map_down, 	map->getBorderMap(utl::Direction::down));
	sv_mapInfo.saveItem(name_map_left,	map->getBorderMap(utl::Direction::left));
	sv_mapInfo.saveItem(name_map_right, map->getBorderMap(utl::Direction::right));

	sv_mapInfo.saveItem(name_map_up_off, 	map->getBorderMapOffset(utl::Direction::up));
	sv_mapInfo.saveItem(name_map_down_off, 	map->getBorderMapOffset(utl::Direction::down));
	sv_mapInfo.saveItem(name_map_left_off, 	map->getBorderMapOffset(utl::Direction::left));
	sv_mapInfo.saveItem(name_map_right_off,	map->getBorderMapOffset(utl::Direction::right));

	sv_mapInfo.writeFile();

	//create tile info file
	SaveFile sv_tilesUsed;
	sv_tilesUsed.setFilePath(filePath + file_tilesUsed);

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
		if(tilePairs.at(tileID).getTexturePath() == "Media/Image/Game/Tiles/Debug/Missing.png")
		{
			sv_tilesUsed.saveItem("-1", "Media/Image/Game/Tiles/Debug/Missing.png");
			newTileNumbers[-1] = -1;
		}
		else
		{
			sv_tilesUsed.saveItem(std::to_string(currentTileID), tilePairs.at(tileID).getSource());
			newTileNumbers[tileID] = currentTileID;
			currentTileID++;
		}
	}
	sv_tilesUsed.writeFile();

	//create map save
	SaveFile sv_map;
	sv_map.setFilePath(filePath + file_map);

	int tileMapIterator = 0;

	//save map
	for(unsigned int itLayers = 0; itLayers < layers; itLayers++)
	{
		for(unsigned int itHeight = 0; itHeight < height; itHeight++)
		{
			//get the map string
			std::vector <std::string> tileLine;
			//-1 to account for end
			for(unsigned int it = 0; it < width - 1; it++)
			{
				tileLine.push_back(std::to_string(newTileNumbers[tileMap.at(tileMapIterator)]) + "|");
				tileMapIterator++;
			}
			tileLine.push_back(std::to_string(newTileNumbers[tileMap.at(tileMapIterator)]));
			tileMapIterator++;
			sv_map.addItem(utl::conjoinString(tileLine), "");
		}
		//line between layers
		if(itLayers != layers - 1)
		{
			sv_map.addItem("", "");
		}
	}
	sv_map.writeFile();
}

///editor
bool SV_TileEngine::createMap(const std::string& mapName, unsigned int width, unsigned int height, unsigned int layers)
{
	//file paths
	std::string filePath = "Data/Maps/" + mapName;

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
	SaveFile sv_mapInfo;
	sv_mapInfo.setFilePath(filePath + file_mapInfo);
	sv_mapInfo.saveItem(name_width, width);
	sv_mapInfo.saveItem(name_height, height);
	sv_mapInfo.saveItem(name_layers, layers);
	sv_mapInfo.saveItem(name_map_up, std::string());
	sv_mapInfo.saveItem(name_map_down, std::string());
	sv_mapInfo.saveItem(name_map_left, std::string());
	sv_mapInfo.saveItem(name_map_right, std::string());
	sv_mapInfo.saveItem(name_map_up_off, 0);
	sv_mapInfo.saveItem(name_map_down_off, 0);
	sv_mapInfo.saveItem(name_map_left_off, 0);
	sv_mapInfo.saveItem(name_map_right_off, 0);
	sv_mapInfo.writeFile();

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

int SV_TileEngine::getTile(unsigned int x, unsigned int y, unsigned int layer, TileMap& tileMap)
{
	return x + (tileMap.getWidth() * y) + (layer * tileMap.getWidth() * tileMap.getHeight());
}
int SV_TileEngine::getChunk(unsigned int x, unsigned int y, unsigned int layer, TileMap& tileMap)
{
	return x + (tileMap.getChunks_x() * y) + (layer * tileMap.getChunks_x() * tileMap.getChunks_y());
}
