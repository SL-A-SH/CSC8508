#pragma once
#include "GameObject.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "Ray.h"
#include "GameWorld.h"
#include "PhysicsObject.h"

namespace NCL {
    namespace CSC8503 {
        class PatrolEnemy : public GameObject {
        public:
            PatrolEnemy(GameWorld* world);
            ~PatrolEnemy();

            void Update(float dt);
            void SetPatrolPoints(const std::vector<Vector3>& points);
            void SetPlayerObject(GameObject* player);

            void OnCatch(GameObject* otherObject) {
                if (otherObject == playerObject) {
                    currentState = PATROL;
                    patrolCounter = 0;

                    playerObject->GetTransform().SetPosition(Vector3(0, 5, 0));
                }
            }

            bool visible;

        protected:
            void InitBehaviourTree();
            bool CanSeePlayer() const;

            BehaviourSequence* rootSequence;
            BehaviourSelector* modeSelector;

            GameWorld* gameWorld;
            GameObject* playerObject;
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
