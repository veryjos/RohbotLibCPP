#pragma once

#include "BasePacket.hpp"

struct ChatPacket : public BasePacket
{
	std::string roomName;
	std::string message;

	virtual Json::Value GetPacketData() const
	{
		Json::Value data;

		data["Type"] = "sendMessage";

		data["Target"] = roomName;
		data["Content"] = message;

		return data;
	}
};