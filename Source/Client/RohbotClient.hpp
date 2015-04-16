#pragma once

#include <string>
#include <functional>
#include <memory>
#include <vector>

#include "../Packet/PacketHandler.hpp"
#include "../Websocket/Websocket.hpp"

#include "../User/RohbotUser.hpp"

#include "../Room/Chatroom.hpp"

namespace RohbotLib
{
	class RohbotClient
	{
		friend class Chatroom;

	public:
		RohbotClient();
		virtual ~RohbotClient();

		void Connect(std::string host, std::string endpoint, int port, std::string username, std::string password);
		void Disconnect();

		void Think();

		void SetSystemMessageReceivedCallback(std::function < void(std::string message) > callback);
		void SetMessageReceivedCallback(std::function < void(const RohbotUser& sender, const Chatroom& chatroom, std::string message) > callback);

		void SetLoggedInCallback(std::function < void(const RohbotUser& user) > callback);
		void SetDisconnectedCallback(std::function < void(const RohbotUser& user) > callback);

		void SetOtherLoggedInCallback(std::function < void(const RohbotUser& user) > callback);
		void SetOtherLoggedOutCallback(std::function < void(const RohbotUser& user) > callback);

		void SetJoinedChatroomCallback(std::function < void(const Chatroom& user) > callback);
		void SetLeftChatroomCallback(std::function < void(const Chatroom& user) > callback);

		bool LoggedIn();

		const std::weak_ptr<Chatroom> GetChatroomByName(std::string shortname) const;

		void SetPrimaryChatroomDelta(int delta);
		void SetPrimaryChatroom(std::shared_ptr<Chatroom> chatroom);
		const std::weak_ptr<Chatroom> GetPrimaryChatroom() const;

		const std::vector< std::shared_ptr<Chatroom> >& GetChatrooms() const;

		std::weak_ptr<RohbotUser> GetUser() const;

	private:
		std::shared_ptr<RohbotUser> m_user;

		std::function < void(std::string message) > m_systemMessageReceivedCallback;
		std::function < void(const RohbotUser& sender, const Chatroom& chatroom, std::string message) > m_messageReceivedCallback;

		std::function < void(const RohbotUser& user) > m_loggedInCallback;
		std::function < void(const RohbotUser& user) > m_disconnectedCallback;

		std::function < void(const RohbotUser& user) > m_otherLoggedInCallback;
		std::function < void(const RohbotUser& user) > m_otherLoggedOutCallback;

		std::function < void(const Chatroom& room) > m_joinChatroomCallback;
		std::function < void(const Chatroom& room) > m_leftChatroomCallback;

		bool m_authenticated;

		Websocket m_websocket;
		PacketHandler m_packetHandler;

		std::weak_ptr<Chatroom> m_primaryChatroom;
		std::vector< std::shared_ptr<Chatroom> > m_chatrooms;
	};
}