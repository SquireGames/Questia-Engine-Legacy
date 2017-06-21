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
	~TileEngine();

	//loads map and textures
	void loadMap(const std::string& mapName, TileMap::TextureMode textureMode = TileMap::TextureMode::Map, TileMap::RenderMode tileMode = TileMap::RenderMode::Batch);

	//just loads map data
	void loadMapData(); 
	
	void closeMap();

	void draw();

	void setViewportSize(float width, float height);
	void setPosition(int x, int y);

	bool isLoaded() {return currentMap->isLoaded();}

private:
	void loadMap(TileMap* map, const std::string& mapName, TileMap::TextureMode textureMode = TileMap::TextureMode::Map, TileMap::RenderMode tileMode = TileMap::RenderMode::Batch);

	//draws chunks
	void drawMap();
	//draws separate tiles
	void drawTiles();

	///helpers
	//for map
	int getTile(unsigned int x, unsigned int y, unsigned int layer, TileMap* map);
	int getChunk(unsigned int x, unsigned int y, unsigned int layer, TileMap* map);
	
	//every map is generated an ID
	std::unordered_map<std::string, int> mapID;
	
	//tiles fit on screen
	unsigned int tileFit_x = (1920.f / 64.f) + 2; // +2 for transitioning tiles
	unsigned int tileFit_y = (1080.f / 64.f) + 2; // +2 for transitioning tiles

	utl::Vector2i cameraPosition {utl::Vector2i(0,0)};
	TileMap::TextureMode textureMode = TileMap::TextureMode::Map;
	TileMap::RenderMode tileMode = TileMap::RenderMode::Batch;

	///default
	sf::RenderWindow& window;
	ResourceManager& resourceManager;

	//holds map
	TileMap maps[2];
	TileMap* currentMap;
	TileMap* nextMap;

	friend class TileEngine_Editor;
};

#endif // TILEENGINE_H
