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

void Button::spawnButton(Vector3 size, Vector3 position, std::string name, Mesh* mesh, Shader* shader, Texture* texture) {
	buttonObject = AddButtonToWorld(size, position, name, mesh, shader, texture);
}

void Button::pressDownButton() {
	if (!pressed && buttonObject) {
		buttonObject->GetTransform().SetPosition(buttonObject->GetTransform().GetPosition() + Vector3(0, -1.5, 0));
		pressed = true;
	}
}


//void Button::pressDetection(GameObject* otherObject) {
//	if (!otherObject || !buttonObject) return;
//
//	CollisionDetection::CollisionInfo info;
//	if (CollisionDetection::ObjectIntersection(this->buttonObject, otherObject, info)) {
//		bool isBox = otherObject->GetName().find("Box") != std::string::npos; 
//		bool isPlayer = otherObject->GetName() == "player";
//
//		if ((boxActivated && isBox) || (playerActivated && isPlayer)) {
//			pressDownButton();
//		}
//	}
//}

void Button::pressDetection(const std::vector<GameObject*>& boxes) {
	CollisionDetection::CollisionInfo info;

	for (GameObject* box : boxes) {
		if (box && CollisionDetection::ObjectIntersection(this->buttonObject, box, info)) {
			if (boxActivated) {
				pressDownButton();
				return;
			}
		}
	}


	//if (player && CollisionDetection::ObjectIntersection(this->buttonObject, player, info)) {
	//	if (playerActivated) {
	//		pressDownButton();
	//	}
	//}
}


GameObject* Button::AddButtonToWorld(Vector3 size, const Vector3& position, const std::string name, Mesh* mesh, Shader* shader, Texture* texture) {
	Vector3 offset = position + Vector3(0, 2.5f, 0);
	
	GameObject* button = new GameObject(name);

	AABBVolume* volume = new AABBVolume(size * 0.5f);
	button->SetBoundingVolume((CollisionVolume*)volume);
	button->GetTransform()
		.SetScale(size)
		.SetPosition(offset);

	RenderObject* renderObject = new RenderObject(&button->GetTransform(), mesh, texture, shader);
	renderObject->SetColour(Vector4(1, 0, 0, 1)); // Set to red

	button->SetRenderObject(renderObject);

	button->SetPhysicsObject(new PhysicsObject(&button->GetTransform(), button->GetBoundingVolume()));

	button->GetPhysicsObject()->SetInverseMass(0);
	button->GetPhysicsObject()->InitCubeInertia();

	GameBase::GetGameBase()->GetWorld()->AddGameObject(button);

	return button;
}