#include "RohbotUser.hpp"

namespace RohbotLib
{
	RohbotUser::RohbotUser(std::string username) : m_username(username)
	{
	}

	RohbotUser::~RohbotUser()
	{
	}

	const std::string& RohbotUser::GetUsername() const
	{
		return m_username;
	}
}