#include "PuzzleT.h"
#include "PhysicsObject.h"
#include "RenderObject.h"


using namespace NCL;
using namespace CSC8503;

NormalCollidCube::NormalCollidCube(const std::string& name) : GameObject("CollidingCube") {
	AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
	SetBoundingVolume((CollisionVolume*)volume);
}

NormalCollidCube::~NormalCollidCube() {
}

void NormalCollidCube::OnCollisionBegin(GameObject* otherObject) {
	RenderObject* renderObject = GetRenderObject();
	if (renderObject && otherObject->GetName() == "player") {
		static int colorIndex = 0;
		std::vector<Vector4> colors = {
			Vector4(1, 0, 0, 1), // Red
			Vector4(0, 0, 1, 1), // Blue
			Vector4(0, 1, 0, 1), // Green
			Vector4(1, 1, 1, 1), // White
			Vector4(0, 0, 0, 1), // Black
			Vector4(1, 1, 0, 1)  // Yellow
		};
		renderObject->SetColour(colors[colorIndex]);
		colorIndex = (colorIndex + 1) % colors.size();
	}
}

Door::Door() : GameObject("Door") {
	isOpen = false;
	openTexture = nullptr;
	closeTexture = nullptr;
	
}
void Door::Open() {
	isOpen = true;
	GetRenderObject()->SetDefaultTexture(openTexture);

	Vector3 newPosition = GetTransform().GetPosition() + Vector3(0, 10, 0); // Move up by 10 units
	GetTransform().SetPosition(newPosition);
	
}

void Door::Close() {
	isOpen = false;
	GetRenderObject()->SetDefaultTexture(closeTexture);
}
void Door::SetTextures(Texture* closedTex, Texture* openTex)
{
	closeTexture = closedTex;
	openTexture = openTex;
}

void Door::Update(float dt) {
	if (isOpen && GetRenderObject()->GetDefaultTexture() != openTexture) {
		GetRenderObject()->SetDefaultTexture(openTexture);
	}
	else if (!isOpen && GetRenderObject()->GetDefaultTexture() != closeTexture) {
		GetRenderObject()->SetDefaultTexture(closeTexture);
	}
}



ButtonTrigger::ButtonTrigger(const std::string& name) : GameObject(name) {
	isPressed = false;
	linkedDoor = nullptr;
}


void ButtonTrigger::OnCollisionBegin(GameObject* otherObject) {
	if (!isPressed && otherObject->GetName() == "player") {
		isPressed = true;
		GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));  // Change color to green (activated)

		if (linkedDoor) {
			linkedDoor->Open();// Open the door
			
		}
	}
}



void ButtonTrigger::SetLinkedDoor(Door* door) {
	linkedDoor = door;
}

