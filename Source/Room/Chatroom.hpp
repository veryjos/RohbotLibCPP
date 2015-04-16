#pragma once

#include <vector>
#include <string>
#include <memory>

namespace RohbotLib
{
	class RohbotUser;

	class Chatroom
	{
		friend class RohbotClient;

	public:
		virtual ~Chatroom();

		struct ChatroomMessage
		{
			std::string user;
			std::string text;
		};

		const std::vector<ChatroomMessage>& GetMessages() const;

		void SendChat(const std::string& message);

		const std::string& GetName() const;
		const std::string& GetShortname() const;

		void UpdateUserlist();

		const std::vector< std::shared_ptr<RohbotUser> >& GetUsers() const;

	private:
		Chatroom(const std::string& name, const std::string& shortname, RohbotClient& client);

		RohbotClient& m_client;

		std::string m_name;
		std::string m_shortname;

		void AddMessage(std::string user, std::string message);
		void SetUsers(std::vector< std::shared_ptr<RohbotUser> > users);

		std::vector<ChatroomMessage> m_messages;
		std::vector< std::shared_ptr<RohbotUser> > m_users;
	};
}