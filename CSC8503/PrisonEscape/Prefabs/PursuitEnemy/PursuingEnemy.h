#pragma once

#include "PrisonEscape/Scripts/PursuitEnemy/PursuitEnemy.h"

namespace NCL {
	namespace CSC8503 {
		class PursuingEnemy : public PursuitEnemy {
		public:
			PursuingEnemy(GameWorld* world);
			~PursuingEnemy();
		};
	}
}