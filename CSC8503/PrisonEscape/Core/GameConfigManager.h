#pragma once

#include "GameServer.h"
#include "GameClient.h"
#include "GameTechRenderer.h"

namespace NCL {
	namespace CSC8503 {

		class GameConfigManager {
		private:
			struct NetworkConfig {
				bool isMultiplayer = false;
				bool isServer = false;
				std::string ip = "127.0.0.1";
				GameServer* server = nullptr;
				GameClient* client = nullptr;
				bool isUsingSteam = false;
				int playerID;
			};

		public:
			GameConfigManager();
			~GameConfigManager();

			void InitNetwork();
			void CreateServer();
			void CreateClient();

			NetworkConfig networkConfig;

			void SetSteamCallback();
			std::function<void(uint64_t)> steamInviteCallback;
		};
	}
}