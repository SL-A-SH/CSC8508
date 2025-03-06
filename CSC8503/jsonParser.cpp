#include "jsonParser.h"
#include <iostream>
#include <fstream>

using namespace std;

// JSON Deserialization Implementations
void from_json(const json& j, Position& p) {
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
}

void from_json(const json& j, Dimensions& d) {
    j.at("x").get_to(d.x);
    j.at("y").get_to(d.y);
    j.at("z").get_to(d.z);
}

void from_json(const json& j, InGameObject& obj) {
    j.at("id").get_to(obj.id);
    j.at("type").get_to(obj.type);
    j.at("position").get_to(obj.position);
    j.at("dimensions").get_to(obj.dimensions);
}

void from_json(const json& j, LevelData& level) {
    j.at("level").get_to(level.level);
    j.at("objects").get_to(level.objects);
}

// Function to load level from JSON file
LevelData loadLevelFromJson(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        return LevelData{};
    }

    json levelJson;
    file >> levelJson;

    return levelJson.get<LevelData>();
}

// Function to process loaded level data
void processLevelData(const LevelData& levelData) {
    cout << "Loaded Level: " << levelData.level << endl;

    for (const auto& obj : levelData.objects) {
        cout << "Object ID: " << obj.id << ", Type: " << obj.type << endl;
        cout << "   Position: (" << obj.position.x << ", " << obj.position.y << ", " << obj.position.z << ")\n";
        cout << "   Dimensions: (" << obj.dimensions.x << ", " << obj.dimensions.y << ", " << obj.dimensions.z << ")\n";

        // object creation functions to work on later after read json files are implemented
        if (obj.type == "Wall") {
            addWallToWorld(obj.position.x, obj.position.y, obj.position.z,
                obj.dimensions.x, obj.dimensions.y, obj.dimensions.z);
        }
        else if (obj.type == "Box") {
            addBoxToWorld(obj.position.x, obj.position.y, obj.position.z,
                obj.dimensions.x, obj.dimensions.y, obj.dimensions.z);
        }
        else if (obj.type == "Button") {
            addButtonToWorld(obj.position.x, obj.position.y, obj.position.z);
        }
        else if (obj.type == "Exit") {
            addExitToWorld(obj.position.x, obj.position.y, obj.position.z);
        }
        else if (obj.type == "Floor") {
            addWallToWorld(obj.position.x, obj.position.y, obj.position.z,
                obj.dimensions.x, obj.dimensions.y, obj.dimensions.z); 
        }
        else {
            cerr << "Warning: Unknown object type '" << obj.type << "'" << endl;
        }
    }

    cout << "Level " << levelData.level << " loaded successfully!" << endl;
}
