#include "AnimationObject.h"

using namespace NCL;
using namespace CSC8503;

AnimationObject::AnimationObject(AnimationType type, MeshAnimation* anim) {
	animation = anim;
	animationType = type;

	currentFrame = 0;
	nextFrame = 1;
	frameTime = 0.0f;
	frameRate = 1.0f;
}

AnimationObject::~AnimationObject() {
	animation = nullptr;
}

void AnimationObject::Update(float dt) {
	std::cout << "UPDATiING ANIMATIONS" << std::endl;
	if (animation == nullptr) {
		std::cout << "NO ANIMATION" << std::endl;
		return;
	}
	else {
		std::cout << "Animation Found" << std::endl;
	}
	frameTime -= frameRate * dt; // scale by frame rate to change speed of animation

	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % animation->GetFrameCount();
		nextFrame = (currentFrame + 1) % animation->GetFrameCount();
		frameTime += 1.0f / animation->GetFrameRate();
	}
}