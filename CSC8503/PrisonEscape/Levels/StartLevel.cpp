#include "StartLevel.h"
#include "Level.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "GameTechRenderer.h"
#include "PrisonEscape/Scripts/puzzle/puzzleT.h"
#include "PrisonEscape/Prefabs/Player/PlayerOne.h" 

#include "PrisonEscape/Core/GameBase.h"
StartLevel::StartLevel()
{
	doorCloseTex = GameBase::GetGameBase()->GetRenderer()->LoadTexture("Default.png");
	doorOpenTex = GameBase::GetGameBase()->GetRenderer()->LoadTexture("checkerboard.png");
	
}

StartLevel::~StartLevel()
{

}

void StartLevel::Init()
{
	Level::Init();
	AddWallToWorld(Vector3(200, 40, 2), Vector3(0, 10, 200));
	AddWallToWorld(Vector3(200, 40, 2), Vector3(0, 10, -200));
	AddWallToWorld(Vector3(2, 40, 200), Vector3(200, 10, 0));
	AddWallToWorld(Vector3(2, 40, 200), Vector3(-200, 10, 0));
	AddWallToWorld(Vector3(90, 40, 2), Vector3(110, 10, 0));
	AddWallToWorld(Vector3(90, 40, 2), Vector3(-110, 10, 0));
	AddWallToWorld(Vector3(2, 40, 30), Vector3(150, 10, 170));
	AddWallToWorld(Vector3(2, 40, 30), Vector3(100, 10, 170));
	AddWallToWorld(Vector3(2, 40, 30), Vector3(50, 10, 170));
	AddWallToWorld(Vector3(2, 40, 30), Vector3(0, 10, 170));
	AddWallToWorld(Vector3(2, 40, 60), Vector3(-50, 10, 140));
	AddWallToWorld(Vector3(2, 40, 20), Vector3(-50, 10, 20));

	AddWallToWorld(Vector3(20, 40, 2), Vector3(180, 10, -80));
	AddWallToWorld(Vector3(40, 40, 2), Vector3(100, 10, -80));
	AddWallToWorld(Vector3(20, 40, 2), Vector3(20, 10, -80));
	AddWallToWorld(Vector3(2, 40, 60), Vector3(100, 10, -140));
	AddWallToWorld(Vector3(2, 40, 60), Vector3(0, 10, -140));
	AddWallToWorld(Vector3(2, 40, 60), Vector3(-40, 10, -60));
	AddWallToWorld(Vector3(60, 40, 2), Vector3(-140, 10, -120));

	CreateRandomColorCube(Vector3(-190, 5, -190));
	
	CreateCollidingCube(Vector3(190, 3, -190));
	CreateCollidingCube(Vector3(80, 3, -190));
	CreateCollidingCube(Vector3(-190, 3, -80));

	Door* door = new Door();
	door->SetTextures(doorCloseTex, doorOpenTex);
	AddDoorToWorld(door, Vector3(0, 25, 3));

	// Create a button linked to the door
	ButtonTrigger* button = new ButtonTrigger();
	AddButtonToWorld(button, Vector3(-150, 2, 120), door);
	 std::cout << "CubeSet contents:" << std::endl;
    for (const Vector4& color : CubeSet) {
        std::cout << "Color: (" << color.x << ", " << color.y << ", " << color.z << ", " << color.w << ")" << std::endl;
    }
}

void StartLevel::AddCubeSetToTheLevel() {


}

