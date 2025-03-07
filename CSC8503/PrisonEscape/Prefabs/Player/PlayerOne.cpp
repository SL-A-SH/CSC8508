#include "PlayerOne.h"
#include "PhysicsObject.h"

#include "PrisonEscape/Core/GameBase.h"

using namespace NCL;
using namespace CSC8503;

PlayerOne::PlayerOne(GameWorld* world, const std::string& name) : Player(world, name)
{
    GameObject::SetName(name);
}

PlayerOne::~PlayerOne()
{

}

void PlayerOne::UpdateGame(float dt)
{
    UpdatePlayerMovement(dt);
}

void PlayerOne::UpdatePlayerMovement(float dt) {
    if (!renderObject) {
        return;
    }

    GameConfigManager* config = GameBase::GetGameBase()->GetGameConfig();
    bool isMultiplayer = config && config->networkConfig.isMultiplayer;
    bool isServer = config && config->networkConfig.isMultiplayer && config->networkConfig.isServer;

    // If we're the server or in single player, PlayerOne should be controlled locally
    if (isServer || !isMultiplayer) {
        Vector3 playerPosition = GetTransform().GetPosition();
        Camera& mainCamera = GameBase::GetGameBase()->GetWorld()->GetMainCamera();

        Vector3 cameraOffset(0, 60.0f, 50.0f);
        mainCamera.SetPosition(playerPosition + cameraOffset);

        float forward = controller.GetAxis(2);
        float sidestep = -controller.GetAxis(0);

        Vector3 currentVelocity = GetPhysicsObject()->GetLinearVelocity();

        Vector3 forwardVec(0, 0, 1);
        Vector3 rightVec(1, 0, 0);

        if (forward != 0.0f) 
        {
            forwardVec = -forwardVec;
        }
        if (sidestep != 0.0f) 
        {
            sidestep = -sidestep;
        }

        Vector3 movement(0, 0, 0);
        if (forward != 0.0f) 
        {
            movement += forwardVec * forward * GetPlayerSpeed();
        }
        if (sidestep != 0.0f) 
        {
            movement += rightVec * sidestep * GetPlayerSpeed();
        }

        if (useGravity) 
        {
            if (fabs(currentVelocity.y) < 1.0f) {
                static float lastJumpTime = -2.2f;
                float currentTime = Window::GetTimer().GetTotalTimeSeconds();

                if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE) && (currentTime - lastJumpTime >= 2.2f)) 
                {
                    movement.y += 10.0f;
                    currentVelocity.y = 35.0f;
                    lastJumpTime = currentTime;
                }
            }

            currentVelocity.y -= 25.0f * dt;
            GetPhysicsObject()->SetLinearVelocity(Vector3(movement.x, currentVelocity.y, movement.z));
        }
        else 
        {
            GetPhysicsObject()->SetLinearVelocity(movement);
        }
    }
}