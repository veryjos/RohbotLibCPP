#include "Websocket.hpp"

#include <chrono>
#include <thread>

#define CONNECT_TIMEOUT 5000

namespace RohbotLib
{
	static void
		log_callback(enum lws_log_levels severity,
		const char *msg, ...)
	{
		va_list va;
		va_start(va, msg);

		vfprintf(stderr, msg, va);
		fputc('\n', stderr);

		va_end(va);
	}

	int rohbot_websocket_protocol_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
	{
		if (wsi == nullptr)
			return 0;

		Websocket* websocketWrapper = (Websocket*)user;

		switch (reason)
		{
		case LWS_CALLBACK_ESTABLISHED:
			printf("LWS_CALLBACK_ESTABLISHED\n");
			break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			printf("LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
			break;

		case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
			printf("LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH\n");
			break;

		case LWS_CALLBACK_CLOSED:
			printf("LWS_CALLBACK_CLOSED\n");
			websocketWrapper->_ClosedByRemote();
			break;

		case LWS_CALLBACK_CLOSED_HTTP:
			printf("LWS_CALLBACK_CLOSED_HTTP\n");
			break;

		case LWS_CALLBACK_RECEIVE:
			printf("LWS_CALLBACK_RECEIVE\n");
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
			printf("LWS_CALLBACK_CLIENT_RECEIVE_PONG\n");
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			printf("LWS_CALLBACK_CLIENT_RECEIVE\n");
			websocketWrapper->_DeliverData((char*)in, len, lws_is_final_fragment(wsi) > 0);
			break;

		case LWS_CALLBACK_HTTP:
			printf("LWS_CALLBACK_HTTP\n");
			break;

		case LWS_CALLBACK_HTTP_BODY:
			printf("LWS_CALLBACK_HTTP_BODY\n");
			break;

		case LWS_CALLBACK_HTTP_BODY_COMPLETION:
			printf("LWS_CALLBACK_HTTP_BODY_COMPLETION\n");
			break;

		case LWS_CALLBACK_HTTP_WRITEABLE:
			printf("LWS_CALLBACK_HTTP_WRITEABLE\n");
			break;

		case LWS_CALLBACK_HTTP_FILE_COMPLETION:
			printf("LWS_CALLBACK_HTTP_FILE_COMPLETION\n");
			break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
			printf("LWS_CALLBACK_SERVER_WRITEABLE\n");
			break;

		case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
			printf("LWS_CALLBACK_FILTER_NETWORK_CONNECTION\n");
			break;

		case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
			printf("LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS\n");
			break;

		case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS:
			printf("LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS\n");
			break;

		case LWS_CALLBACK_OPENSSL_CONTEXT_REQUIRES_PRIVATE_KEY:
			printf("LWS_CALLBACK_OPENSSL_CONTEXT_REQUIRES_PRIVATE_KEY\n");
			break;

		case LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			printf("LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION\n");
			break;

		case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
			printf("LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER\n");
			break;

		case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:
			printf("LWS_CALLBACK_CONFIRM_EXTENSION_OKAY\n");
			break;

		case LWS_CALLBACK_PROTOCOL_INIT:
			printf("LWS_CALLBACK_PROTOCOL_INIT\n");
			break;

		case LWS_CALLBACK_PROTOCOL_DESTROY:
			printf("LWS_CALLBACK_PROTOCOL_DESTROY\n");
			break;

		case LWS_CALLBACK_WSI_CREATE:
			printf("LWS_CALLBACK_WSI_CREATE\n");
			break;

		case LWS_CALLBACK_WSI_DESTROY:
			printf("LWS_CALLBACK_WSI_DESTROY\n");
			break;

		case LWS_CALLBACK_ADD_POLL_FD:
			printf("LWS_CALLBACK_ADD_POLL_FD\n");
			break;

		case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			printf("LWS_CALLBACK_CHANGE_MODE_POLL_FD\n");
			break;

		case LWS_CALLBACK_UNLOCK_POLL:
			printf("LWS_CALLBACK_UNLOCK_POLL\n");
			break;

		case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
			printf("LWS_CALLBACK_WS_PEER_INITIATED_CLOSE\n");
			break;

		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			printf("LWS_CALLBACK_CLIENT_ESTABLISHED\n");
			websocketWrapper->_ConnectionEstablished();
			lws_callback_on_writable(wsi);
			break;

		case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
			printf("LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED\n");
			return false;
			break;

		case LWS_CALLBACK_DEL_POLL_FD:
			printf("LWS_CALLBACK_DEL_POLL_FD\n");
			websocketWrapper->_ClosedByRemote();
			break;

		default:
			printf("unknown what\n");
			break;
		}

		return 0;
	}

	static struct lws_protocols protocols[] = {
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

		lws_context_creation_info info;
		memset((void*)&info, 0, sizeof(info));

		info.port = CONTEXT_PORT_NO_LISTEN;
		info.protocols = protocols;
		info.gid = -1;
		info.uid = -1;
		info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

		m_context = lws_create_context(&info);

		if (!m_context)
		{
			_Close();

			throw std::runtime_error("Failed to create websocket context.");
		}

		lws_client_connect_info cinfo;
		memset((void*)&cinfo, 0, sizeof(cinfo));

		cinfo.context = m_context;
		cinfo.address = host.c_str();
		cinfo.port = port;
		cinfo.ssl_connection = 2;
		cinfo.path = endpoint.c_str();
		cinfo.host = host.c_str();
		cinfo.origin = host.c_str();
		cinfo.protocol = protocols[0].name;
		cinfo.ietf_version_or_minus_one = -1;
		cinfo.userdata = this;
		cinfo.client_exts = nullptr;
		cinfo.method = nullptr;

		m_socket = lws_client_connect_via_info(&cinfo);

		if (!m_socket)
		{
			_Close();

			throw std::runtime_error("Failed to connect to server.");
		}

		auto connectStartTime = std::chrono::high_resolution_clock::now();

		while (!m_connected && std::chrono::high_resolution_clock::now() - connectStartTime < std::chrono::milliseconds(CONNECT_TIMEOUT))
		{
			lws_service(m_context, 0);

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		if (!m_connected)
		{
			_Close();

			throw std::runtime_error("Connection to server timed out.");
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

		lws_write(m_socket,
			&paddedData[LWS_SEND_BUFFER_PRE_PADDING], length, LWS_WRITE_TEXT);
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

		lws_service(m_context, 0);

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

		lws_context_destroy(m_context);

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
