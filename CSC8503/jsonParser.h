#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <vector>
#include <string>
#include "../CSC8503/Third Party/nlohmann/json.hpp" 

using json = nlohmann::json;
using namespace std;

// Structs for JSON mapping
struct Position {
    float x, y, z;
};

struct Dimensions {
    float x, y, z;
};

struct InGameObject {
    int id;
    string type;
    Position position;
    Dimensions dimensions;
};

struct LevelData {
    int level;
    vector<InGameObject> objects;
};

// Function to load level from a JSON file
LevelData loadLevelFromJson(const string& filename);

// Function to process the loaded level data
void processLevelData(const LevelData& levelData);

// JSON Deserialization Functions
void from_json(const json& j, Position& p);
void from_json(const json& j, Dimensions& d);
void from_json(const json& j, InGameObject& obj);
void from_json(const json& j, LevelData& level);

#endif // LEVEL_LOADER_H
