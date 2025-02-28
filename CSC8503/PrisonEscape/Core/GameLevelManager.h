#pragma once

#include "GameWorld.h"
#include "PhysicsSystem.h"
#include "GameTechRenderer.h"
#include "../CSC8503/PrisonEscape/Scripts/Player/Player.h"
#include "../CSC8503/PrisonEscape/Levels/Level.h"

namespace NCL {
	namespace CSC8503 {
		class GameLevelManager {
		public:
			GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer);
			~GameLevelManager();
			virtual void UpdateGame(float dt);



		public:
			Level* GetCurrentLevel() { return currentLevel; }
			void SetCurrentLevel(Level* level) { currentLevel = level; }
			void AddLevel(Level* newLevel) { levelStack.push(newLevel); }
			void InitAssets();
			void AssignPlayerObjects(Player& player, const Transform& playerTransform);

			Player* AddPlayerToWorld(const Transform& transform, const std::string& name);
		private:
			GameWorld* world;
			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			Level* currentLevel;
			std::stack<Level*> levelStack;

			std::unordered_map<std::string, Mesh*> mMeshes;
			std::unordered_map<std::string, Texture*> mTextures;
			std::unordered_map<std::string, Shader*> mShaders;
			std::unordered_map<std::string, MeshMaterial*> mMaterials;
			std::unordered_map<std::string, MeshAnimation*> mAnimations;
			std::unordered_map<std::string, vector<int>> mMeshMaterials;

			std::vector<std::string> mShadersToLoad;
			std::map<std::string, MeshAnimation*> preloadedAnimations;

			Player* mTempPlayer;
		};
	}
}