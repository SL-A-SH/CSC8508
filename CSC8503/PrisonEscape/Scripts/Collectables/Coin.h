#pragma once

#include "../CSC8503/PrisonEscape/Core/AudioManager.h"
#include "PhysicsSystem.h"
#include "../CSC8503/PrisonEscape/Scripts/Player/Player.h"

namespace NCL {
	namespace CSC8503 {
		class Coin : public GameObject {
		public:
			Coin(GameWorld* world, const std::string& name);
			~Coin();

			void OnCollisionBegin(GameObject* otherObject) override {
				if (collected == false) {
					if (otherObject == playerObject) {
						playerObject->SetScore(playerObject->GetScore() + 1);
						collected = true;
					}
				}
			}

		protected:
			Player* playerObject;
			bool collected;
		};
	}
}