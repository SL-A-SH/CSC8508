#include "LevelOne.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"



LevelOne::LevelOne()
{

}

LevelOne::~LevelOne()
{

}

void LevelOne::Init()
{
	Level::Init();

	// surrounding walls
	AddWallToWorld(Vector3(200, 40, 2), Vector3(0, 10, 200));
	AddWallToWorld(Vector3(200, 40, 2), Vector3(0, 10, -200));
	AddWallToWorld(Vector3(2, 40, 200), Vector3(200, 10, 0));
	AddWallToWorld(Vector3(2, 40, 200), Vector3(-200, 10, 0));

	// high platform
	AddObstacleToWorld(Vector3(20, 7, 40), Vector3(-100, 6, 28));

	// box that's part of the puzzle
	box = AddBoxToWorld(Vector3(-100, 11, 9), Vector3(5, 5, 5), "box269");

	// ramp for player to climb up to platform
	AddRampToWorld(Vector3(-100, 3, 85), Vector3(20, 3, 20), 20, 0, 0);

	//// button
	//button = new Button();
	//button->spawnButton(Vector3(5, 5, 5), Vector3(-100, 1, -38), "button");

	//// for this level, button can only be activated by the movable box
	//button->SetBoxActivated(true);

}

void LevelOne::Update(float dt)
{
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().UpdateCamera(dt);
	Level::Update(dt);
	//if (button->IsPressed() == false) {
	//	button->pressDetection(boxes);
	//}
}

GameObject* LevelOne::AddWallToWorld(Vector3 wallSize, const Vector3& position) {
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

GameObject* LevelOne::AddBoxToWorld(const Vector3& position, Vector3 dimensions, const std::string name, float inverseMass) {
	GameObject* cube = new GameObject(name);

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2.0f);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(cube);

	return cube;
}

GameObject* LevelOne::AddObstacleToWorld(Vector3 dimensions, const Vector3& position) {
	GameObject* path = new GameObject("path");

	AABBVolume* volume = new AABBVolume(dimensions);

	path->SetBoundingVolume((CollisionVolume*)volume);

	path->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2.0f);

	path->SetRenderObject(new RenderObject(&path->GetTransform(), cubeMesh, basicTex, basicShader));
	path->SetPhysicsObject(new PhysicsObject(&path->GetTransform(), path->GetBoundingVolume()));

	path->GetPhysicsObject()->SetInverseMass(0);
	path->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(path);

	return path;
}

GameObject* LevelOne::AddRampToWorld(const Vector3& position, Vector3 dimensions, float x, float y, float z) {
	GameObject* path = new GameObject("path");

	Quaternion newOrientation = Quaternion::EulerAnglesToQuaternion(x, y, z);
	path->GetTransform().SetOrientation(newOrientation);


	OBBVolume* volume = new OBBVolume(dimensions);
	path->SetBoundingVolume((CollisionVolume*)volume);

	path->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2.0f);

	path->SetRenderObject(new RenderObject(&path->GetTransform(), cubeMesh, basicTex, basicShader));
	path->SetPhysicsObject(new PhysicsObject(&path->GetTransform(), path->GetBoundingVolume()));

	path->GetPhysicsObject()->SetInverseMass(0);
	path->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(path);

	return path;
}

void LevelOne::openExit() {

}