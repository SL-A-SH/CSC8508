#include "Player.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

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
	delete(_mPlayer);
	delete(basicShader);
}

void Player::InitializeAssets()
{
	_mPlayerMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("Male_Guard.msh");

	basicShader = GameBase::GetGameBase()->GetRenderer()->LoadShader("scene.vert", "scene.frag");
}

void Player::SpawnPlayer(Vector3 position)
{
	_mPlayer = AddPlayerToWorld(position);
}

GameObject* Player::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	_mPlayer = new GameObject("player");
	SphereVolume* volume = new SphereVolume(1.0f);


	_mPlayer->SetBoundingVolume((CollisionVolume*)volume);

	_mPlayer->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize) * 10.0f).SetPosition(position);

	_mPlayer->SetRenderObject(new RenderObject(&_mPlayer->GetTransform(), _mPlayerMesh, nullptr, basicShader));
	_mPlayer->SetPhysicsObject(new PhysicsObject(&_mPlayer->GetTransform(), _mPlayer->GetBoundingVolume()));

	_mPlayer->GetPhysicsObject()->SetInverseMass(inverseMass);
	_mPlayer->GetPhysicsObject()->InitSphereInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(_mPlayer);

	return _mPlayer;
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



		return;
	}
}