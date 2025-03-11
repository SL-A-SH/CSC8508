#pragma once

#include "GameTechRenderer.h"
#include "PhysicsSystem.h"
#include "Window.h"
#include "Vector.h"
#include "Mesh.h"
#include "../CSC8503/PrisonEscape/Core/GameBase.h"

namespace NCL {
	namespace CSC8503 {
		class Button : public GameObject {
		public:
			Button();
			~Button();

			void spawnButton(Vector3 size, Vector3 position, std::string name, Mesh* mesh, Shader* shader, Texture* texture);
			void SetPlayerActivated(bool state) { playerActivated = state; }
			void SetBoxActivated(bool state) { boxActivated = state; }

			bool IsPlayerActivated() const { return playerActivated; }
			bool IsBoxActivated() const { return boxActivated; }
			bool IsPressed() const { return pressed; }

			void pressDetection(GameObject* otherObject, const std::string desiredName);

		private:
			bool pressed;
			bool playerActivated;
			bool boxActivated;

			GameObject* AddButtonToWorld(Vector3 size, const Vector3& position, const std::string name, Mesh* mesh, Shader* shader, Texture* texture);
			void InitializeAssets();

			// "animate" the button that gets pressed down (move it down lower so it seems like its been pressed)
			void pressDownButton();

			Mesh* buttonMesh = nullptr;

			Shader* basicShader = nullptr;

			GameObject* buttonObject = nullptr;

		};
	}
}