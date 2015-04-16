#pragma once

#include "BasePacket.hpp"

struct AuthPacket : public BasePacket
{
	std::string method;
	std::string username;
	std::string password;

	virtual Json::Value GetPacketData() const
	{
		Json::Value data;
		
		data["Type"] = "auth";

		data["method"] = method;
		data["Username"] = username;
		data["Password"] = password;
		
		return data;
	}
};