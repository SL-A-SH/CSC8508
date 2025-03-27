#include <string>
#include <fstream>
#include <iostream>

#include "GameLevelManager.h"
#include "GameTechRenderer.h"
#include "GameWorld.h"
#include "Assets.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "AnimationController.h"

#include "PrisonEscape/Scripts/puzzle/Button.h"
#include "PrisonEscape/Levels/Level.h"
#include "PrisonEscape/Scripts/Player/Player.h"
#include "PrisonEscape/Scripts/PatrolEnemy/PatrolEnemy.h"
#include "PrisonEscape/Scripts/puzzle/HidingArea.h"
#include "PrisonEscape/Scripts/PursuitEnemy/PursuitEnemy.h"
#include "../CSC8503/PrisonEscape/Scripts/CameraEnemy/CameraEnemy.h"
#include "../CSC8503/PrisonEscape/Scripts/puzzle/puzzleT.h"


using namespace NCL;
using namespace CSC8503;

GameLevelManager* GameLevelManager::manager = nullptr;

GameLevelManager::GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer, bool multiplayerStatus, bool isServer)
{
	mWorld = existingWorld;
	mRenderer = existingRenderer;
	mPhysics = new PhysicsSystem(*mWorld);
	mAnimator = new AnimationController(*mWorld, mPreLoadedAnimationList);
	mPhysics->UseGravity(true);
	manager = this;
	isMultiplayer = multiplayerStatus;
	this->isServer = isServer;

	InitAssets();
	std::cout << "The Level to load is at: " << mLevelList["Level1"] << std::endl;
	boxNumber = 0;
	loadMap(mLevelList["Level2"]);
	InitAnimationObjects();
}

GameLevelManager::~GameLevelManager()
{
	delete mPhysics;
	delete mAnimator;

	for (int i = 0; i < mUpdatableObjectList.size(); i++) {
		delete(mUpdatableObjectList[i]);
	}
	mUpdatableObjectList.clear();

	for (auto& mesh : mMeshList) {
		delete mesh.second;
	}
	mMeshList.clear();

	for (auto& texture : mTextureList) {
		delete texture.second;
	}
	mTextureList.clear();

	for (auto& shader : mShaderList) {
		delete shader.second;
	}
	mShaderList.clear();

	for (auto& material : mMaterialList) {
		delete material.second;
	}
	mMaterialList.clear();

	for (auto& anim : mAnimationList) {
		delete anim.second;
	}
	mAnimationList.clear();

	delete mRenderer;
	delete mWorld;
}

