#pragma once

#include "../NCLCoreClasses/KeyboardMouseController.h"
#include "GameTechRenderer.h"
#include "PhysicsSystem.h"
#include "Window.h"
#include "Vector.h"

namespace NCL {
	namespace CSC8503 {
		class Player : public GameObject {
		public:
			Player(GameWorld* world, const std::string& name);
			~Player();

			void UpdateGame(float dt);
			void UpdatePlayerMovement(float dt);

			void InitializeController();

		protected:
			GameObject* playerObject;

			KeyboardMouseController* controller;

			bool useGravity;
			bool cameraAttached = false;

			Vector3 lastCameraPosition;
			Vector3 lastCameraOrientation;

			std::string mName;
			GameWorld* mWorld;

		private:
			float playerSpeed = 10.0f;

		public:
			std::string GetName() { return mName; }

			float GetPlayerSpeed() const { return playerSpeed; }
			void SetPlayerSpeed(float speed) { playerSpeed = speed; }

			GameObject* GetPlayerObject() const { return playerObject; }
		};
	}
}

