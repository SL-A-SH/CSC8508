#pragma once

#include "PhysicsSystem.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

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

		protected:
			Mesh* cubeMesh = nullptr;
			Texture* basicTex = nullptr;
			Shader* basicShader = nullptr;

			PlayerOne* playerOne = nullptr;

		private:
			void InitializeAssets();
			void InitializeLevel();
			void SetCameraAttributes();

			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& floorSize, const Vector4& color);


		public:
			PlayerOne* GetPlayerOne() const { return playerOne; }
		};
	}
}
