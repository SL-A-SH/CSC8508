#pragma once

#include "GameWorld.h"
#include "PhysicsSystem.h"
#include "GameTechRenderer.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "../CSC8503/PrisonEscape/Levels/Level.h"
#include "../CSC8503/PrisonEscape/Scripts/Player/Player.h"
#include "../CSC8503/PrisonEscape/Scripts/PatrolEnemy/PatrolEnemy.h"

namespace NCL {
	constexpr float PLAYER_MESH_SIZE = 3.0f;
	constexpr float PLAYER_INVERSE_MASS = 0.5f;
	constexpr float PATROL_ENEMY_MESH_SIZE = 3.0f;
	constexpr float PATROL_ENEMY_INVERSE_MASS = 0.5f;
	namespace CSC8503 {
		class GameLevelManager {
		public:
			GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer);
			~GameLevelManager();
			virtual void UpdateGame(float dt);

			void InitAssets();

			// Player Methods

			PlayerOne* AddPlayerToWorld(const Transform& transform, const std::string& playerName);
			void AddComponentsToPlayer(Player& playerObj, const Transform& transform);

			// Enemy Methods

			PatrolEnemy* AddPatrolEnemyToWorld(const Transform& transform);
			void AddComponentsToPatrolEnemy(PatrolEnemy& enemyObj, const Transform& transform);

		public:
			Level* GetCurrentLevel() { return mCurrentLevel; }
			void SetCurrentLevel(Level* level) { mCurrentLevel = level; }
			void AddLevel(Level* newLevel) { mLevelStack.push(newLevel); }
		private:
			GameWorld* mWorld;
			GameTechRenderer* mRenderer;
			PhysicsSystem* mPhysics;
			Level* mCurrentLevel;
			std::stack<Level*> mLevelStack;


			vector<GameObject*> mUpdatableObjectList;

			std::unordered_map<std::string, Mesh*> mMeshList;
			std::unordered_map<std::string, Shader*> mShaderList;
			std::unordered_map<std::string, Texture*> mTextureList;
			std::unordered_map<std::string, MeshAnimation*> mAnimationList;
			std::unordered_map<std::string, MeshMaterial*> mMaterialList;
			std::unordered_map<std::string, vector<int>> mMeshMaterialsList;
			std::map<std::string, MeshAnimation*> mPreLoadedAnimationList;

			//Player Members
			PlayerOne* mPlayerToAdd;

			//Enemy Members
			PatrolEnemy* mEnemyToAdd;
		};
	}
}