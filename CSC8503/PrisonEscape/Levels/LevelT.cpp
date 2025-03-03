#include "LevelT.h"
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
LevelT::LevelT()
{
	doorCloseTex = GameBase::GetGameBase()->GetRenderer()->LoadTexture("Default.png");
	doorOpenTex = GameBase::GetGameBase()->GetRenderer()->LoadTexture("checkerboard.png");

}

LevelT::~LevelT()
{

}

void LevelT::Init()
{
	Level::Init();
	CreateRandomColorCube(Vector3(1, 5, 0));
	CreateRandomColorCube(Vector3(4, 5, 0));
	CreateCollidingCube(Vector3(10, 3, 0));
	CreateCollidingCube(Vector3(15, 3, 0));

	Door* door = new Door();
	door->SetTextures(doorCloseTex, doorOpenTex);
	AddDoorToWorld(door, Vector3(10, 2, 10));

	// Create a button linked to the door
	ButtonTrigger* button = new ButtonTrigger();
	AddButtonToWorld(button, Vector3(6, 2, 10), door);
	std::cout << "CubeSet contents:" << std::endl;
	for (const Vector4& color : CubeSet) {
		std::cout << "Color: (" << color.x << ", " << color.y << ", " << color.z << ", " << color.w << ")" << std::endl;
	}
}

void LevelT::AddCubeSetToTheLevel() {


}

void LevelT::Update(float dt)
{

	GameBase::GetGameBase()->GetWorld()->GetMainCamera().UpdateCamera(dt);

	Level::Update(dt);
	//UpdateCubeVisibility();
}
GameObject* LevelT::CreateRandomColorCube(Vector3 position) {
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


	cube->GetTransform().SetScale(Vector3(1, 1, 1) * 3.0f).SetPosition(position);
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

void LevelT::CreateCollidingCube(Vector3 position)
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

GameObject* LevelT::AddDoorToWorld(Door* door, const Vector3& position) {
	Vector3 doorSize(5.0f, 3.0f, 0.5f);
	AABBVolume* volume = new AABBVolume(doorSize);
	door->SetBoundingVolume((CollisionVolume*)volume);

	door->GetTransform().SetScale(doorSize * 2.0f).SetPosition(position);
	door->SetRenderObject(new RenderObject(&door->GetTransform(), cubeMesh, basicTex, basicShader));
	door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume()));
	door->GetPhysicsObject()->SetLayer(LayerMask::Doors);
	door->GetPhysicsObject()->SetInverseMass(0);
	door->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(door);
	return door;
}

void LevelT::UpdateCubeVisibility() {
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


GameObject* LevelT::AddButtonToWorld(ButtonTrigger* button, const Vector3& position, Door* linkedDoor) {
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