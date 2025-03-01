#pragma once

#include "Vector.h"
#include "imgui/imgui.h"
#include "Level.h"
#include "../CSC8503/PrisonEscape/Core/GameBase.h"
#include "PhysicsSystem.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include <PrisonEscape/Scripts/puzzle/puzzleT.h>

using namespace NCL;
using namespace CSC8503;

class LevelT : public Level
{
public:
	LevelT();
	~LevelT();

	void Init() override;

	

	void Update(float dt) override;
	
private:
	Texture* doorCloseTex = nullptr;
	Texture* doorOpenTex = nullptr;


	//void InitializeAssets();

	void AddCubeSetToTheLevel();
	void CreateCollidingCube(Vector3 position);
	void UpdateCubeVisibility();
	GameObject* AddDoorToWorld(Door* door, const Vector3& position);

	GameObject* AddButtonToWorld(ButtonTrigger* button, const Vector3& position, Door* linkedDoor);

	GameObject* CreateRandomColorCube(Vector3 position);
	/*Vector3 visibilityThreshold = Vector3(4.0f, 4.0f, 4.0f);*/
	std::vector<GameObject*> Cubes;
	vector<Vector4> CubeSet;
};