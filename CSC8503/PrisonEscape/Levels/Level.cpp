#include "Level.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "GameTechRenderer.h"

#include "PrisonEscape/Core/GameBase.h"
#include "PrisonEscape/Prefabs/Player/PlayerOne.h"

using namespace NCL;
using namespace CSC8503;

Level::Level()
{
	playerOne = new PlayerOne();
}

void Level::Init()
{
	InitializeAssets();
	InitializeLevel();
	SetCameraAttributes();
}

void Level::Update(float dt)
{
	if (playerOne)
	{
		playerOne->UpdatePlayerMovement(dt);
	}
}

void Level::InitializeAssets()
{
	cubeMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("cube.msh");

	basicTex = GameBase::GetGameBase()->GetRenderer()->LoadTexture("checkerboard.png");
	basicShader = GameBase::GetGameBase()->GetRenderer()->LoadShader("scene.vert", "scene.frag");
}

void Level::InitializeLevel()
{
	AddFloorToWorld(Vector3(0, 0, 0), Vector3(200, 0, 200), Vector4(0.5, 0.5, 0.5, 1));
	playerOne->SpawnPlayer(Vector3(0, 50, 0));
}

void Level::SetCameraAttributes()
{
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetNearPlane(0.1f);
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetFarPlane(500.0f);
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPitch(-35.0f);
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetYaw(0.0f);
}

GameObject* Level::AddFloorToWorld(const Vector3& position, const Vector3& floorSize, const Vector4& color) {
	GameObject* floor = new GameObject();

	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);

	floor->GetTransform().SetScale(floorSize * 2.0f).SetPosition(position);

	RenderObject* renderObject = new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader);
	renderObject->SetColour(color);
	floor->SetRenderObject(renderObject);
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(floor);

	return floor;
}