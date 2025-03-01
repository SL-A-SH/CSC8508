#pragma once

#include "../NCLCoreClasses/KeyboardMouseController.h"
#include "GameTechRenderer.h"
#include "PhysicsSystem.h"
#include "Window.h"
#include "Vector.h"
#include "MeshMaterial.h"

namespace NCL {
	namespace CSC8503 {
		class Player : public GameObject {
		public:
			Player();
			~Player();

			void SpawnPlayer(Vector3 position);
			virtual void UpdateGame(float dt);
			virtual void UpdatePlayerMovement(float dt);
			virtual float GetPlayerSpeed() const { return 10.0f; }  // Default speed

		protected:
			GameObject* _mPlayer;

		private:
			void InitializeAssets();
			GameObject* AddPlayerToWorld(const Vector3& position);

			KeyboardMouseController controller;

			bool useGravity;
			bool cameraAttached = false;

			Vector3 lastCameraPosition;
			Vector3 lastCameraOrientation;

			Mesh* mesh = nullptr;

			MeshMaterial* material = nullptr;

			Shader* basicShader = nullptr;
		};
	}
}

