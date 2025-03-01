#pragma once

#include "GameObject.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {

		class NormalCollidCube : public GameObject {
		public:
			NormalCollidCube(const std::string& name = "");
			~NormalCollidCube();
			void OnCollisionBegin(GameObject* otherObject) override;
		};
		
		class Door : public GameObject {
		public:
			Door();
			~Door();

			void Update(float dt) ;
			void Open();
			void Close();
			bool IsOpen() const { return isOpen; }
			void SetTextures(Texture* closedTex, Texture* openTex);
			
		private:
			bool isOpen;
			Texture* openTexture;
			Texture* closeTexture;
		};

		class ButtonTrigger : public GameObject {
		public:
			ButtonTrigger(const std::string& name = "ButtonTrigger");
			~ButtonTrigger() = default;

			void OnCollisionBegin(GameObject* otherObject) override;
			void SetLinkedDoor(Door* door);

		private:
			bool isPressed;
			Door* linkedDoor;
		};
	}
}