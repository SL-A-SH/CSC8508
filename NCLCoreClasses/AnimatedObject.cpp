#include "AnimatedObject.h"

using namespace NCL;
using namespace CSC8503;
AnimatedObject::AnimatedObject(AnimationType type, MeshAnimation* anim) {
	this->mAnimation = anim;
	this->mAnimationType = type;

	mCurrentFrame = 0;
	mNextFrame = 0;
	mFrameTime = 0.0f;
	mFrameRate = 1.0f;
}

AnimatedObject::~AnimatedObject() {
	mAnimation = nullptr;
}

void AnimatedObject::Update(float dt) {
	if (mAnimation == nullptr) return;
	mFrameTime -= mFrameRate * dt;
	
	while (mFrameTime < 0.0f) {
		mCurrentFrame = (mCurrentFrame + 1) % mAnimation->GetFrameCount();
		mNextFrame = (mCurrentFrame + 1) % mAnimation->GetFrameCount();
		mFrameTime += 1.0f / mAnimation->GetFrameRate();
	}
}