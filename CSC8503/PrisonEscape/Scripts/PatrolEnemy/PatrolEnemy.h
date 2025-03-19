#pragma once
#include "GameObject.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "Ray.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "../Player/Player.h"

namespace NCL {
    namespace CSC8503 {
        class PatrolEnemy : public GameObject {
        public:
            PatrolEnemy(GameWorld* world, const std::string& name);
            ~PatrolEnemy();

            void Update(float dt);
            void SetPatrolPoints(const std::vector<Vector3>& points);
            void SetPlayerObject(Player* player);

            void OnCatch(Player* otherObject) {
                if (otherObject == playerObject) {
                    currentState = PATROL;
                    patrolCounter = 0;

                    playerObject->SetHealth(playerObject->GetHealth() - 1);
                }
            }

            bool visible;

        protected:
            void InitBehaviourTree();
            bool CanSeePlayer() const;

            BehaviourSequence* rootSequence;
            BehaviourSelector* modeSelector;

            GameWorld* gameWorld;
            Player* playerObject;
            std::vector<Vector3> patrolPoints;

            int currentPatrolPoint;
            int patrolCounter;
            float warningTimer;

            const float VISION_RANGE = 15.0f;

            enum AIState {
                PATROL,
                CAUGHT
            };
            AIState currentState;
        };
    }
}
