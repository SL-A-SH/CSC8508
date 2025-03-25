#pragma once

#include "PhysicsSystem.h"
#include "../CSC8503/jsonParser.h"

namespace NCL {
	namespace CSC8503 {
		constexpr float PLAYER_MESH_SIZE = 3.0f;
		constexpr float PLAYER_INVERSE_MASS = 0.5f;
		constexpr float PATROL_ENEMY_MESH_SIZE = 3.0f;
		constexpr float PATROL_ENEMY_INVERSE_MASS = 0.1f;

		class GameWorld;
		class Level;
		class Button;
		class Player;
		class PatrolEnemy;
		class jsonParser;
		class GameTechRenderer;
		class AnimationController;

		class GameLevelManager {
		public:
			GameLevelManager(GameWorld* existingWorld, GameTechRenderer* existingRenderer, string LevelToLoad);
			~GameLevelManager();
			virtual void UpdateGame(float dt);

			void InitAssets();
			void InitAnimationObjects() const;

			Player* AddPlayerToWorld(const Transform& transform, const std::string& playerName);
			void AddComponentsToPlayer(Player& playerObj, const Transform& transform);


			PatrolEnemy* AddPatrolEnemyToWorld(const std::string& enemyName, const std::vector<Vector3>& patrolPoints, Player* player);
			void AddComponentsToPatrolEnemy(PatrolEnemy& enemyObj, const Transform& transform);

			void loadMap();

		public:
			Level* GetCurrentLevel() { return mCurrentLevel; }
			void SetCurrentLevel(Level* level) { mCurrentLevel = level; }
			void AddLevel(Level* newLevel) { mLevelStack.push(newLevel); }

			AnimationController* GetAnimator() { return mAnimator; }

			static GameLevelManager* GetGameLevelManager() { return manager; }

			void SetLevelToLoad(string levelName) { mLevelToLoad = levelName; }

		private:
			GameWorld* mWorld;
			static GameLevelManager* manager;
			GameTechRenderer* mRenderer;
			PhysicsSystem* mPhysics;
			Level* mCurrentLevel;

			string mLevelToLoad = "Level1";

			// for handling multiple buttons/boxes in a level
			int boxNumber;
			std::vector<GameObject*> boxes;
			std::vector<Button*> buttons;
			GameObject* pushableBox;
			std::stack<Level*> mLevelStack;
			AnimationController* mAnimator;

			vector<GameObject*> mUpdatableObjectList;

			std::unordered_map<std::string, Mesh*> mMeshList;
			std::unordered_map<std::string, Shader*> mShaderList;
			std::unordered_map<std::string, Texture*> mTextureList;
			std::unordered_map<std::string, MeshAnimation*> mAnimationList;
			std::unordered_map<std::string, MeshMaterial*> mMaterialList;
			std::unordered_map<std::string, vector<int>> mMeshMaterialsList;
			std::map<std::string, MeshAnimation*> mPreLoadedAnimationList;

			GameObject* AddWallToWorld(Vector3 wallSize, const Vector3& position, float x, float y, float z);
			GameObject* AddFloorToWorld(Vector3 size, const Vector3& position);
			GameObject* AddBoxToWorld(const Vector3& position, Vector3 dimensions, const std::string name, float inverseMass = 10.0f);
			GameObject* AddButtonToWorld(Vector3 size, const Vector3& position, const std::string name, Mesh* mesh, Shader* shader, Texture* texture);

			void LogObjectPlacement(const InGameObject& obj);
			void CreateWall(const InGameObject& obj);
			void CreateButton(const InGameObject& obj);
			void CreateBox(const InGameObject& obj);
			void CreateFloor(const InGameObject& obj);

			bool isPlaying;
		};
	}
}