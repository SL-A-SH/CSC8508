#include "GameLevelManager.h"
#include "PrisonEscape/Levels/SampleLevel.h"
#include "Assets.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include <string>
#include <fstream>


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
			}

			else if (group == "shader") {
				std::cout << "Found Shader: " << groupInfo[1] << std::endl;
				for (int i = 0; i < groupInfo.size(); i +=3) {
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
			}
			group = assetInfo[0];
			groupInfo.clear();

		}

		for (int i = 1; i < 4; i++) {
			groupInfo.push_back(assetInfo[i]);
		}


	}


}

PlayerOne* GameLevelManager::AddPlayerToWorld(const Transform& transform, const std::string& playerName) {
	mPlayerToAdd = new PlayerOne(mWorld, playerName);
	AddComponentsToPlayer(*mPlayerToAdd, transform);

	mWorld->AddGameObject(mPlayerToAdd);
	mUpdatableObjectList.push_back(mPlayerToAdd);

	return mPlayerToAdd;
}

void GameLevelManager::AddComponentsToPlayer(Player& playerObject, const Transform& playerTransform) {
	SphereVolume* volume = new SphereVolume(1.0f);
	playerObject.SetBoundingVolume((CollisionVolume*)volume);

	playerObject.GetTransform()
		.SetScale(Vector3(PLAYER_MESH_SIZE, PLAYER_MESH_SIZE, PLAYER_MESH_SIZE))
		.SetPosition(playerTransform.GetPosition())
		.SetOrientation(playerTransform.GetOrientation());

	playerObject.SetRenderObject(new RenderObject(&playerObject.GetTransform(), mMeshList["PlayerMesh"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));

	playerObject.SetPhysicsObject(new PhysicsObject(&playerObject.GetTransform(), playerObject.GetBoundingVolume()));

	playerObject.GetPhysicsObject()->SetInverseMass(PLAYER_INVERSE_MASS);
	playerObject.GetPhysicsObject()->InitSphereInertia();
}