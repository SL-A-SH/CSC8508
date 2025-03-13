#include "GameLevelManager.h"
#include "PrisonEscape/Levels/SampleLevel.h"
#include "Assets.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include <string>
#include <fstream>
#include <iostream>


using namespace NCL;
using namespace CSC8503;

GameLevelManager::GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer)
{
	mWorld = existingWorld;
	mRenderer = existingRenderer;
	mPhysics = new PhysicsSystem(*mWorld);
	mPhysics->UseGravity(true);
	// Move to another place if needed

	InitAssets();
	loadMap();
	
	

}

GameLevelManager::~GameLevelManager()
{
	delete mPhysics;

	for (int i = 0; i < mUpdatableObjectList.size(); i++) {
		delete(mUpdatableObjectList[i]);
	}
	mUpdatableObjectList.clear();

	for (auto& mesh : mMeshList) {
		delete mesh.second;
	}
	mMeshList.clear();

	delete mPlayerToAdd;
	delete mRenderer;
	delete mWorld;
}

void GameLevelManager::UpdateGame(float dt)
{
	// TODO: Level Updates
	// TODO: Remove Debug
	mPhysics->Update(dt);

	GetCurrentLevel()->Update(dt);

	if ((mUpdatableObjectList.size() > 0)) {
		for (GameObject* obj : mUpdatableObjectList) {
			obj->UpdateGame(dt);
		}
	}

	for (Button* button : buttons) {
		if (!button->IsPressed()) {
			button->pressDetection(pushableBox, "Box");
		}

		//if (button->IsPressed()) {
		//	std::cout << "BOOPED";
		//}
	}


	
	Debug::Print("LEVELS", Vector2(25, 30), Debug::WHITE);
}


