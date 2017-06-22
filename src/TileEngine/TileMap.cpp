#include "QuestiaEng/TileEngine/TileMap.h"

void TileMap::setBorderMap(const std::string& name, utl::Direction dir)
{
	switch(dir)
	{
	case utl::Direction::up:
		borderMaps[0] = name;
		break;
	case utl::Direction::down:
		borderMaps[1] = name;
		break;
	case utl::Direction::left:
		borderMaps[2] = name;
		break;
	case utl::Direction::right:
		borderMaps[3] = name;
		break;
	default:
		break;
	}
}

void TileMap::setBorderMapOffset(int tileOffset, utl::Direction dir)
{
	switch(dir)
	{
	case utl::Direction::up:
		borderMapOffsets[0] = tileOffset;
		break;
	case utl::Direction::down:
		borderMapOffsets[1] = tileOffset;
		break;
	case utl::Direction::left:
		borderMapOffsets[2] = tileOffset;
		break;
	case utl::Direction::right:
		borderMapOffsets[3] = tileOffset;
		break;
	default:
		break;
	}
}

const std::string& TileMap::getBorderMap(utl::Direction dir)
{
	switch(dir)
	{
	case utl::Direction::up:
		return borderMaps[0];
	case utl::Direction::down:
		return borderMaps[1];
	case utl::Direction::left:
		return borderMaps[2];
	case utl::Direction::right:
		return borderMaps[3];
	default:
		break;
	}
	//TODO log warning
	return borderMaps[0];;
}

int TileMap::getBorderMapOffset(utl::Direction dir)
{
	switch(dir)
	{
	case utl::Direction::up:
		return borderMapOffsets[0];
	case utl::Direction::down:
		return borderMapOffsets[1];
	case utl::Direction::left:
		return borderMapOffsets[2];
	case utl::Direction::right:
		return borderMapOffsets[3];
	default:
		break;
	}
	//TODO log warning
	return 0;
}
