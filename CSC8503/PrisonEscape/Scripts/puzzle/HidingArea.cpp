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

bool HidingArea::IsPlayerInside(const Vector3& playerPosition) const {
    return (playerPosition.x >= hidingAreaPosition.x && playerPosition.x <= hidingAreaPosition.x + hidingAreaSize.x &&
        playerPosition.y >= hidingAreaPosition.y && playerPosition.y <= hidingAreaPosition.y + hidingAreaSize.y &&
        playerPosition.z >= hidingAreaPosition.z && playerPosition.z <= hidingAreaPosition.z + hidingAreaSize.z);
}