void GameLevelManager::UpdateGame(float dt)
{
	if (mUpdatableObjectList.size() >= 1) {
		mRenderer->SetGameReady(true);
	}

	// TODO: Level Updates
	// TODO: Remove Debug
	mPhysics->Update(dt);
	mAnimator->Update(dt, mUpdatableObjectList);
	GetCurrentLevel()->Update(dt);

	//mWorld->UpdateWorld(dt);

	if ((mUpdatableObjectList.size() > 0)) {
		for (GameObject* obj : mUpdatableObjectList) {
			obj->UpdateGame(dt);
		}
	}

	for (Button* button : buttons) {
		if (!button->IsPressed()) {
			button->pressDetection(boxes);
		}
	}
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

			else if (group == "level") {
				std::cout << "Found Level: " << groupInfo[1] << std::endl;
				for (int i = 0; i < groupInfo.size(); i += 3) {
					mLevelList[groupInfo[i]] = groupInfo[i + 1];
					lines++;
				}
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
	mPreLoadedAnimationList.insert(std::make_pair("PlayerWalk", mAnimationList["PlayerWalk"]));

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


void GameLevelManager::InitAnimationObjects() const {
	mAnimator->SetObjectList(mUpdatableObjectList);
}

Player* GameLevelManager::AddPlayerToWorld(const Transform& transform, const std::string& playerName)
{
	std::cout << "Adding Player To World" << std::endl;
	Player* mPlayerToAdd = new Player(mWorld, playerName);
	AddComponentsToPlayer(*mPlayerToAdd, transform);

	mWorld->AddGameObject(mPlayerToAdd);
	mUpdatableObjectList.push_back(mPlayerToAdd);
	std::cout << "mUpdatable Object List Size:    " << mUpdatableObjectList.size() << std::endl;
	mAnimator->SetObjectList(mUpdatableObjectList);

	return mPlayerToAdd;
}

void GameLevelManager::AddComponentsToPlayer(Player& playerObject, const Transform& playerTransform) {

	SphereVolume* volume = new SphereVolume(PLAYER_MESH_SIZE / 2);
	playerObject.SetBoundingVolume((CollisionVolume*)volume);

	playerObject.GetTransform()
		.SetScale(Vector3(PLAYER_MESH_SIZE, PLAYER_MESH_SIZE, PLAYER_MESH_SIZE))
		.SetPosition(playerTransform.GetPosition())
		.SetOrientation(playerTransform.GetOrientation());

	playerObject.SetRenderObject(new RenderObject(&playerObject.GetTransform(), mMeshList["Player"], mTextureList["DefaultTexture"], mShaderList["Animation"]));
	playerObject.GetRenderObject()->SetAnimObject(new AnimationObject(AnimationObject::AnimationType::player, mAnimationList["PlayerWalk"]));

	playerObject.SetPhysicsObject(new PhysicsObject(&playerObject.GetTransform(), playerObject.GetBoundingVolume()));

	playerObject.GetRenderObject()->SetMaterialTextures(mMeshMaterialsList["Player"]);

	playerObject.GetPhysicsObject()->SetInverseMass(PLAYER_INVERSE_MASS);
	playerObject.GetPhysicsObject()->InitSphereInertia();
}

//Should add enemy to the world, needs testing

PatrolEnemy* GameLevelManager::AddPatrolEnemyToWorld(const std::string& enemyName,const std::vector<Vector3>& patrolPoints, const Vector3& spawnPoint, Player* player) {
	Transform transform;
	transform.SetPosition(spawnPoint);
	PatrolEnemy* mEnemyToAdd = new PatrolEnemy(mWorld, enemyName);

	if (isMultiplayer && !this->isServer) {
		mEnemyToAdd->SetClientControlled(true);
	}
	
	AddComponentsToPatrolEnemy(*mEnemyToAdd, transform);

	mEnemyToAdd->SetPatrolPoints(patrolPoints);
	mEnemyToAdd->SetPlayerObject(player);

	mWorld->AddGameObject(mEnemyToAdd);
	mUpdatableObjectList.push_back(mEnemyToAdd);

	return mEnemyToAdd;
}

void GameLevelManager::AddComponentsToPatrolEnemy(PatrolEnemy& enemyObj, const Transform& enemyTransform) {
	SphereVolume* volume = new SphereVolume(PATROL_ENEMY_MESH_SIZE / 2);
	enemyObj.SetBoundingVolume((CollisionVolume*)volume);

	enemyObj.GetTransform()
		.SetScale(Vector3(PATROL_ENEMY_MESH_SIZE, PATROL_ENEMY_MESH_SIZE, PATROL_ENEMY_MESH_SIZE))
		.SetPosition(enemyTransform.GetPosition())
		.SetOrientation(enemyTransform.GetOrientation());

	enemyObj.SetRenderObject(new RenderObject(&enemyObj.GetTransform(), mMeshList["Guard"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));

	enemyObj.SetPhysicsObject(new PhysicsObject(&enemyObj.GetTransform(), enemyObj.GetBoundingVolume()));

	enemyObj.GetRenderObject()->SetMaterialTextures(mMeshMaterialsList["Guard"]);

	enemyObj.GetPhysicsObject()->SetInverseMass(PATROL_ENEMY_INVERSE_MASS);
	enemyObj.GetPhysicsObject()->InitSphereInertia();

}

PursuitEnemy* GameLevelManager::AddPursuitEnemyToWorld(const std::string& enemyName, const std::vector<Vector3>& pursuitPatrolPoints, Player* player) {
	Transform transform;
	PursuitEnemy* mEnemyToAdd = new PursuitEnemy(mWorld, enemyName);
	AddComponentsToPursuitEnemy(*mEnemyToAdd, transform);

	mEnemyToAdd->SetPatrolPoints(pursuitPatrolPoints);
	mEnemyToAdd->SetPlayerObject(player);

	mWorld->AddGameObject(mEnemyToAdd);
	mUpdatableObjectList.push_back(mEnemyToAdd);

	return mEnemyToAdd;
}

void GameLevelManager::AddComponentsToPursuitEnemy(PursuitEnemy& enemyObj, const Transform& enemyTransform) {
	SphereVolume* volume = new SphereVolume(PURSUIT_ENEMY_MESH_SIZE / 2);
	enemyObj.SetBoundingVolume((CollisionVolume*)volume);

	enemyObj.GetTransform()
		.SetScale(Vector3(PURSUIT_ENEMY_MESH_SIZE, PURSUIT_ENEMY_MESH_SIZE, PURSUIT_ENEMY_MESH_SIZE))
		.SetPosition(enemyTransform.GetPosition())
		.SetOrientation(enemyTransform.GetOrientation());

	enemyObj.SetRenderObject(new RenderObject(&enemyObj.GetTransform(), mMeshList["Guard"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));

	enemyObj.SetPhysicsObject(new PhysicsObject(&enemyObj.GetTransform(), enemyObj.GetBoundingVolume()));

	enemyObj.GetRenderObject()->SetMaterialTextures(mMeshMaterialsList["Guard"]);

	enemyObj.GetPhysicsObject()->SetInverseMass(PURSUIT_ENEMY_INVERSE_MASS);
	enemyObj.GetPhysicsObject()->InitSphereInertia();

}

CameraEnemy* GameLevelManager::AddCameraEnemyToWorld(const std::string& enemyName, const Vector3& spawnPoint, Player* player) {
	Transform transform;
	CameraEnemy* mEnemyToAdd = new CameraEnemy(mWorld, enemyName);
	AddComponentsToCameraEnemy(*mEnemyToAdd, transform);

	mEnemyToAdd->SetPlayerObject(player);

	mWorld->AddGameObject(mEnemyToAdd);
	mUpdatableObjectList.push_back(mEnemyToAdd);

	return mEnemyToAdd;
}

void GameLevelManager::AddComponentsToCameraEnemy(CameraEnemy& enemyObj, const Transform& enemyTransform) {
	SphereVolume* volume = new SphereVolume(CAMERA_ENEMY_MESH_SIZE / 2);
	enemyObj.SetBoundingVolume((CollisionVolume*)volume);

	enemyObj.GetTransform()
		.SetScale(Vector3(CAMERA_ENEMY_MESH_SIZE, CAMERA_ENEMY_MESH_SIZE, CAMERA_ENEMY_MESH_SIZE))
		.SetPosition(enemyTransform.GetPosition())
		.SetOrientation(enemyTransform.GetOrientation());

	enemyObj.SetRenderObject(new RenderObject(&enemyObj.GetTransform(), mMeshList["Guard"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));

	enemyObj.SetPhysicsObject(new PhysicsObject(&enemyObj.GetTransform(), enemyObj.GetBoundingVolume()));

	enemyObj.GetRenderObject()->SetMaterialTextures(mMeshMaterialsList["Guard"]);

	enemyObj.GetPhysicsObject()->SetInverseMass(CAMERA_ENEMY_INVERSE_MASS);
	enemyObj.GetPhysicsObject()->InitSphereInertia();

}

// world gameobjects called in loadMap();
GameObject* GameLevelManager::AddWallToWorld(Vector3 dimensions, const Vector3& position, float x, float y, float z) {

	GameObject* wall = new GameObject("Wall");

	Quaternion newOrientation = Quaternion::EulerAnglesToQuaternion(x, y, z);
	wall->GetTransform().SetOrientation(newOrientation);

	AABBVolume* volume = new AABBVolume(dimensions * 0.5f);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform()
		.SetScale(dimensions)
		.SetPosition(position);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), mMeshList["Cube"], mTextureList["WallTexture"], mShaderList["BasicShader"]));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(wall);

	return wall;
}

GameObject* GameLevelManager::AddFloorToWorld(Vector3 size, const Vector3& position) {

	GameObject* floor = new GameObject("Floor");

	AABBVolume* volume = new AABBVolume(size * 0.5f);

	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(size)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), mMeshList["Cube"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(floor);

	return floor;
}

GameObject* GameLevelManager::AddBoxToWorld(const Vector3& position, Vector3 dimensions, const std::string name, float inverseMass) {
	
	GameObject* cube = new GameObject(name);

	AABBVolume* volume = new AABBVolume(dimensions * 0.5f);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetScale(dimensions)
		.SetPosition(position);


	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), mMeshList["Cube"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(cube);

	return cube;
}

// note: 
// button1 = box activated only, 
// button2 = player activated only, (in progress)
// button3 = can be activated by both

GameObject* GameLevelManager::AddButtonToWorld(Vector3 size, const Vector3& position, const std::string name, Mesh* mesh, Shader* shader, Texture* texture) {

	GameObject* button = new GameObject(name);

	AABBVolume* volume = new AABBVolume(size * 0.5f);
	button->SetBoundingVolume((CollisionVolume*)volume);
	button->GetTransform()
		.SetScale(size)
		.SetPosition(position);

	RenderObject* renderObject = new RenderObject(&button->GetTransform(), mesh, texture, shader);

	if (name == "Button1") {
		renderObject->SetColour(Vector4(1, 0, 0, 1)); // red
	}
	else if (name == "Button2") {
		renderObject->SetColour(Vector4(0, 1, 0, 1)); // green
	}
	else if (name == "Button3") {
		renderObject->SetColour(Vector4(0, 0, 1, 1)); // blue
	}

	button->SetRenderObject(renderObject);

	button->SetPhysicsObject(new PhysicsObject(&button->GetTransform(), button->GetBoundingVolume()));

	button->GetPhysicsObject()->SetInverseMass(0);
	button->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(button);

	return button;
}

GameObject* GameLevelManager::AddDoorToWorld(Door* door, Vector3 size, const Vector3& position, float x, float y, float z) {
	AABBVolume* volume = new AABBVolume(size);

	Quaternion newOrientation = Quaternion::EulerAnglesToQuaternion(x, y, z);
	door->GetTransform().SetOrientation(newOrientation);

	door->SetBoundingVolume((CollisionVolume*)volume);

	door->GetTransform().SetScale(size).SetPosition(position);
	door->SetRenderObject(new RenderObject(&door->GetTransform(), mMeshList["Cube"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));
	door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume()));
	door->GetPhysicsObject()->SetLayer(LayerMask::Doors);
	door->GetPhysicsObject()->SetInverseMass(0);
	door->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(door);
	return door;
}

GameObject* GameLevelManager::AddExitToWorld(Exit* exit, Vector3 size, const Vector3& position) {
	AABBVolume* volume = new AABBVolume(size);

	exit->SetBoundingVolume((CollisionVolume*)volume);

	exit->GetTransform().SetScale(size).SetPosition(position);

	exit->SetRenderObject(new RenderObject(&exit->GetTransform(), mMeshList["Cube"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));
	exit->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));  // Red when inactive

	exit->SetPhysicsObject(new PhysicsObject(&exit->GetTransform(), exit->GetBoundingVolume()));
	exit->GetPhysicsObject()->SetInverseMass(0); // Static object
	exit->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(exit);
	return exit;
}
GameObject* GameLevelManager::AddButtonnToWorld(ButtonTrigger* button, const Vector3& position, Door* linkedDoor) {
	Vector3 buttonSize(2.0f, 0.3f, 2.0f);
	AABBVolume* volume = new AABBVolume(buttonSize);
	button->SetBoundingVolume((CollisionVolume*)volume);

	button->GetTransform().SetScale(buttonSize).SetPosition(position);
	button->SetRenderObject(new RenderObject(&button->GetTransform(), mMeshList["Cube"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));
	button->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));  // Red when inactive

	button->SetPhysicsObject(new PhysicsObject(&button->GetTransform(), button->GetBoundingVolume()));
	button->GetPhysicsObject()->SetInverseMass(0);
	button->GetPhysicsObject()->InitCubeInertia();

	button->SetLinkedDoor(linkedDoor);
	GameBase::GetGameBase()->GetWorld()->AddGameObject(button);
	return button;
}

