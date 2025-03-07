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

			virtual void UpdateGame(float dt);
			virtual void UpdatePlayerMovement(float dt);
			virtual void HandleJumpingAndGravity(float dt, Vector3& currentVelocity, Vector3& movement);
			virtual float GetPlayerSpeed() const { return 10.0f; }  // Default speed


			void DetectPlayerCollision();
			void DisplayInteractionPrompt();

			virtual std::string GetName() { return mName; }


		protected:
			GameObject* playerObject;

			KeyboardMouseController controller;

			bool useGravity;
			bool cameraAttached = false;

			Vector3 lastCameraPosition;
			Vector3 lastCameraOrientation;

			std::string mName;
			GameWorld* mWorld;

		public:
			GameObject* GetPlayerObject() const { return playerObject; }


			RayCollision closestCollision;

			Vector3 rayPos;
			Vector3 rayDir;



		};
	}
}