void GameLevelManager::InitAssets()
{

	std::cout << "Initialising Assets" << std::endl;
	std::ifstream assetsFile(Assets::ASSETROOT + "AssetsToLoad.csv");

	if (!assetsFile) {
		std::cout << "ERROR: Unable To Load Asset File! Returning..." << std::endl;
		return;
	}
	else {
		std::cout << "Asset File Successfully Loaded..." << std::endl;
	}

	std::string line;
	std::string* assetInfo = new std::string[4];
	vector<std::string> groupInfo;
	std::string group = "";

	std::thread animationLoadThread;
	std::thread materialLoadThread;

	int fileSize = 0;

	while (getline(assetsFile, line)) {
		fileSize++;
		if (line.substr(0, line.find(",")) == "material") {
			fileSize++;
		}
	}

	assetsFile.clear();
	assetsFile.seekg(std::ifstream::beg);

	bool meshesLoaded = false;
	int meshCounted = 0;
	bool ready = false;

	int lines = 0;
	int materialLines = 0;
	int animationLines = 0;

	while (getline(assetsFile, line)) {

		//removes the , from the file
		for (int i = 0; i < 3; i++) {
			assetInfo[i] = line.substr(0, line.find(","));
			line.erase(0, assetInfo[i].length() + 1);
		}

		assetInfo[3] = line;

		if (group == "")
		{
			std::cout << "Group Not Found" << std::endl;
			group = assetInfo[0];
		}

		if (group != assetInfo[0]) {
			// Mesh Works
			if (group == "mesh") {
				std::cout << "Found Mesh: " << groupInfo[1] << std::endl;
				mRenderer->LoadMeshes(mMeshList, groupInfo);
				meshesLoaded = true;
			}

			else if (group == "anim") {
				std::cout << "Found Animation: " << groupInfo[1] << std::endl;

				if (animationLoadThread.joinable()) {
					animationLoadThread.join();
				}
				animationLoadThread = std::thread([this, groupInfo, &animationLines] {
					for (int i = 0; i < groupInfo.size(); i += 3) {
						mAnimationList[groupInfo[i]] = mRenderer->LoadAnimation(groupInfo[i + 1]);
						animationLines++;
					}
					});
			}

			else if (group == "shader") {
				std::cout << "Found Shader: " << groupInfo[1] << std::endl;
				for (int i = 0; i < groupInfo.size(); i += 3) {
					mShaderList[groupInfo[i]] = mRenderer->LoadShader(groupInfo[i + 1], groupInfo[i + 2]);
					lines++;
				}
			}

			else if (group == "texture") {
				std::cout << "Found Texture: " << groupInfo[1] << std::endl;
				for (int i = 0; i < groupInfo.size(); i += 3) {
					mTextureList[groupInfo[i]] = mRenderer->LoadTexture(groupInfo[i + 1]);
					lines++;
				}
			}

			else if (group == "material") {
				std::cout << "Found Material: " << groupInfo[1] << std::endl;
				if (materialLoadThread.joinable()) {
					materialLoadThread.join();
				}
				materialLoadThread = std::thread([this, groupInfo, &materialLines] {
					for (int i = 0; i < groupInfo.size(); i += 3) {
						mMaterialList[groupInfo[i]] = mRenderer->LoadMaterial(groupInfo[i + 1]);
						materialLines++;
					}
					});
			}
	
			group = assetInfo[0];
			groupInfo.clear();

		}

		for (int i = 1; i < 4; i++) {
			groupInfo.push_back(assetInfo[i]);
		}

	}
	delete[] assetInfo;

	if (animationLoadThread.joinable()) {
		animationLoadThread.join();
	}
	mPreLoadedAnimationList.insert(std::make_pair("PlayerIdle", mAnimationList["PlayerIdle"]));

	if (materialLoadThread.joinable()) {
		materialLoadThread.join();
	}



	//std::cout << "Material List Size: " << mMaterialList.size() << std::endl;
	//for (auto const& [key, val] : mMaterialList) {
	//	std::cout << "Material Loaded: " << key << std::endl;
	//}

	//std::cout << "Mesh List Size: " << mMeshList.size() << std::endl;
	//for (auto const& [key, val] : mMeshList) {
	//	std::cout << "Mesh Loaded: " << key << std::endl;
	//}



	for (auto const& [key, val] : mMaterialList) {
		if (key.substr(0, 6) == "Player") {
			std::cout << "Loading Player Mesh Materials" << std::endl;
			mMeshMaterialsList[key] = mRenderer->LoadMeshMaterial(*mMeshList["Player"], *val);
			std::cout << "Finished Loaded Player Mesh Materials" << std::endl;
		}
		else {
			std::cout << "Loading Other Mesh Materials" << std::endl;
			std::cout << "Processing Material for mesh: " << key << std::endl;
			mMeshMaterialsList[key] = mRenderer->LoadMeshMaterial(*mMeshList[key], *val);
		}
		lines++;
	}
	std::cout << "Loading Successful!" << std::endl;
	ready = true;
}

PlayerOne* GameLevelManager::AddPlayerToWorld(const Transform& transform, const std::string& playerName) {
	mPlayerToAdd = new PlayerOne(mWorld, playerName);
	AddComponentsToPlayer(*mPlayerToAdd, transform);

	mWorld->AddGameObject(mPlayerToAdd);
	mUpdatableObjectList.push_back(mPlayerToAdd);

	return mPlayerToAdd;
}

