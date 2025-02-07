#pragma once

#include "PhysicsSystem.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

namespace NCL {
	namespace CSC8503 {
		class Level
		{
		public:
			Level();
			~Level() {}
			virtual void Update(float dt) = 0;

		protected:
			Mesh* capsuleMesh = nullptr;
			Mesh* cubeMesh = nullptr;
			Mesh* sphereMesh = nullptr;
			Texture* basicTex = nullptr;
			Shader* basicShader = nullptr;
			Mesh* catMesh = nullptr;
			Mesh* kittenMesh = nullptr;
			Mesh* enemyMesh = nullptr;
			Mesh* bonusMesh = nullptr;
		public:

			virtual void Init();
			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);
		};
	}
}
