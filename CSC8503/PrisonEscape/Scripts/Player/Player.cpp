#include "Player.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

#include "PrisonEscape/Prefabs/Player/PlayerTwo.h"
#include "PrisonEscape/Core/GameBase.h"
#include "PrisonEscape/Core/GameConfigManager.h"
#include "PrisonEscape/Core/ImGuiManager.h"

Player::Player() : controller(*Window::GetKeyboard(), *Window::GetMouse())
{
	playerObject = nullptr;
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
	delete(playerObject);
	delete(basicShader);
}

void Player::InitializeAssets()
{
	playerMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("Male_Guard.msh");

	basicShader = GameBase::GetGameBase()->GetRenderer()->LoadShader("scene.vert", "scene.frag");
}

void Player::SpawnPlayer(Vector3 position)
{
	playerObject = AddPlayerToWorld(position);
}

GameObject* Player::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	playerObject = new GameObject("player");
	SphereVolume* volume = new SphereVolume(1.0f);


	playerObject->SetBoundingVolume((CollisionVolume*)volume);

	playerObject->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);

	playerObject->SetRenderObject(new RenderObject(&playerObject->GetTransform(), playerMesh, nullptr, basicShader));
	playerObject->SetPhysicsObject(new PhysicsObject(&playerObject->GetTransform(), playerObject->GetBoundingVolume()));

	playerObject->GetPhysicsObject()->SetInverseMass(inverseMass);
	playerObject->GetPhysicsObject()->InitSphereInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(playerObject);

	return playerObject;
}

void Player::UpdateGame(float dt)
{
	UpdatePlayerMovement(dt);


}

void Player::UpdatePlayerMovement(float dt)
{

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
		HandleJumpingAndGravity(dt, currentVelocity, movement);
	}
	else {
		playerObject->GetPhysicsObject()->SetLinearVelocity(movement);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::V)) {
		Vector3 position = playerObject->GetTransform().GetPosition();
		std::cout << "Player position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
	}
}

void Player::HandleJumpingAndGravity(float dt, Vector3& currentVelocity, Vector3& movement) {
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


void Player::DetectPlayerCollision() {
	if (!playerObject) {
		return;
	}
	rayDir = playerObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

	rayPos = playerObject->GetTransform().GetPosition();

	Ray r = Ray(rayPos, rayDir);
	Debug::DrawLine(rayPos, rayPos + rayDir * 100.0f, Vector4(1, 0, 0, 1));

	if (GameBase::GetGameBase()->GetWorld()->Raycast(r, closestCollision, true, playerObject)) {

		GameObject* collisionObject = (GameObject*)closestCollision.node;
		if (collisionObject->GetPhysicsObject()->GetLayer() == 1) {
			GameBase::GetGameBase()->GetRenderer()->AddDrawableFunction("Interaction", [this]() {DisplayInteractionPrompt();});
		}
		else
		{
			GameBase::GetGameBase()->GetRenderer()->RemoveDrawableFunction("Interaction");
		}
	}

}

void Player::DisplayInteractionPrompt()
{
	ImGuiManager::DrawPanel("", {}, {}, {}, "Press E to interact");
}