GameObject* GameLevelManager::AddPressableDoorToWorld(PressableDoor* door, Vector3 size, const Vector3& position, float x, float y, float z) {
	AABBVolume* volume = new AABBVolume(size);

	Quaternion newOrientation = Quaternion::EulerAnglesToQuaternion(x, y, z);
	door->GetTransform().SetOrientation(newOrientation);

	door->SetBoundingVolume((CollisionVolume*)volume);

	door->GetTransform().SetScale(size).SetPosition(position);
	door->SetRenderObject(new RenderObject(&door->GetTransform(), mMeshList["Cube"], mTextureList["DefaultTexture"], mShaderList["BasicShader"]));
	door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume()));
	door->GetPhysicsObject()->SetLayer(LayerMask::Doors);
	door->GetPhysicsObject()->SetInverseMass(0);
	door->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(door);
	return door;
}

//void GameLevelManager::CreateButton(const InGameObject& obj) {
//	Button* newButton = new Button();
//
//	newButton->setButtonObject(
//		AddButtonToWorld(obj.dimensions, obj.position, obj.type,
//			mMeshList["Cube"], mShaderList["BasicShader"],
//			mTextureList["DefaultTexture"])
//	);
//
//	if (obj.type == "Button1") {
//		newButton->SetBoxActivated(true);
//	}
//	else if (obj.type == "Button2") {
//		newButton->SetPlayerActivated(true);
//	}
//	else if (obj.type == "Button3") {
//		newButton->SetBoxActivated(true);
//		newButton->SetPlayerActivated(true);
//	}
//
//	buttons.push_back(newButton);
//}