void StartLevel::Update(float dt)
{

	GameBase::GetGameBase()->GetWorld()->GetMainCamera().UpdateCamera(dt);
	
	Level::Update(dt);
	//UpdateCubeVisibility();
}
GameObject* StartLevel::CreateRandomColorCube(Vector3 position) {
	GameObject* cube = new GameObject("RandomColorCube");

	AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
	cube->SetBoundingVolume((CollisionVolume*)volume);

	Vector4 colors[] = {
		Vector4(1, 0, 0, 1), // Red
		Vector4(0, 0, 1, 1), // Blue
		Vector4(0, 1, 0, 1), // Green
		Vector4(1, 1, 1, 1), // White
		Vector4(0, 0, 0, 1), // Black
		Vector4(1, 1, 0, 1)  // Yellow
	};

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, 5);

	Vector4 randomColor = colors[dis(gen)];


	cube->GetTransform().SetScale(Vector3(1, 1, 1) *3.0f).SetPosition(position);
	RenderObject* renderObject = new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader);
	renderObject->SetColour(randomColor);
	CubeSet.push_back(randomColor);
	cube->SetRenderObject(renderObject);
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(cube);
	Cubes.push_back(cube);
	return cube;
}

void StartLevel::CreateCollidingCube(Vector3 position)
{
	NormalCollidCube* cube = new NormalCollidCube();
	cube->GetTransform().SetScale(Vector3(1, 1, 1) * 3.0f).SetPosition(position);
	RenderObject* renderObject = new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader);
	cube->SetRenderObject(renderObject);
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();
	GameBase::GetGameBase()->GetWorld()->AddGameObject(cube);
}

GameObject* StartLevel::AddDoorToWorld(Door* door, const Vector3& position) {
	Vector3 doorSize(20.0f, 25.0f, 1.0f);
	AABBVolume* volume = new AABBVolume(doorSize);
	door->SetBoundingVolume((CollisionVolume*)volume);

	door->GetTransform().SetScale(doorSize * 2.0f).SetPosition(position);
	door->SetRenderObject(new RenderObject(&door->GetTransform(), cubeMesh, basicTex, basicShader));
	door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume()));

	door->GetPhysicsObject()->SetInverseMass(0);
	door->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(door);
	return door;
}

GameObject* StartLevel::AddWallToWorld(Vector3 wallSize, const Vector3& position) {
	GameObject* wall = new GameObject("wall");
	AABBVolume* volume = new AABBVolume(wallSize);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform()
		.SetScale(wallSize * 2.0f)
		.SetPosition(position);
	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));
	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();
	GameBase::GetGameBase()->GetWorld()->AddGameObject(wall);

	return wall;
}

void StartLevel::UpdateCubeVisibility() {
	PlayerOne* player = GetPlayerOne();
	if (!player) return;

	Vector3 playerPos = player->GetTransform().GetPosition();
	float visibilityThreshold = 5.0f; // Adjust visibility range

	for (GameObject* cube : Cubes) {
		if (!cube) continue; // Ensure cube is valid

		Vector3 cubePos = cube->GetTransform().GetPosition();
		float distance = (cubePos - playerPos).Length();

		RenderObject* renderObject = cube->GetRenderObject();
		if (!renderObject) continue; // Ensure cube has a RenderObject

		Vector4 color = renderObject->GetColour();

		// If the cube is within the visibility threshold, make it visible, otherwise hide it
		if (distance <= visibilityThreshold) {
			color.w = 1.0f; // Fully visible
		}
		else {
			color.w = 0.0f; // Invisible (completely transparent)
		}

		renderObject->SetColour(color);
	}
}


GameObject* StartLevel::AddButtonToWorld(ButtonTrigger* button, const Vector3& position, Door* linkedDoor) {
	Vector3 buttonSize(2.0f, 0.3f, 2.0f);
	AABBVolume* volume = new AABBVolume(buttonSize);
	button->SetBoundingVolume((CollisionVolume*)volume);

	button->GetTransform().SetScale(buttonSize * 2.0f).SetPosition(position);
	button->SetRenderObject(new RenderObject(&button->GetTransform(), cubeMesh, basicTex, basicShader));
	button->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));  // Red when inactive

	button->SetPhysicsObject(new PhysicsObject(&button->GetTransform(), button->GetBoundingVolume()));
	button->GetPhysicsObject()->SetInverseMass(0);
	button->GetPhysicsObject()->InitCubeInertia();

	button->SetLinkedDoor(linkedDoor);
	GameBase::GetGameBase()->GetWorld()->AddGameObject(button);
	return button;
}