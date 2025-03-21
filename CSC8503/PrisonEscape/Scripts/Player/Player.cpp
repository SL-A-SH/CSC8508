#include "Player.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

#include "PrisonEscape/Core/GameBase.h"
#include "PrisonEscape/Core/GameConfigManager.h"
#include "PrisonEscape/Core/ImGuiManager.h"

Player::Player(GameWorld* world, const std::string& name) : GameObject()
{
	useGravity = true;
	cameraAttached = true;

    state = Default;
	GameObject::SetName(name);
    mName = name;

    health = 3;
}

Player::~Player()
{
}

void Player::UpdateGame(float dt)
{
	UpdatePlayerMovement(dt);
}

void Player::UpdatePlayerMovement(float dt)
{
    if (!renderObject) {
        return;
    }
    bool isIdle = true;

    GameConfigManager* config = GameBase::GetGameBase()->GetGameConfig();

    // Only process inputs for the local player
    if (config && config->networkConfig.isMultiplayer) {
        std::string playerName = GetName();

        // For the server, control "playerOne"; for the client, control "playerTwo"
        bool shouldControlLocally = (config->networkConfig.isServer && playerName == "playerOne") || (!config->networkConfig.isServer && playerName == "playerTwo");

        if (!shouldControlLocally) {
            // This player is controlled remotely - don't process local inputs
            return;
        }
    }

    Vector3 playerPosition = GetTransform().GetPosition();
    Camera& mainCamera = GameBase::GetGameBase()->GetWorld()->GetMainCamera();

    Vector3 cameraOffset(0, 60.0f, 50.0f);
    mainCamera.SetPosition(playerPosition + cameraOffset);

    float forward = controller->GetAxis(2);
    float sidestep = -controller->GetAxis(0);

    Vector3 currentVelocity = GetPhysicsObject()->GetLinearVelocity();

    Vector3 forwardVec(0, 0, 1);
    Vector3 rightVec(1, 0, 0);

    if (forward != 0.0f)
    {
        forwardVec = -forwardVec;
        
        isIdle = false;
    }
    if (sidestep != 0.0f)
    {
        sidestep = -sidestep;
        isIdle = false;
    }

    float sprintMultiplier = 1.0f;
    if (Window::GetKeyboard()->KeyDown(KeyCodes::SHIFT)) {
        sprintMultiplier = 3.0f; // Adjust the multiplier as needed
    }

    Vector3 movement(0, 0, 0);
    if (forward != 0.0f) {
        movement += forwardVec * forward * GetPlayerSpeed() * sprintMultiplier;
    }
    if (sidestep != 0.0f) {
        movement += rightVec * sidestep * GetPlayerSpeed() * sprintMultiplier;
    }
    if (movement.Length() > 0.0f) {
        float angle = atan2(-movement.x, -movement.z);
        Quaternion targetOrientation = Quaternion::EulerAnglesToQuaternion(0, angle * 180.0f / M_PI, 0);
        Quaternion currentOrientation = GetTransform().GetOrientation();
        Quaternion newOrientation = Quaternion::Slerp(currentOrientation, targetOrientation, dt * 2.0f); // Adjust the interpolation speed as needed
        GetTransform().SetOrientation(newOrientation);
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
    
    if (isIdle) {
        SetObjectAnimationState(Idle);
    }
    else if (!isIdle) {
        SetObjectAnimationState(Walk);
    }
}

void Player::InitializeController()
{
    controller = new KeyboardMouseController(*Window::GetKeyboard(), *Window::GetMouse());

    GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetController(*controller);

    controller->MapAxis(0, "Sidestep");
    controller->MapAxis(1, "UpDown");
    controller->MapAxis(2, "Forward");
    controller->MapAxis(3, "XLook");
    controller->MapAxis(4, "YLook");
}