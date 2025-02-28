#include "GameLevelManager.h"
#include "PrisonEscape/Levels/SampleLevel.h"
#include <fstream>
#include "Assets.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "AnimatedObject.h"
using namespace NCL;
using namespace CSC8503;

GameLevelManager::GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer)
{
	world = existingWorld;
	renderer = existingRenderer;
	physics = new PhysicsSystem(*world);
	physics->UseGravity(true);
	// Move to another place if needed



}

GameLevelManager::~GameLevelManager()
{

	delete physics;
}

void GameLevelManager::UpdateGame(float dt)
{
	// TODO: Level Updates
	// TODO: Remove Debug
	physics->Update(dt);

	GetCurrentLevel()->Update(dt);
	Debug::Print("LEVELS", Vector2(25, 30), Debug::WHITE);
}

void GameLevelManager::InitAssets() {
	std::ifstream assetFile(Assets::ASSETROOT + "AssetsToLoad.csv");
	std::string line;
	std::string* assetInfo = new std::string[4];
	vector<std::string> groupInfo;
	std::string groupType = "";
	std::thread animationLoadThread;
	std::thread materialLoadThread;

	int fileSize = 0;
	while (getline(assetFile, line)) {
		fileSize++;
		if (line.substr(0, line.find(",")) == "mat") fileSize++;
	}
	assetFile.clear();
	assetFile.seekg(std::ifstream::beg);
	bool updateScreen = true;
	bool meshesLoaded = false;
	int meshCount = 0;
	bool ready = false;
	int lines = 0;
	int animationLines = 0;
	int materialLines = 0;

	std::thread renderFlip([&updateScreen, &ready, &meshesLoaded, &meshCount, &lines] {
		int added = 0;
		while (!ready) {
			updateScreen = true;
			//std::this_thread::sleep_for(16.7ms);
			if (meshesLoaded && added != meshCount) {
				added++;
				lines++;
			}
		}
		});
	while (getline(assetFile, line)) {
		//TODO Add load progression meter
		for (int i = 0; i < 3; i++) {
			assetInfo[i] = line.substr(0, line.find(","));
			line.erase(0, assetInfo[i].length() + 1);
		}
		assetInfo[3] = line;
		if (groupType == "") groupType = assetInfo[0];
		if (groupType != assetInfo[0]) {
			if (groupType == "anim") {
				animationLoadThread = std::thread([this, groupInfo, &animationLines] {
					for (int i = 0; i < groupInfo.size(); i += 3) {
						mAnimations[groupInfo[i]] = renderer->LoadAnimation(groupInfo[i + 1]);
						animationLines++;
					}
					});
			}
			else if (groupType == "mat") {
				materialLoadThread = std::thread([this, groupInfo, &materialLines] {
					for (int i = 0; i < groupInfo.size(); i += 3) {
						mMaterials[groupInfo[i]] = renderer->LoadMaterial(groupInfo[i + 1]);
						materialLines++;
					}
					});
			}
			else if (groupType == "mesh") {
				renderer->LoadMeshes(mMeshes, groupInfo);
				meshCount = groupInfo.size() / 3;
				meshesLoaded = true;
			}
			else if (groupType == "tex") {
				for (int i = 0; i < groupInfo.size(); i += 3) {
					//TO DO Progression meter
					mTextures[groupInfo[i]] = renderer->LoadTexture(groupInfo[i + 1]);
					lines++;
				}
			}
			else if (groupType == "shader") {
				for (int i = 0; i < groupInfo.size(); i += 3) {
					//Load screen
					mShaders[groupInfo[i]] = renderer->LoadShader(groupInfo[i + 1], groupInfo[i + 2]);
					lines;
				}
			}
			groupType = assetInfo[0];
			groupInfo.clear();
		}
		for (int i = 1; i < 4; i++) {
			groupInfo.push_back(assetInfo[i]);
		}
	}
	delete[] assetInfo;

	animationLoadThread.join();

	preloadedAnimations.insert(std::make_pair("PlayerIdle", mAnimations["PlayerIdle"]));
	preloadedAnimations.insert(std::make_pair("PlayerWalk", mAnimations["PlayerWalk"]));
	preloadedAnimations.insert(std::make_pair("PlayerSprint", mAnimations["PlayerSprint"]));

	materialLoadThread.join();
	for (auto const& [key, val] : mMaterials) {
		// Load screen
		if (key.substr(0, 6) == "Player") {
			mMeshMaterials[key] = renderer->LoadMeshMaterial(*mMeshes["Player"], *val);
		}
		else {
			mMeshMaterials[key] = renderer->LoadMeshMaterial(*mMeshes[key], *val);
		}
		lines++;
	}
	ready = true;
	renderFlip.join();
	//completed screen
}

Player* GameLevelManager::AddPlayerToWorld(const Transform& transform, const std::string& name) {
	mTempPlayer = new Player();
	AssignPlayerObjects(*mTempPlayer, transform);
	return mTempPlayer;
}

void GameLevelManager::AssignPlayerObjects(Player& player, const Transform& playerTransform) {
	CapsuleVolume* volume = new CapsuleVolume(1.0f, 1.0f);
	player.SetBoundingVolume(volume);

	player.GetTransform()
		.SetScale(Vector3(3.0f, 3.0f, 3.0f))
		.SetPosition(playerTransform.GetPosition())
		.SetOrientation(playerTransform.GetOrientation());

	player.SetRenderObject(new RenderObject(&player.GetTransform(), mMeshes["Player"], mTextures["FleshyAlbedo"], mTextures["Normal"], mShaders["Animation"]));
	player.SetPhysicsObject(new PhysicsObject(&player.GetTransform(), player.GetBoundingVolume()));

	player.GetRenderObject()->SetAnimatedObject(new AnimatedObject(AnimatedObject::AnimationType::playerAnimation, mAnimations["playerIdle"]));
	player.GetRenderObject()->SetMatTextures(mMeshMaterials["Player"]);
	
	player.GetPhysicsObject()->SetInverseMass(0.5f);
	player.GetPhysicsObject()->InitSphereInertia();

}