void GameLevelManager::AddComponentsToPlayer(Player& playerObject, const Transform& playerTransform) {

	SphereVolume* volume = new SphereVolume(PLAYER_MESH_SIZE/2);
	playerObject.SetBoundingVolume((CollisionVolume*)volume);

	playerObject.GetTransform()
		.SetScale(Vector3(PLAYER_MESH_SIZE, PLAYER_MESH_SIZE, PLAYER_MESH_SIZE))
		.SetPosition(playerTransform.GetPosition())
		.SetOrientation(playerTransform.GetOrientation());

	playerObject.SetRenderObject(new RenderObject(&playerObject.GetTransform(), mMeshList["Player"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));

	playerObject.SetPhysicsObject(new PhysicsObject(&playerObject.GetTransform(), playerObject.GetBoundingVolume()));

	playerObject.GetRenderObject()->SetMaterialTextures(mMeshMaterialsList["Player"]);

	playerObject.GetPhysicsObject()->SetInverseMass(PLAYER_INVERSE_MASS);
	playerObject.GetPhysicsObject()->InitSphereInertia();
}

// world gameobjects called in loadMap();
GameObject* GameLevelManager::AddWallToWorld(Vector3 dimensions, const Vector3& position, float x, float y, float z) {

	Vector3 offset = position + Vector3(0, 2.0f, 0);

	GameObject* wall = new GameObject("Wall");

	Quaternion newOrientation = Quaternion::EulerAnglesToQuaternion(x, y, z);
	wall->GetTransform().SetOrientation(newOrientation);

	AABBVolume* volume = new AABBVolume(dimensions * 0.5f);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform()
		.SetScale(dimensions)
		.SetPosition(offset);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), mMeshList["Cube"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(wall);
	
	return wall;
}

GameObject* GameLevelManager::AddBoxToWorld(const Vector3& position, Vector3 dimensions, const std::string name, float inverseMass) {
	Vector3 offset = position + Vector3(0, 2.0f, 0);
	
	GameObject* cube = new GameObject(name);

	AABBVolume* volume = new AABBVolume(dimensions * 0.5f);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetScale(dimensions)
		.SetPosition(offset);


	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), mMeshList["Cube"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(cube);

	return cube;
}


// map loading from json file
void GameLevelManager::loadMap() {
	int level;
	std::vector<InGameObject> objects;

	if (jsonParser::LoadLevel("../CSC8503/PrisonEscape/Levels/levelTest.json", level, objects)) {
		for (const auto& obj : objects) {
			if (obj.type == "Button") {

				Button* newButton = new Button();
				newButton->spawnButton(obj.dimensions, obj.position, obj.type, mMeshList["Cube"], mShaderList["BasicShader"], mTextureList["DefaultTexture"]);
				// code to check if box is box activated or something
				newButton->SetBoxActivated(true);
				buttons.push_back(newButton);
			}

			if (obj.type == "Box") {

				pushableBox = AddBoxToWorld(obj.position, obj.dimensions, obj.type);

			}

			if (obj.type == "Wall") {

				AddWallToWorld(obj.dimensions, obj.position, obj.orientation.x, obj.orientation.y, obj.orientation.z);
			}

			if (obj.type == "Exit") {
				std::cout << "ID: " << obj.id << " Type: " << obj.type << "\n";
				std::cout << "Position -> x: " << obj.position.x << " y: " << obj.position.y << " z: " << obj.position.z << "\n";
				std::cout << " Dimensions -> x: " << obj.dimensions.x << " y: " << obj.dimensions.y << " z: " << obj.dimensions.z << "\n";
				std::cout << "Orientation -> x: " << obj.orientation.x << " y: " << obj.orientation.y << " z: " << obj.orientation.z << "\n";
				std::cout << "Function to place " + obj.type + "\n\n";
			}

			if (obj.type == "Floor") {
				std::cout << "ID: " << obj.id << " Type: " << obj.type << "\n";
				std::cout << "Position -> x: " << obj.position.x << " y: " << obj.position.y << " z: " << obj.position.z << "\n";
				std::cout << " Dimensions -> x: " << obj.dimensions.x << " y: " << obj.dimensions.y << " z: " << obj.dimensions.z << "\n";
				std::cout << "Orientation -> x: " << obj.orientation.x << " y: " << obj.orientation.y << " z: " << obj.orientation.z << "\n";
				std::cout << "Function to place " + obj.type + "\n\n";

			}


		}
	}
	else {
		std::cerr << "Failed to load level \n";
	}
}