#pragma once

#include "NetworkBase.h"
#include "GameServer.h"
#include "GameClient.h"
#include "GameTechRenderer.h"

namespace NCL {
	namespace CSC8503 {

		class GameConfigManager: public PacketReceiver {
		private:
			struct NetworkConfig {
				bool isMultiplayer = false;
				bool isServer = false;
				std::string ip = "127.0.0.1";
				GameServer* server = nullptr;
				GameClient* client = nullptr;
				int playerID;
			};

		public:
			GameConfigManager();
			~GameConfigManager();

			void InitNetwork();
			void CreateServer();
			void CreateClient();

			void ReceivePacket(int type, GamePacket* packet, int source) override;

			NetworkConfig networkConfig;
		};
	}
}