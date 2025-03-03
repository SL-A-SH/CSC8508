#include "Player.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

#include "PrisonEscape/Prefabs/Player/PlayerTwo.h"
#include "PrisonEscape/Core/GameBase.h"
#include "PrisonEscape/Core/GameConfigManager.h"

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

	playerObject->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize) * 10.0f).SetPosition(position);

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

}