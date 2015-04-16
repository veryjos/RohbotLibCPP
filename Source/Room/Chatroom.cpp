#include "Chatroom.hpp"
#include "../Client/RohbotClient.hpp"
#include "../Packet/ChatPacket.hpp"

namespace RohbotLib
{
	Chatroom::Chatroom(const std::string& name, const std::string& shortname, RohbotClient& client) : m_client(client)
	{
		m_name = name;
		m_shortname = shortname;
	}

	Chatroom::~Chatroom()
	{

	}

	void Chatroom::AddMessage(std::string user, std::string message)
	{
		m_messages.push_back({ user, message });
	}

	const std::vector<Chatroom::ChatroomMessage>& Chatroom::GetMessages() const
	{
		return m_messages;
	}

	void Chatroom::SendChat(const std::string& message)
	{
		ChatPacket packet;

		packet.roomName = m_shortname;
		packet.message = message;

		m_client.m_websocket.SendPacket(packet);
	}

	const std::string& Chatroom::GetName() const
	{
		return m_name;
	}

	const std::string& Chatroom::GetShortname() const
	{
		return m_shortname;
	}

	void Chatroom::UpdateUserlist()
	{
		SendChat("/users");
	}

	void Chatroom::SetUsers(std::vector< std::shared_ptr<RohbotUser> > users)
	{
		m_users = users;
	}

	const std::vector< std::shared_ptr<RohbotUser> >& Chatroom::GetUsers() const
	{
		return m_users;
	}
}