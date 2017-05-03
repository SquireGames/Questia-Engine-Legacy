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

	///mapInfo
	//reads mapInfo
	SaveFile saveFile_mapInfo;
	saveFile_mapInfo.setFilePath(utl::conjoinString( {"Maps/", mapName, "/mapInfo.txt"}));
	saveFile_mapInfo.readFile();
	mapData.setWidth(utl::asInt(saveFile_mapInfo.getItem(name_width)));
	mapData.setHeight(utl::asInt(saveFile_mapInfo.getItem(name_height)));
	mapData.setLayers(utl::asInt(saveFile_mapInfo.getItem(name_layers)));
	unsigned int totalTiles = mapData.getWidth() * mapData.getHeight() * mapData.getLayers();

	//get all tile textures
	std::vector <std::pair <int, std::string> > tileLocations = getTileLocations(utl::conjoinString( {"Maps/", mapName}), textureMode);

	///load tile textures into resourceManager, tileData into mapData.tileStorage
	loadTiles(tileLocations, mapData, tileMode, window);

	///tileMap
	mapData.getTileMap().resize(totalTiles);
	//fill with nil
	std::fill(mapData.getTileMap().begin(), mapData.getTileMap().end(), 0);
	//open map file
	SaveFile saveFile_map;
	saveFile_map.setFilePath(utl::conjoinString( {"Maps/", mapName, "/map.txt"}));
	saveFile_map.readFile();

	//get tiles
	std::vector <std::string> tileVector = saveFile_map.getItemList();
	unsigned int tileNumberIterator = 0;

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

	std::cout << "Tiles: " << tileNumberIterator << std::endl;
	return mapData;
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
		{

		}
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
			std::vector<std::string> tilesDirs = utl::getAllFiles("Media/Image/Game/Tiles/", true);

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

void SaveFile_TileEngine::loadTiles(std::vector <std::pair <int, std::string> >& tileLocations, TileMap& mapData, TileMap::TileMode tileMode, sf::RenderWindow& window)
{
	//used to hold transform data for each tile
	std::map<int, TileTransform> tileTransform;
	//only used for Batching
	TextureAtlasData compiledTexture = TextureAtlasData(nullptr);

	//for getting txt file references to pngs
	std::map<int, int> pngIdDirectory;
	for(auto& tileData : tileLocations)
	{
		pngIdDirectory.emplace(std::make_pair(tileData.first, tileData.first));
	}

	//loads textures into textureAtlas
	if(tileMode == TileMap::TileMode::Batch)
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
				texturePath = utl::conjoinString( {texturePath,".png"});

				//will not do anything if not a valid filepath (not a reference)
				if(textureAtlas.addTexture(utl::asString(tileData.first), texturePath))
				{
					std::cout << "TILEENGINE - Reference texture: " << texturePath << "(" << utl::asString(tileData.first) << ")" << std::endl;
				}
			}
			else
			{
				std::cout << "TILEENGINE - Tile extension: '" << filePath << "' is not accepted <Utl/SaveFile/SaveFile_TileEngine.cpp>" << std::endl;
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
	compiledTexture = textureAtlas.compileTextures("TILESTORAGE");

	//load tileInfo into mapData
	for(auto& tileData : tileLocations)
	{
		const int& tileID = tileData.first;

		std::string& texturePath = tileTransform.at(pngIdDirectory[tileID]).texturePath;
		utl::Vector2i& tileSize  = tileTransform.at(tileID).tileSize;
		int& degrees             = tileTransform.at(tileID).degrees;
		char& flip               = tileTransform.at(tileID).flip;

		mapData.getTileKey().emplace(tileID, Tile(window, resourceManager));

		switch(tileMode)
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
	std::string filePath = utl::conjoinString( {"Maps/", mapName});

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

///helper
int SaveFile_TileEngine::getTile(unsigned int x, unsigned int y, unsigned int layer, unsigned int mapWidth, unsigned int mapHeight)
{
	return x + (mapWidth * y) + (layer * mapWidth * mapHeight);
}
