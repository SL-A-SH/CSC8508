#include "AnimationController.h"
#include "Assets.h"
#include "AnimationObject.h"
#include "RenderObject.h"
#include "Mesh.h"
#include <map>
#include <string>

AnimationController::AnimationController(GameWorld& world, std::map<std::string, MeshAnimation*>& preLoadedAnimationList) : mGameWorld(world), mPreLoadedAnimationList(preLoadedAnimationList)
{
	mMesh = nullptr;
	mAnim = nullptr;
	InitPlayerAnimMap();
}

AnimationController::~AnimationController(){}

void AnimationController::Clear() {
	mAnimationList.clear();
	mPlayersList.clear();
}

void AnimationController::Update(float dt, vector<GameObject*> animationObjects)
{
	UpdateCurrentFrames(dt);
	UpdateAllAnimations(dt, animationObjects);
}


void AnimationController::UpdateAllAnimations(float dt, vector<GameObject*> animatedObjects) {
	for (GameObject* obj : animatedObjects) {
		if (obj->GetRenderObject()->GetAnimObject()) {
			std::cout << "Updating Animations" << std::endl;
			AnimationObject* animObj = obj->GetRenderObject()->GetAnimObject();
			if (animObj != nullptr) {
				int currentFrame = animObj->GetCurrentFrame();
				mMesh = obj->GetRenderObject()->GetMesh();
				mAnim = animObj->GetAnimation();

				const Matrix4* invBindPose = mMesh->GetInverseBindPose().data();
				const Matrix4* frameData = mAnim->GetJointData(currentFrame);

				const int* bindPoseIndices = mMesh->GetBindPoseIndices();
				std::vector<std::vector<Matrix4>> frameMatricesVec;
				for (unsigned int i = 0; i < mMesh->GetSubMeshCount(); ++i) {
					Mesh::SubMeshPoses pose;
					mMesh->GetBindPoseState(i, pose);

					vector<Matrix4> frameMatrices;
					for (unsigned int i = 0; i < pose.count; ++i) {
						int jointID = bindPoseIndices[pose.start + i];
						Matrix4 mat = frameData[jointID] * invBindPose[pose.start + i];
						std::cout << "Added to Frame Matrices" << std::endl;
						frameMatrices.emplace_back(mat);
					}
					std::cout << "Added to frame Matrices Vector" << std::endl;
					frameMatricesVec.emplace_back(frameMatrices);
				}
				obj->GetRenderObject()->SetCurrentFrame(currentFrame);
				obj->GetRenderObject()->SetFrameMatricesVector(frameMatricesVec);
				
				frameMatricesVec.clear();
			}
		}
	}
}
void AnimationController::InitPlayerAnimMap() {
	mPlayerAnimationMap = {
		{GameObject::AnimationState::Idle, "PlayerIdle"},
	};
}

void AnimationController::SetAnimationState(GameObject* obj, GameObject::AnimationState state) {
	obj->GetRenderObject()->GetAnimObject()->ResetCurrentFrame();

	AnimationObject::AnimationType animationType = obj->GetRenderObject()->GetAnimObject()->GetAnimationType();
	std::map<GameObject::AnimationState, std::string>& animationMap = mPlayerAnimationMap;

	const std::string& animationName = animationMap[state];

	MeshAnimation* animation = mPreLoadedAnimationList[animationName];

	obj->GetRenderObject()->GetAnimObject()->SetRate(1.0);

	obj->GetRenderObject()->GetAnimObject()->SetAnimation(animation);
}

void AnimationController::UpdateCurrentFrames(float dt) {
	for (AnimationObject*& anim : mAnimationList) {
		anim->Update(dt);
	}
}

void AnimationController::SetObjectList(vector<GameObject*> animationObjects) {
	for (auto& obj : animationObjects) {
		if (obj->GetName().find("player") != std::string::npos) {
			mPlayersList.emplace_back((Player*)obj);
			AnimationObject* animObj = obj->GetRenderObject()->GetAnimObject();
			mAnimationList.emplace_back(animObj);
		}
	}
}