void GameLevelManager::CreateDoorButton(const InGameObject& obj, std::unordered_map<std::string, Door*>& doorMap) {
	ButtonTrigger* newButton = new ButtonTrigger();

	std::string linkedDoorName = "ButtonDoor" + obj.type.substr(6); 
	std::cout << "Looking for door: " + linkedDoorName + "\n";
	auto doorCheck = doorMap.find(linkedDoorName);
	if (doorCheck == doorMap.end()) {
		std::cerr << "Warning: Button " << obj.type << " has no matching door!\n";
	}
	else {
		Door* linkedDoor = doorCheck->second;
		std::cout << "Button " << obj.type << " linked to door " << linkedDoorName << "\n";
		AddButtonnToWorld(newButton, obj.position, linkedDoor);
	}
}

void GameLevelManager::CreateBox(const InGameObject& obj) {
	GameObject* newBox = AddBoxToWorld(obj.position, obj.dimensions, obj.type + std::to_string(++boxNumber));
	boxes.push_back(newBox);
}

void GameLevelManager::CreateWall(const InGameObject& obj) {
	AddWallToWorld(obj.dimensions, obj.position, obj.orientation.x, obj.orientation.y, obj.orientation.z);
}

void GameLevelManager::CreateFloor(const InGameObject& obj) {
	AddFloorToWorld(obj.dimensions, obj.position);
}

