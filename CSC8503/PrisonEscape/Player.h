#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"

namespace NCL {
	namespace CSC8503 {
		class Player {
		public:
			Player();
			~Player();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();
			void InitCamera();
			void InitWorld();
			void UpdatePlayerMovement(float dt);


			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& floorSize, const Vector4& color);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddPlayerToWorld(const Vector3& position);

			GameObject* player;
			GameObject* floor;

#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem* physics;
			GameWorld* world;

			KeyboardMouseController controller;

			bool useGravity;
			bool cameraAttached = false;

			float PlayerStrength;
			float PlayerSpeed;

			Vector3 lastCameraPosition;
			Vector3 lastCameraOrientation;

			Mesh* capsuleMesh = nullptr;
			Mesh* cubeMesh = nullptr;
			Mesh* sphereMesh = nullptr;
			Mesh* gooseMesh = nullptr;
			Mesh* catMesh = nullptr;
			Mesh* kittenMesh = nullptr;
			Mesh* enemyMesh = nullptr;
			Mesh* bonusMesh = nullptr;

			Shader* basicShader = nullptr;
			Texture* basicTex = nullptr;

		};
	}
}

