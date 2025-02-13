#pragma once

#include "Vector.h"

#include "PrisonEscape/Scripts/Player/Player.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerOne : public Player {
		public:
			PlayerOne();
			~PlayerOne();

		private:
			float playerSpeed = 10.0f;

		public:
			float GetPlayerSpeed() const { return playerSpeed; }
		};
	}
}