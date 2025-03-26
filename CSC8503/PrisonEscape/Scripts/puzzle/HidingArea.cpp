#include "HidingArea.h"
#include "CollisionVolume.h"
#include "CollisionDetection.h"

using namespace NCL;
using namespace CSC8503;

HidingArea::HidingArea(const Vector3& position, const Vector3& size) : GameObject("HidingArea") {
    hidingAreaPosition = position;
    hidingAreaSize = size;
    SetBoundingVolume(new AABBVolume(size * 0.5f)); // Assuming AABBVolume is used for bounding volume
    GetTransform().SetPosition(position);
}

HidingArea::~HidingArea() {
}

void HidingArea::Update(float dt) {
    // Update logic for the hiding area if needed
}

void HidingArea::OnCollisionBegin(GameObject* otherObject) {
	// Check if the object is a player
	if (otherObject->GetName() == "playerOne") {
		otherObject->SetActive(false);
	}
}

void HidingArea::OnCollisionEnd(GameObject* otherObject) {
	// Check if the object is a player
	if (otherObject->GetName() == "playerOne") {
		otherObject->SetActive(true);
	}
}