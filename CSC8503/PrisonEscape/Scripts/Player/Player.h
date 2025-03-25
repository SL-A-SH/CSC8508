#pragma once

#include "../NCLCoreClasses/KeyboardMouseController.h"
#include "PhysicsSystem.h"
#include "Window.h"
#include "Vector.h"
#define M_PI 3.14159265358979323846
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
			KeyboardMouseController* controller;

			bool useGravity;
			bool cameraAttached = false;

			Vector3 lastCameraPosition;
			Vector3 lastCameraOrientation;

			std::string mName;
			GameWorld* mWorld;

		private:
			float playerSpeed = 10.0f;
			int health;
			Vector3 playerSpawn;

		public:
			std::string GetName() { return mName; }

			float GetPlayerSpeed() const { return playerSpeed; }
			void SetPlayerSpeed(float speed) { playerSpeed = speed; }

			int GetHealth() const { return health; }
			void SetHealth(int h) { health = h; }

			Vector3 GetSpawn() const { return playerSpawn; }
			void SetSpawn(Vector3 s) { playerSpawn = s; }

			RayCollision closestCollision;

			Vector3 rayPos;
			Vector3 rayDir;



		};
	}
}

