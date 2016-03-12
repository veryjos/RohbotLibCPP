#include "PacketHandler.hpp"

namespace RohbotLib
{
	PacketHandler::PacketHandler()
	{
	}

	PacketHandler::~PacketHandler()
	{
	}

	void PacketHandler::AddHandler(std::string packetType, std::function<void(const Json::Value& root)> handler)
	{
		auto itr = m_packetHandlers.find(packetType);

		if (itr == m_packetHandlers.end())
			m_packetHandlers[packetType] = handler;
		else
			throw std::runtime_error("Packet type already registered.");
	}

	void PacketHandler::RemoveHandler(std::string packetType)
	{
		auto itr = m_packetHandlers.find(packetType);

		if (itr != m_packetHandlers.end())
			m_packetHandlers.erase(itr);
	}

	void PacketHandler::HandlePacket(char* packetData, int length)
	{
		Json::Value root;
		Json::Reader reader;

		reader.parse(packetData, &packetData[length], root, false);

		Json::Value packetType = root.get("Type", "");
		if (packetType != "")
		{
			std::string key = packetType.asString();

			auto itr = m_packetHandlers.find(key);
			if (itr != m_packetHandlers.end())
				itr->second(root);
			else
			{
				packetData[length] = '\0';
				//printf("Unknown packet type: %s\n\n%s\n", key.c_str(), packetData);
			}
		}
	}
}
