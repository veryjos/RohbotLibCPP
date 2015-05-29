#include "RohbotClient.hpp"

#include "../Packet/AuthPacket.hpp"
#include "../Packet/ChatPacket.hpp"

#include <chrono>
#include <thread>

#define CONNECT_TIMEOUT 500000

namespace RohbotLib
{
	RohbotClient::RohbotClient()
	{
		m_packetHandler.AddHandler("ping", [&](const Json::Value &root)
		{
			//Rohbot doesn't expect a response for pings
		});

		m_packetHandler.AddHandler("authResponse", [&](const Json::Value &root)
		{
			bool success = root.get("Success", Json::Value::null).asBool();

			m_authenticated = success;

			if (m_authenticated)
			{
				std::string username = root.get("Name", Json::Value::null).asString();

				m_user = std::shared_ptr<RohbotUser>(new RohbotUser(username));

				if (m_loggedInCallback)
					m_loggedInCallback(*m_user.get());
			}
			else
				throw std::exception("Auth failed!");
		});

		m_packetHandler.AddHandler("sysMessage", [&](const Json::Value &root)
		{
			std::string content = root.get("Content", Json::Value::null).asString();

			if (m_systemMessageReceivedCallback)
				m_systemMessageReceivedCallback(content);

			GetPrimaryChatroom().lock()->AddMessage("::rohbot::", content);
		});

		m_packetHandler.AddHandler("message", [&](const Json::Value &root)
		{
			Json::Value line = root.get("Line", Json::Value::null);
			std::string lineType = line.get("Type", Json::Value::null).asString();

			if (lineType == "chat")
			{
				std::string sender = line.get("Sender", Json::Value::null).asString();
				std::string content = line.get("Content", Json::Value::null).asString();
				std::string chatroomName = line.get("Chat", Json::Value::null).asString();

				std::shared_ptr<Chatroom> chatroom = GetChatroomByName(chatroomName).lock();
				(chatroom.get())->AddMessage(sender, content);

				if (m_messageReceivedCallback)
					m_messageReceivedCallback(RohbotUser(sender), *chatroom.get(), content);
			}
			else if (lineType == "state")
			{
				std::string state = line.get("State", Json::Value::null).asString();
				std::string username = line.get("For", Json::Value::null).asString();
				std::string chatroomName = line.get("Chat", Json::Value::null).asString();

				std::shared_ptr<Chatroom> chatroom = GetChatroomByName(chatroomName).lock();

				if (state == "Disconnected")
				{
					if (m_otherLoggedOutCallback)
						m_otherLoggedOutCallback(RohbotUser(username));

					(chatroom.get())->AddMessage("::rohbot::", username + " left chat.");
					m_primaryChatroom.lock()->UpdateUserlist();
				}
				else if (state == "Enter")
				{
					if (m_otherLoggedInCallback)
						m_otherLoggedInCallback(RohbotUser(username));

					(chatroom.get())->AddMessage("::rohbot::", username + " entered chat.");
					m_primaryChatroom.lock()->UpdateUserlist();
				}
			}
		});

		m_packetHandler.AddHandler("chat", [&](const Json::Value &root)
		{
			std::string method = root.get("Method", Json::Value::null).asString();

			if (method == "join")
			{
				std::string name = root.get("Name", Json::Value::null).asString();
				std::string shortname = root.get("ShortName", Json::Value::null).asString();

				m_chatrooms.push_back(std::shared_ptr<Chatroom>(new Chatroom(name, shortname, *this)));
				m_primaryChatroom = m_chatrooms.back();

				m_primaryChatroom.lock()->UpdateUserlist();
			}
		});

		m_packetHandler.AddHandler("userList", [&](const Json::Value &root)
		{
			std::string shortname = root.get("ShortName", Json::Value::null).asString();
			Json::Value userArray = root.get("Users", Json::Value::null);

			std::vector< std::shared_ptr<RohbotUser> > users;

			for (Json::Value user : userArray)
			{
				std::string username = user.get("Name", Json::Value::null).asString();

				users.push_back(std::shared_ptr<RohbotUser>(new RohbotUser(username)));
			}

			std::shared_ptr<Chatroom> chatroom = GetChatroomByName(shortname).lock();
			(chatroom.get())->SetUsers(users);
		});
	}

	RohbotClient::~RohbotClient()
	{
	}

