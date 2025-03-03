#include "Button.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "TextureLoader.h"
#include "CollisionVolume.h"
#include "CollisionDetection.h"


Button::Button() {
	pressed = false;
	playerActivated = false;
	boxActivated = false;
}

Button::~Button() {
	buttonMesh = nullptr;
	basicShader = nullptr;
	buttonObject = nullptr;
}

void Button::InitializeAssets() {
	buttonMesh = GameBase::GetGameBase()->GetRenderer()->LoadMesh("cube.msh");
	basicShader = GameBase::GetGameBase()->GetRenderer()->LoadShader("scene.vert", "scene.frag");
}

void Button::spawnButton(Vector3 position) {
	InitializeAssets();
	buttonObject = AddButtonToWorld(Vector3(8, 3, 8), position, "button");
}

void Button::pressDownButton() {
	if (!pressed) {
		buttonObject->GetTransform().SetPosition(buttonObject->GetTransform().GetPosition() + Vector3(0, -1.5, 0));
		pressed = true;
	}
}

void Button::pressDetection(GameObject* otherObject, const std::string desiredName) {
	CollisionDetection::CollisionInfo info;
	if (CollisionDetection::ObjectIntersection(this->buttonObject, otherObject, info)) {
		if (desiredName == otherObject->GetName()) {
			if (boxActivated && otherObject->GetName() == "box") {
				pressDownButton();
				std::cout << "BOOP\n";
			}
			if (playerActivated && otherObject->GetName() == "player") {
				pressDownButton();
			}
		}
	}
}

GameObject* Button::AddButtonToWorld(Vector3 size, const Vector3& position, const std::string name) {
	GameObject* button = new GameObject(name);

	AABBVolume* volume = new AABBVolume(size);
	button->SetBoundingVolume((CollisionVolume*)volume);
	button->GetTransform()
		.SetScale(size * 2.0f)
		.SetPosition(position);

	RenderObject* renderObject = new RenderObject(&button->GetTransform(), buttonMesh, nullptr, basicShader);
	renderObject->SetColour(Vector4(1, 0, 0, 1)); // Set to red

	button->SetRenderObject(renderObject);

	button->SetPhysicsObject(new PhysicsObject(&button->GetTransform(), button->GetBoundingVolume()));

	button->GetPhysicsObject()->SetInverseMass(0);
	button->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(button);

	return button;
}