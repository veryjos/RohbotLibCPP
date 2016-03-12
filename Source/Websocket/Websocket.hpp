#pragma once

#include <queue>
#include <string>
#include "libwebsockets.h"

#include "Packet/BasePacket.hpp"

#include <functional>
#include "json/json.h"

namespace RohbotLib
{
	class Websocket
	{
		friend int rohbot_websocket_protocol_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

	public:
		Websocket();
		~Websocket();

		void Connect(std::string host, std::string endpoint, int port);
		void Disconnect();

		void Send(const char* data, unsigned int length);
		void SendJSON(const Json::Value& root);
		void SendPacket(const BasePacket& packet);

		bool IsConnected();

		void Poll(std::function<void(char* data, int length)> callback);

	private:
		struct Webpacket
		{
			Webpacket()
			{
				data = nullptr;
				length = 0;
			}

			char* data;
			int length;
		};

		void _ClosedByRemote();
		void _Close();
		void _DeliverData(char* data, int length, bool endOfPacket);
		void _ConnectionEstablished();

		Webpacket m_bufferPacket;

		lws_context* m_context;
		lws* m_socket;

		bool m_connected;

		std::queue<Webpacket> m_packets;
	};

}
