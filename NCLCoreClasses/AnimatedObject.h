#pragma once
#include "MeshAnimation.h"
#include "MeshMaterial.h"

namespace NCL{
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class Transform;
		class AnimatedObject {
		public:
			enum AnimationType {
				playerAnimation,
				enemeyAnimation
			};

			AnimatedObject(AnimationType type, MeshAnimation* anim);
			~AnimatedObject();

			void Update(float dt);
			void SetAnimation(MeshAnimation* anim) {
				mAnimation = anim;
			}
			MeshAnimation* GetAnimation() {
				return mAnimation;
			}
			int GetCurrentFrame() {
				return mCurrentFrame;
			}
			void ResetCurrentFrame() {
				mCurrentFrame = 0;
			}
			void SetRate(float rate) {
				mFrameRate = rate;
			}
			float GetRate() {
				return mFrameRate;
			}
			AnimationType GetAnimationType() {
				return mAnimationType;
			}
		protected:
			MeshAnimation* mAnimation = nullptr;
			AnimationType mAnimationType;

			int mCurrentFrame;
			int mNextFrame;
			float mFrameTime;
			float mFrameRate;

		};
	}
}