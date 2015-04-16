#pragma once

#include "json/json.h"
#include <string>

struct BasePacket
{
	virtual Json::Value GetPacketData() const=0;
};