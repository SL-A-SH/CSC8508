#include "PatrolEnemy.h"

using namespace NCL;
using namespace CSC8503;

PatrolEnemy::PatrolEnemy(GameWorld* world, const std::string& name) : GameObject() {
    gameWorld = world;
    GameObject::SetName(name);
    currentPatrolPoint = 0;
    patrolCounter = 0;
    currentState = PATROL;
    playerObject = nullptr;
    visible = true;
    warningTimer = 2.0f;
    InitBehaviourTree();
}

PatrolEnemy::~PatrolEnemy() {
    delete rootSequence;
}

void PatrolEnemy::Update(float dt) {

    std::string stateStr;
    switch (currentState) {
    case PATROL:  stateStr = "PATROL"; break;
    case CAUGHT: stateStr = "CAUGHT"; break;
    }

    rootSequence->Execute(dt);
}

void PatrolEnemy::SetPatrolPoints(const std::vector<Vector3>& points) {
    patrolPoints = points;
}

void PatrolEnemy::SetPlayerObject(GameObject* player) {
    playerObject = player;
}

bool PatrolEnemy::CanSeePlayer() const {
    if (!playerObject) return false;

    if (!visible) return false;

    Vector3 direction = playerObject->GetTransform().GetPosition() - transform.GetPosition();

    if (Vector::Length(direction) > VISION_RANGE) return false;

    Ray ray(transform.GetPosition(), Vector::Normalise(direction));
    RayCollision closestCollision;

    if (gameWorld->Raycast(ray, closestCollision, true, (GameObject*)this)) {
        if (closestCollision.node == playerObject) {
            return true;
        }
    }

    return false;
}

void PatrolEnemy::InitBehaviourTree() {
    BehaviourAction* patrolAction = new BehaviourAction("Patrol",
        [&](float dt, BehaviourState state) -> BehaviourState {
            if (currentState != PATROL) {
                return Failure;
            }

            if (CanSeePlayer()) {
                currentState = CAUGHT;
                return Success;
            }

            Vector3 targetPoint = patrolPoints[currentPatrolPoint];
            Vector3 direction = targetPoint - transform.GetPosition();
            float distance = Vector::Length(direction);

            if (distance < 1.0f) {
                currentPatrolPoint = (currentPatrolPoint + 1) % patrolPoints.size();
                patrolCounter++;
            }

            Vector3 force = Vector::Normalise(direction) * 100.0f;
            GetPhysicsObject()->AddForce(force);

            Vector3 currentVel = GetPhysicsObject()->GetLinearVelocity();
            GetPhysicsObject()->AddForce(-currentVel * 10.0f);

            return Ongoing;
        });

    BehaviourAction* pursuitAction = new BehaviourAction("Caught",
        [&](float dt, BehaviourState state) -> BehaviourState {
            if (currentState != CAUGHT) {
                return Failure;
            }

            if (warningTimer > 0.0f) {
				Debug::Print("Warning: " + std::to_string(warningTimer), Vector2(10, 10));
                warningTimer -= dt;
            }

            else {
                warningTimer = 2.0f;
				OnCatch(playerObject);
				currentState = PATROL;
                return Success;
            }
            return Ongoing;
        });

    modeSelector = new BehaviourSelector("Mode Selector");
    modeSelector->AddChild(pursuitAction);
    modeSelector->AddChild(patrolAction);

    rootSequence = new BehaviourSequence("Root");
    rootSequence->AddChild(modeSelector);
}