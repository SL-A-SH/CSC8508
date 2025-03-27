#pragma once

#include "../CSC8503/PrisonEscape/Core/AudioManager.h"
#include "PhysicsSystem.h"

namespace NCL {
	namespace CSC8503 {
		class Coin : public GameObject {
		public:
			Coin(GameWorld* world, const std::string& name);
			~Coin();
		};
	}
}