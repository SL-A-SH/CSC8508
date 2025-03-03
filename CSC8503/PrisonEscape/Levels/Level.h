#pragma once

#include "PhysicsSystem.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "imgui/imgui.h"
#include "NetworkBase.h"

#include "PrisonEscape/Prefabs/Player/PlayerOne.h"
#include "PrisonEscape/Prefabs/Player/PlayerTwo.h"

namespace NCL {
	namespace CSC8503 {
		class Level : public PacketReceiver
		{
		public:
			Level();
			~Level() {}
			virtual void Init();
			void AddPlayerOneToLevel(PlayerOne* player);
			void AddPlayerTwoToLevel();

			virtual void Update(float dt);

			void ReceivePacket(int type, GamePacket* packet, int source) override;

		protected:
			Mesh* cubeMesh = nullptr;
			Mesh* kittenMesh = nullptr;
			Texture* basicTex = nullptr;
			Texture* pauseButton = nullptr;
			Shader* basicShader = nullptr;

			PlayerOne* playerOne = nullptr;
			PlayerTwo* playerTwo = nullptr;

		private:
			void InitializeAssets();
			void InitializeLevel();
			void SetCameraAttributes();

			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& floorSize, const Vector4& color);
			GameObject* AddMeshToWorldPosition(const Vector3& position, Mesh* mesh, const Vector3& meshSize, VolumeType type, const Vector3& volumeSize, const float& inverseMass = 10.0f);

		public:
			PlayerOne* GetPlayerOne() const { return playerOne; }
			PlayerTwo* GetPlayerTwo() const { return playerTwo; }


#pragma region UI

		public:
			void DrawPauseButton();
			void DrawPauseMenu();

#pragma endregion

		};
	}
}
