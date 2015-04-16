#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include "json/json.h"

namespace RohbotLib
{
	class PacketHandler
	{
	public:
		PacketHandler();
		~PacketHandler();

		void AddHandler(std::string packetType, std::function<void(const Json::Value& root)> handler);
		void RemoveHandler(std::string packetType);

		void HandlePacket(char* packetData, int length);

	private:
		std::unordered_map< std::string, std::function<void(const Json::Value& root)> > m_packetHandlers;
	};
}