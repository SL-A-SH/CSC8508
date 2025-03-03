#include "PlayerTwo.h"
#include "PhysicsObject.h"

#include "PrisonEscape/Core/GameBase.h"

using namespace NCL;
using namespace CSC8503;

PlayerTwo::PlayerTwo(GameWorld* world, const std::string& name) : Player(world, name)
{
    mWorld = world;
    mName = name;
}

PlayerTwo::~PlayerTwo()
{

}

void PlayerTwo::UpdatePlayerMovement(float dt) {
    if (!playerObject) {
        return;
    }

    GameConfigManager* config = GameBase::GetGameBase()->GetGameConfig();
    bool isClient = config && config->networkConfig.isMultiplayer && !config->networkConfig.isServer;

    // If we're the client, PlayerTwo should be controlled locally
    if (isClient) {
        Vector3 playerPosition = playerObject->GetTransform().GetPosition();
        Camera& mainCamera = GameBase::GetGameBase()->GetWorld()->GetMainCamera();

        Vector3 cameraOffset(0, 60.0f, 50.0f);
        mainCamera.SetPosition(playerPosition + cameraOffset);

        float forward = controller.GetAxis(2);
        float sidestep = -controller.GetAxis(0);

        Vector3 currentVelocity = playerObject->GetPhysicsObject()->GetLinearVelocity();

        Vector3 forwardVec(0, 0, 1);
        Vector3 rightVec(1, 0, 0);

        if (forward != 0.0f) {
            forwardVec = -forwardVec;
        }
        if (sidestep != 0.0f) {
            sidestep = -sidestep;
        }

        Vector3 movement(0, 0, 0);
        if (forward != 0.0f) {
            movement += forwardVec * forward * GetPlayerSpeed();
        }
        if (sidestep != 0.0f) {
            movement += rightVec * sidestep * GetPlayerSpeed();
        }

        if (useGravity) {
            if (fabs(currentVelocity.y) < 1.0f) {
                static float lastJumpTime = -2.2f;
                float currentTime = Window::GetTimer().GetTotalTimeSeconds();

                if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE) && (currentTime - lastJumpTime >= 2.2f)) {
                    movement.y += 10.0f;
                    currentVelocity.y = 35.0f;
                    lastJumpTime = currentTime;
                }
            }

            currentVelocity.y -= 25.0f * dt;
            playerObject->GetPhysicsObject()->SetLinearVelocity(Vector3(movement.x, currentVelocity.y, movement.z));
        }
        else {
            playerObject->GetPhysicsObject()->SetLinearVelocity(movement);
        }
    }
}