void GameLevelManager::CreateExit(const InGameObject& obj) {
	Exit* newExit = new Exit();
	
	AddExitToWorld(newExit, obj.dimensions, obj.position);
	
}
void GameLevelManager::CreateNormalDoor(const InGameObject& obj) {
	PressableDoor* newDoor = new PressableDoor();
	newDoor->SetTextures(mTextureList["DefaultTexture"], mTextureList["DefaultTexture"]);
	AddPressableDoorToWorld(newDoor, obj.dimensions, obj.position, obj.orientation.x, obj.orientation.y, obj.orientation.z);
	LogObjectPlacement(obj);
}

Door* GameLevelManager::CreateButtonDoor(const InGameObject& obj) {
	Door* newDoor = new Door();
	newDoor->SetTextures(mTextureList["DefaultTexture"], mTextureList["DefaultTexture"]);
	AddDoorToWorld(newDoor, obj.dimensions, obj.position, obj.orientation.x, obj.orientation.y, obj.orientation.z);
	return newDoor;
}

// logging object placement for debugging
void GameLevelManager::LogObjectPlacement(const InGameObject& obj) {
	std::cout << "ID: " << obj.id << " | Type: " << obj.type << "\n"
		<< "Position -> x: " << obj.position.x << " y: " << obj.position.y << " z: " << obj.position.z << "\n"
		<< "Dimensions -> x: " << obj.dimensions.x << " y: " << obj.dimensions.y << " z: " << obj.dimensions.z << "\n"
		<< "Orientation -> x: " << obj.orientation.x << " y: " << obj.orientation.y << " z: " << obj.orientation.z << "\n\n";
}

