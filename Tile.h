#ifndef TILE_H
#define TILE_H

#include <string>
using namespace std;

// Board Tile Types
enum class TileType { EMPTY, RESOURCE, SAFE_ZONE, HAZARD, MARKET, CHEAT };

// Board Tile Type Structure
struct Tile {
    TileType type;
    string description;

    Tile(TileType t = TileType::EMPTY, string desc = "Empty Land")
    : type(t), description(desc) {}
};

#endif