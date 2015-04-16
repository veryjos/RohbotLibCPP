#pragma once

#include <string>

namespace RohbotLib
{
	class RohbotUser
	{
		friend class RohbotClient;

	public:
		~RohbotUser();
		const std::string& GetUsername() const;

	private:
		RohbotUser(std::string username);

		std::string m_username;
	};
}