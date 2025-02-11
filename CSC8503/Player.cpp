#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "Player.h"

using namespace NCL;
using namespace CSC8503;

Player::Player() : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	world = new GameWorld();
#ifdef USEVULKAN
	renderer = new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics = new PhysicsSystem(*world);

	PlayerSpeed = 50.0f;
	PlayerStrength = 1000.0f;
	useGravity = true;


	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	InitialiseAssets();
	physics->UseGravity(useGravity);

	InitCamera();
	cameraAttached = true;

}

void Player::InitialiseAssets() {
	cubeMesh = renderer->LoadMesh("cube.msh");
	sphereMesh = renderer->LoadMesh("sphere.msh");
	catMesh = renderer->LoadMesh("ORIGAMI_Chat.msh");
	kittenMesh = renderer->LoadMesh("Kitten.msh");
	gooseMesh = renderer->LoadMesh("goose.msh");

	enemyMesh = renderer->LoadMesh("Keeper.msh");
	bonusMesh = renderer->LoadMesh("19463_Kitten_Head_v1.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex = renderer->LoadTexture("checkerboard.png");


	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
}

Player::~Player() {
	delete cubeMesh;
	delete sphereMesh;
	delete catMesh;
	delete kittenMesh;
	delete enemyMesh;
	delete bonusMesh;
	delete gooseMesh;

	delete basicTex;

	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void Player::UpdateGame(float dt) {

	world->GetMainCamera().UpdateCamera(dt);
	
	UpdatePlayerMovement(dt);

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void Player::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);
	world->GetMainCamera().SetPitch(-35.0f);
	world->GetMainCamera().SetYaw(0.0f);

	if (cameraAttached) {
		Vector3 playerPosition = player->GetTransform().GetPosition();
		world->GetMainCamera().SetPosition(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));
	}
	else {
		world->GetMainCamera().SetPosition(Vector3(-150, 7, -150));
	}
}

GameObject* Player::AddFloorToWorld(const Vector3& position, const Vector3& floorSize, const Vector4& color) {
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

	floor->SetAsFloor(true);

	world->AddGameObject(floor);

	return floor;
}

GameObject* Player::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
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

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* Player::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
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

	world->AddGameObject(cube);

	return cube;
}

GameObject* Player::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	SphereVolume* volume = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), capsuleMesh, basicTex, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));
	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

void Player::UpdatePlayerMovement(float dt) {
	if (!player) {
		return;
	}

	if (cameraAttached) {
		Vector3 playerPosition = player->GetTransform().GetPosition();
		Camera& mainCamera = world->GetMainCamera();

		static bool cameraAdjusted = false;
		if (!cameraAdjusted) {
			cameraAdjusted = true;
		}

		Vector3 cameraOffset(0, 60.0f,50.0f);  

		mainCamera.SetPosition(playerPosition + cameraOffset);

		float forward = controller.GetAxis(2);
		float sidestep = -controller.GetAxis(0);

		Vector3 currentVelocity = player->GetPhysicsObject()->GetLinearVelocity();

		
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
			movement += forwardVec * forward * PlayerSpeed;
		}
		if (sidestep != 0.0f) {
			movement += rightVec * sidestep * PlayerSpeed;
		}

		if (useGravity) {
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

			player->GetPhysicsObject()->SetLinearVelocity(Vector3(movement.x, currentVelocity.y, movement.z));
		}
		else {
			player->GetPhysicsObject()->SetLinearVelocity(movement);
		}

		
		Quaternion newOrientation = Quaternion::EulerAnglesToQuaternion(0, 180.0f, 0);
		player->GetTransform().SetOrientation(newOrientation);

		return;
	}

	if (!cameraAttached) {
		return;
	}
}


void Player::InitWorld() {
	world->ClearAndErase();
	physics->Clear();


	floor = AddFloorToWorld(Vector3(0, 0, 0), Vector3(200, 0, 200), Vector4(0.5, 0.5, 0.5, 1));

	player = AddPlayerToWorld(Vector3(0, 5, 0));

}