// map loading from json file
void GameLevelManager::loadMap(std::string levelToLoad) {
	
	int level;
	std::vector<InGameObject> objects;
	std::vector<Enemy> enemies;
	std::unordered_map<std::string, Door*> doorMap; // storing doors by name
	std::string levelPath = Assets::LEVELDIR + levelToLoad;

	if (::jsonParser::LoadLevel(levelPath, level, objects, enemies)) {

		// create button doors first and store
		for (const auto& obj : objects) {
			if (obj.type.find("ButtonDoor") != std::string::npos) {
				Door* newDoor = CreateButtonDoor(obj);
				doorMap[obj.type] = newDoor;
				LogObjectPlacement(obj);
			}
		}

		//std::cout << "Door Map Contents After Creation:\n";
		//for (const auto& pair : doorMap) {
		//	std::cout << "  " << pair.first << " -> " << pair.second << "\n";
		//}

		for (const auto& obj : objects) {
			if (obj.type.find("Button") != std::string::npos && obj.type.find("ButtonDoor") == std::string::npos) {
				CreateDoorButton(obj, doorMap);
				LogObjectPlacement(obj);
			}

			else if (obj.type == "NormalDoor") {
				CreateNormalDoor(obj);
			}

			else if (obj.type == "Box") {
				CreateBox(obj);
			}

			else if (obj.type.find("Wall") != std::string::npos) {
				CreateWall(obj);
			}

			else if (obj.type == "Floor") {
				CreateFloor(obj);
			}

			else if (obj.type == "Exit") {
				CreateExit(obj);
			}

			else if (obj.type == "Soap") {
				continue;
			}

			else if (obj.type.find("Player") != std::string::npos) {
				if (!playerOne && !playerTwo) {
					if (obj.type == "Player1") {
						Transform playerOneTransform;
						playerOne = AddPlayerToWorld(playerOneTransform.SetPosition(obj.position), "playerOne");
					}
					else if (obj.type == "Player2" && isMultiplayer) {
						Transform playerTwoTransform;
						playerTwo = AddPlayerToWorld(playerTwoTransform.SetPosition(obj.position), "playerTwo");
					}
				} 
				else {
					if (obj.type == "Player1") {
						playerOne->GetTransform().SetPosition(obj.position);
					}
					else if (obj.type == "Player2" && isMultiplayer) {
						playerTwo->GetTransform().SetPosition(obj.position);
					}
				}	
			}

			else {
				LogObjectPlacement(obj);
			}
		}

		for (const auto& enemy : enemies) {
			AddPatrolEnemyToWorld(enemy.name, enemy.waypoints, enemy.position, playerOne);
		}
	}
	else {
		std::cerr << "Can't load level \n";
	}
}
void GameLevelManager::ClearLevel() {
	// Output for debugging
	std::cout << "Clearing the level..." << std::endl;

	// Remove all updatable objects from the world
	for (auto& obj : GameBase::GetGameBase()->GetWorld()->getGameObjects()) {
		if (obj->GetName() == "playerOne" || obj->GetName() == "playerTwo") {
			continue;
		} else { 
			GameBase::GetGameBase()->GetWorld()->RemoveGameObject(obj); 
		}
	}

	// GameBase::GetGameBase()->GetWorld()->Clear();


	//// Clear the updatable object list
	//mUpdatableObjectList.clear();

	//// Remove all other game objects and resources related to the level
	//// Clear specific lists
	//buttons.clear();
	//buttonDoors.clear();
	//pressDoors.clear();
	//boxes.clear();
	//pressDoors.clear();  // Duplicate clear, you can remove if unnecessary
	//buttonss.clear();


	//// Optionally, clear resources that might be reused or need reinitialization
	//// Clear textures, meshes, or shaders if necessary for a fresh load
	//// For example:
	//// mMeshList.clear();
	//// mTextureList.clear();
	//// mShaderList.clear();

	//// Clear other global game state if needed (camera, environment, etc.)
	//// Example:
	//// mCamera.Clear();  // If you have a camera object, clear its state
	//// mEnvironment.Clear();  // If you have environmental objects, clear them

	std::cout << "Level cleared. Ready for next level." << std::endl;
}