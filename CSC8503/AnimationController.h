#pragma once
#include "GameWorld.h"
#include "MeshAnimation.h"
#include "RenderObject.h"

#include <map>


namespace NCL {
	namespace CSC8503
	{
		class AnimationObject;
		class Player;
		class AnimationController {
		public:
			AnimationController(GameWorld& world, std::map<std::string, MeshAnimation*>& preLoadedAnimationList);
			~AnimationController();

			void clear();
			void Update(float dt, vector<GameObject*> updatableObjects);

			void UpdateCurrentFrames(float dt);
			void UpdateAnimations(float dt);
		
		};
	}
}