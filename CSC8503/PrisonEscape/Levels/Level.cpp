#include "Level.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "PrisonEscape/Core/GameBase.h"
#include "GameTechRenderer.h"


using namespace NCL;
using namespace CSC8503;

Level::Level() :controller(*Window::GetKeyboard(), *Window::GetMouse())
{

}
void Level::Init()
{
	capsuleMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("capsule.msh");
	cubeMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("cube.msh");
	sphereMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("sphere.msh");
	catMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("ORIGAMI_Chat.msh");
	kittenMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("Kitten.msh");

	enemyMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("Keeper.msh");
	bonusMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("19463_Kitten_Head_v1.msh");

	basicTex = GameBase::GetGameBase()->GetRenderer()->LoadTexture("checkerboard.png");
	basicShader = GameBase::GetGameBase()->GetRenderer()->LoadShader("scene.vert", "scene.frag");

	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetNearPlane(0.1f);
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetFarPlane(500.0f);
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetPitch(-35.0f);
	GameBase::GetGameBase()->GetWorld()->GetMainCamera().SetYaw(0.0f);




}

GameObject* Level::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2.0f)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(floor);
	std::cout << "Floor added to world" << std::endl;
	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple'
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* Level::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(sphere);
	std::cout << "Sphere added to world" << std::endl;
	return sphere;
}

GameObject* Level::AddFloorToWorld(const Vector3& position, const Vector3& floorSize, const Vector4& color) {
	GameObject* floor = new GameObject();

	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);

	floor->GetTransform()
		.SetScale(floorSize * 2.0f)
		.SetPosition(position);

	RenderObject* renderObject = new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader);
	renderObject->SetColour(color);
	floor->SetRenderObject(renderObject);
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();



	GameBase::GetGameBase()->GetWorld()->AddGameObject(floor);

	return floor;
}


GameObject* Level::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

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

GameObject* Level::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	SphereVolume* volume = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), capsuleMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(character);
	std::cout << "Player added to world" << std::endl;
	return character;
}

GameObject* Level::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(character);

	return character;
}

GameObject* Level::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(apple);

	return apple;
}


void Level::UpdatePlayerMovement(float dt) {
	if (!_mPlayer) {
		return;
	}


	std::cout << "Called";
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
			movement += forwardVec * forward * _mPlayerSpeed;
		}
		if (sidestep != 0.0f) {
			movement += rightVec * sidestep * _mPlayerSpeed;
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

//StateGameObject* Level::AddStateObjectToWorld(const Vector3& position)
//{
//	StateGameObject* apple = new StateGameObject();
//
//	SphereVolume* volume = new SphereVolume(0.5f);
//	apple->SetBoundingVolume((CollisionVolume*)volume);
//	apple->GetTransform()
//		.SetScale(Vector3(2, 2, 2))
//		.SetPosition(position);
//
//	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
//	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));
//
//	apple->GetPhysicsObject()->SetInverseMass(1.0f);
//	apple->GetPhysicsObject()->InitSphereInertia();
//
//	GameBase::GetWorld()->AddGameObject(apple);
//
//	return apple;
//}