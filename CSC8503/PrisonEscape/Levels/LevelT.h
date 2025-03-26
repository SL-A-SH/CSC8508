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

namespace NCL {
	namespace CSC8503 {
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

			void AddCubeSetToTheLevel();
			void CreateCollidingCube(Vector3 position);
			void UpdateCubeVisibility();
			GameObject* AddDoorToWorld(Door* door, const Vector3& position);

			GameObject* AddPressableDoorToWorld(PressableDoor* door, const Vector3& position);
			void AddHidingAreaToWorld(const Vector3& position, const Vector3& size);
			GameObject* AddButtonToWorld(ButtonTrigger* button, const Vector3& position, Door* linkedDoor);

			GameObject* CreateRandomColorCube(Vector3 position);
			/*Vector3 visibilityThreshold = Vector3(4.0f, 4.0f, 4.0f);*/
			std::vector<GameObject*> Cubes;
			vector<Vector4> CubeSet;
		};
	}
}