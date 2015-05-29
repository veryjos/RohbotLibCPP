#include "websocket.hpp"
#include <chrono>
#include <thread>

#define CONNECT_TIMEOUT 5000

namespace RohbotLib
{
	static void
		log_callback(enum libwebsocket_log_severity severity,
		const char *msg, ...)
	{
		va_list va;
		va_start(va, msg);

		vfprintf(stderr, msg, va);
		fputc('\n', stderr);

		va_end(va);
	}

	static int rohbot_websocket_protocol_callback(struct libwebsocket_context * context, struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len)
	{
		if (wsi == nullptr)
			return 0;

		Websocket* websocketWrapper = (Websocket*)libwebsockets_get_external_user_space(wsi);

		switch (reason)
		{
		case LWS_CALLBACK_CLOSED:
			websocketWrapper->_ClosedByRemote();
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			websocketWrapper->_DeliverData((char*)in, len, libwebsocket_is_final_fragment(wsi) > 0);
			break;

		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			websocketWrapper->_ConnectionEstablished();
			libwebsocket_callback_on_writable(context, wsi);
			break;

		case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
			return false;
			break;

		case LWS_CALLBACK_DEL_POLL_FD:
			websocketWrapper->_ClosedByRemote();
			break;

		case LWS_CALLBACK_ADD_POLL_FD:
		case LWS_CALLBACK_SET_MODE_POLL_FD:
		case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
		case LWS_CALLBACK_CLIENT_WRITEABLE:
			break;

		default:
			break;
		}

		return 0;
	}

	static struct libwebsocket_protocols protocols[] = {
		{
			"rohbot_websocket_protocol",
			rohbot_websocket_protocol_callback,
			0,
		},
		{  /* end of list */
			NULL,
			NULL,
			0
		}
	};

	Websocket::Websocket() : m_context(nullptr), m_socket(nullptr), m_connected(false)
	{

	}

	Websocket::~Websocket()
	{

	}

	void Websocket::Connect(std::string host, std::string endpoint, int port)
	{
		if (m_context || m_socket)
			return;

		//libwebsockets_set_log_callback(log_callback);
		m_context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL, protocols, libwebsocket_internal_extensions,
												nullptr, nullptr, nullptr, -1, -1, 0);

		if (!m_context)
		{
			_Close();

			throw std::exception("Failed to create websocket context.");
		}

		m_socket = libwebsocket_client_connect(m_context, host.c_str(), port, 2,
											   endpoint.c_str(), host.c_str(), host.c_str(),
											   protocols[0].name, -1);

		if (!m_socket)
		{
			_Close();

			throw std::exception("Failed to connect to server.");
		}

		libwebsockets_set_external_user_space(m_socket, this);

		auto connectStartTime = std::chrono::high_resolution_clock::now();

		while (!m_connected && std::chrono::high_resolution_clock::now() - connectStartTime < std::chrono::milliseconds(CONNECT_TIMEOUT))
		{
			libwebsocket_service(m_context, 0);

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		if (!m_connected)
		{
			_Close();

			throw std::exception("Connection to server timed out.");
		}
	}

	void Websocket::Disconnect()
	{
		if (!m_context || !m_socket)
			return;

		_Close();
	}

	void Websocket::Send(const char* data, unsigned int length)
	{
		if (!m_context || !m_socket)
			return;

		if (length == 0)
			return;

		unsigned int paddedLength = LWS_SEND_BUFFER_PRE_PADDING + length + LWS_SEND_BUFFER_POST_PADDING;
		unsigned char* paddedData = (unsigned char*)alloca(paddedLength);

		memcpy(&paddedData[LWS_SEND_BUFFER_PRE_PADDING], data, length);

		libwebsocket_write(m_socket,
			&paddedData[LWS_SEND_BUFFER_PRE_PADDING], length, LWS_WRITE_TEXT);

		libwebsocket_callback_on_writable(m_context, m_socket);
	}

	void Websocket::SendJSON(const Json::Value& root)
	{
		Json::StreamWriterBuilder writer;
		std::string serialized = Json::writeString(writer, root);

		Send(serialized.c_str(), serialized.length());
	}

	void Websocket::SendPacket(const BasePacket& packet)
	{
		SendJSON(packet.GetPacketData());
	}

	bool Websocket::IsConnected()
	{
		return m_connected;
	}

	void Websocket::Poll(std::function<void(char* data, int length)> callback)
	{
		if (!m_context || !m_socket)
			return;

		libwebsocket_service(m_context, 0);

		while (!m_packets.empty())
		{
			Webpacket packet = m_packets.front();
			m_packets.pop();

			callback(packet.data, packet.length);

			free(packet.data);
		}
	}

	void Websocket::_ClosedByRemote()
	{
		m_socket = nullptr;
		_Close();
	}

	void Websocket::_Close()
	{
		if (!m_context)
			return;

		if (m_socket)
			libwebsocket_close_and_free_session(m_context, m_socket, LWS_CLOSE_STATUS_GOINGAWAY);

		libwebsocket_context_destroy(m_context);

		m_socket = nullptr;
		m_context = nullptr;

		m_connected = false;
	}

	void Websocket::_ConnectionEstablished()
	{
		m_connected = true;
	}

	void Websocket::_DeliverData(char* data, int length, bool endOfPacket)
	{
		if (data == nullptr || length == 0)
			return;

		if (m_bufferPacket.data)
		{
			char* expandedDataBuffer = (char*)realloc(m_bufferPacket.data, m_bufferPacket.length + length + 1);
			memcpy(expandedDataBuffer + m_bufferPacket.length, data, length);

			m_bufferPacket.data = expandedDataBuffer;
			m_bufferPacket.length += length;
		}
		else
		{
			m_bufferPacket.data = (char*)malloc(length);
			m_bufferPacket.length = length;

			memcpy(m_bufferPacket.data, data, length);
		}

		if (endOfPacket)
		{
			m_packets.push(m_bufferPacket);

			m_bufferPacket = Webpacket();

			m_bufferPacket.data = nullptr;
			m_bufferPacket.length = 0;
		}
	}
}