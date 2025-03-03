#pragma once

#include "Vector.h"

#include "PrisonEscape/Scripts/Player/Player.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerOne : public Player {
		public:
			PlayerOne(GameWorld* world, const std::string& name);
			~PlayerOne();

			void UpdateGame(float dt) override;

			void UpdatePlayerMovement(float dt) override;

			GameWorld* mWorld;
			std::string mName;

		private:
			float playerSpeed = 10.0f;

		public:
			float GetPlayerSpeed() const { return playerSpeed; }
		};
	}
}