#pragma once

#include "Vector.h"

#include "PrisonEscape/Scripts/Player/Player.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerOne : public Player {
		public:
			PlayerOne();
			~PlayerOne();

			void UpdatePlayerMovement(float dt) override;

		private:
			float playerSpeed = 100.0f;

		public:
			float GetPlayerSpeed() const { return playerSpeed; }
		};
	}
}