	void RohbotClient::Connect(std::string host, std::string endpoint, int port, std::string username, std::string password)
	{
		m_websocket.Connect(host, endpoint, port);

		AuthPacket authPacket;

		authPacket.method = "login";
		authPacket.username = username;
		authPacket.password = password;

		m_websocket.SendPacket(authPacket);

		auto startTime = std::chrono::high_resolution_clock::now();

		while (!m_websocket.IsConnected() && std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(CONNECT_TIMEOUT))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		while (!m_authenticated && std::chrono::high_resolution_clock::now() - startTime < std::chrono::milliseconds(CONNECT_TIMEOUT))
		{
			m_websocket.Poll(
				[&](char* data, int length)
				{
					m_packetHandler.HandlePacket(data, length);
				}
			);

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		if (!m_websocket.IsConnected())
			throw std::exception("Connection timed out.");
	}

	void RohbotClient::Disconnect()
	{
		m_websocket.Disconnect();
	}

	void RohbotClient::SetSystemMessageReceivedCallback(std::function < void(std::string message) > callback)
	{
		m_systemMessageReceivedCallback = callback;
	}

	void RohbotClient::SetMessageReceivedCallback(std::function < void(const RohbotUser& sender, const Chatroom& chatroom, std::string message) > callback)
	{
		m_messageReceivedCallback = callback;
	}

	void RohbotClient::SetLoggedInCallback(std::function < void(const RohbotUser& user) > callback)
	{
		m_loggedInCallback = callback;
	}

	void RohbotClient::SetDisconnectedCallback(std::function < void(const RohbotUser& user) > callback)
	{
		m_disconnectedCallback = callback;
	}

	void RohbotClient::SetOtherLoggedInCallback(std::function < void(const RohbotUser& user) > callback)
	{
		m_otherLoggedInCallback = callback;
	}

	void RohbotClient::SetOtherLoggedOutCallback(std::function < void(const RohbotUser& user) > callback)
	{
		m_otherLoggedOutCallback = callback;
	}

	void RohbotClient::SetJoinedChatroomCallback(std::function < void(const Chatroom& user) > callback)
	{
		m_joinChatroomCallback = callback;
	}

	void RohbotClient::SetLeftChatroomCallback(std::function < void(const Chatroom& user) > callback)
	{
		m_leftChatroomCallback = callback;
	}

	bool RohbotClient::LoggedIn()
	{
		return m_authenticated;
	}

	std::weak_ptr<RohbotUser> RohbotClient::GetUser() const
	{
		return m_user;
	}

	const std::weak_ptr<Chatroom> RohbotClient::GetChatroomByName(std::string shortname) const
	{
		auto itr = std::find_if(m_chatrooms.begin(), m_chatrooms.end(), [&](const std::shared_ptr<Chatroom> chatroom)
			{
				return chatroom->GetShortname() == shortname;
			});

		return itr != m_chatrooms.end() ? *itr : std::weak_ptr<Chatroom>();
	}

	void RohbotClient::SetPrimaryChatroomDelta(int delta)
	{
		if (delta == 0)
			return;

		auto T = std::find(m_chatrooms.begin(), m_chatrooms.end(), m_primaryChatroom.lock());
		int index = T - m_chatrooms.begin();
		
		int direction = (delta > 0 ? 1 : -1);

		delta = abs(delta);
		while (delta > 0)
		{
			index += direction;
			if (index == m_chatrooms.size())
				index = 0;
			else if (index < 0)
				index = m_chatrooms.size() - 1;

			delta--;
		}

		SetPrimaryChatroom(*(m_chatrooms.begin() + index));
	}

	void RohbotClient::SetPrimaryChatroom(std::shared_ptr<Chatroom> chatroom)
	{
		m_primaryChatroom = chatroom;
	}

	const std::weak_ptr<Chatroom> RohbotClient::GetPrimaryChatroom() const
	{
		return m_primaryChatroom;
	}

	const std::vector< std::shared_ptr<Chatroom> >& RohbotClient::GetChatrooms() const
	{
		return m_chatrooms;
	}

	void RohbotClient::Think()
	{
		if (!m_websocket.IsConnected())
			throw std::exception("Not connected.");

		if (!m_authenticated)
			throw std::exception("Not authenticated.");

		m_websocket.Poll([&](char* data, int length)
		{
			m_packetHandler.HandlePacket(data, length);
		});
	}
}