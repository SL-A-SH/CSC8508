#include "Player.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "MeshMaterial.h"
#include "PrisonEscape/Core/GameBase.h"

Player::Player() : controller(*Window::GetKeyboard(), *Window::GetMouse())
{
	_mPlayer = nullptr;
	useGravity = true;
	cameraAttached = true;
	InitializeAssets();

	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");
	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");
}

Player::~Player()
{
	mesh = nullptr;
	basicShader = nullptr;
	material = nullptr;
	idleAnim = nullptr;
}

void Player::InitializeAssets()
{
	mesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("Max/Rig_Maximilian.msh");

	idleAnim = new MeshAnimation("Max/PassiveIdle.anm");

	basicShader = GameBase::GetGameBase()->GetRenderer()->LoadShader("scene.vert", "scene.frag");
}


void Player::UpdateGame(float dt) {
	UpdatePlayerMovement(dt);
}

void Player::UpdatePlayerMovement(float dt) {
	if (!_mPlayer) {
		return;
	}

	// setting by default true since it's attached to the camera
	if (true) {
		Vector3 playerPosition = _mPlayer->GetTransform().GetPosition();
		Camera& mainCamera = GameBase::GetGameBase()->GetWorld()->GetMainCamera();

		static bool cameraAdjusted = false;
		if (!cameraAdjusted) {
			cameraAdjusted = true;
		}

		Vector3 cameraOffset(0, 60.0f, 50.0f);

		mainCamera.SetPosition(playerPosition + cameraOffset);

		float forward = controller.GetAxis(2);
		float sidestep = -controller.GetAxis(0);

		Vector3 currentVelocity = _mPlayer->GetPhysicsObject()->GetLinearVelocity();


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

		//if use gravity is true by def for now
		if (true) {
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

			_mPlayer->GetPhysicsObject()->SetLinearVelocity(Vector3(movement.x, currentVelocity.y, movement.z));
		}
		else {
			_mPlayer->GetPhysicsObject()->SetLinearVelocity(movement);
		}


		Quaternion newOrientation = Quaternion::EulerAnglesToQuaternion(0, 180.0f, 0);
		_mPlayer->GetTransform().SetOrientation(newOrientation);

		return;
	}
}