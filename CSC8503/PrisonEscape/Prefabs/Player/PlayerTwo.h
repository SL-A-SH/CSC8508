#pragma once

#include "Vector.h"

#include "PrisonEscape/Scripts/Player/Player.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerTwo : public Player {
		public:
			PlayerTwo(GameWorld* world, const std::string& name);
			~PlayerTwo();

			void UpdatePlayerMovement(float dt) override;

		private:
			float playerSpeed = 100.0f;

		public:
			float GetPlayerSpeed() const { return playerSpeed; }
		};
	}
}