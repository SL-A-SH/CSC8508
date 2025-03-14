#pragma once

#include "PrisonEscape/Scripts/PatrolEnemy/PatrolEnemy.h"

namespace NCL {
	namespace CSC8503 {
		class PatrollingEnemy : public PatrolEnemy {
		public:
			PatrollingEnemy(GameWorld* world, const std::string& name);
			~PatrollingEnemy();
		};
	}
}