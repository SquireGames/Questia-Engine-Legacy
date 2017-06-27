#ifndef TILEENGINE_H
#define TILEENGINE_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <cmath>    //std::floor

#include <unordered_map>

#include "QuestiaEng/TileEngine/TileMap.h"

#include "QuestiaEng/ResourceManager/ResourceManager.h"

#include "QuestiaEng/Utl/Utl.h"
#include "QuestiaEng/Utl/SpriteSheet.h"

#include "QuestiaEng/TileEngine/SV_TileEngine.h"

#include "QuestiaEng/TileEngine/Tile.h"

class TileEngine
{
public:
	//ctor and dtor
	TileEngine(sf::RenderWindow& window, ResourceManager& resourceManager);
	~TileEngine() = default;

	//only needs to be called to change default settings
	void setMode(TileMap::TextureMode textureMode, TileMap::RenderMode renderMode);

	//loads map and textures
	void loadMap(const std::string& mapName);

	//just loads map data
	void loadMapData();

	void closeMaps();

	//draws the surrounding maps if visible (9 possible)
	void draw();

	void setViewportSize(float width, float height);
	void setPosition(utl::Vector2f& pos);

	bool isLoaded() {return maps.size();}

private:
	//draws chunks
	void drawMaps();
	void drawMap(TileMap* map, utl::Direction dir, TileMap* refMap = nullptr, int tilesOffset = 0);
	//draws separate tiles
	void drawTiles();

	///helpers
	//for map
	int getTile(unsigned int x, unsigned int y, unsigned int layer, TileMap* map);
	int getChunk(unsigned int x, unsigned int y, unsigned int layer, TileMap* map);

	//tiles fit on screen
	unsigned int tileFit_x = (1920.f / 64.f) + 2; // +2 for transitioning tiles
	unsigned int tileFit_y = (1080.f / 64.f) + 2; // +2 for transitioning tiles

	utl::Vector2f cameraPosition {utl::Vector2f(0,0)};
	TileMap::TextureMode textureMode = TileMap::TextureMode::Map;
	TileMap::RenderMode renderMode = TileMap::RenderMode::Batch;

	sf::RenderWindow& window;
	ResourceManager& resourceManager;

	//every map is generated an ID
	int mapCount = 0;
	std::unordered_map<std::string, int> mapID;
	std::vector<int> activeMaps;
	std::vector<TileMap> maps;
	//used to allow functions to remain void
	TileMap* lastMap = nullptr;
	
	int currentMapID = -1;

	bool mapLoaded(const std::string& mapName)
	{
		return mapID.count(mapName) && std::find(activeMaps.begin(), activeMaps.end(), mapID.at(mapName)) != activeMaps.end();
	}
	TileMap* getMap(const std::string& mapName)
	{
		return mapLoaded(mapName) ? getMap(mapID.at(mapName)) : nullptr;
	}
	TileMap* getMap(int mapID)
	{
		for(auto& map : maps)
		{
			if(map.getID() == mapID)
			{
				return &map;
			}
		}
		return nullptr;
	}
	friend class TileEngine_Editor;
};

#endif // TILEENGINE_H
