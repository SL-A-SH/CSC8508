#pragma once

#include "PhysicsSystem.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "imgui/imgui.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerOne;

		class Level
		{
		public:
			Level();
			~Level() {}
			virtual void Init();

			virtual void Update(float dt);

		private:
			Mesh* cubeMesh = nullptr;
			Mesh* kittenMesh = nullptr;
			Texture* basicTex = nullptr;
			Texture* pauseButton = nullptr;
			Shader* basicShader = nullptr;

			PlayerOne* playerOne = nullptr;

		private:
			void InitializeAssets();
			void InitializeLevel();
			void SetCameraAttributes();

			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& floorSize, const Vector4& color);
			GameObject* AddMeshToWorldPosition(const Vector3& position, Mesh* mesh, const Vector3& meshSize, VolumeType type, const Vector3& volumeSize, const float& inverseMass = 10.0f);

		public:
			PlayerOne* GetPlayerOne() const { return playerOne; }


#pragma region UI

		public:
			void DrawPauseButton();
			void DrawPauseMenu();

#pragma endregion

		};
	}
}
