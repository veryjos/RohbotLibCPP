# RohbotLibCPP
Library for interfacing with [RohBot](https://github.com/Rohansi/RohBot) in C++

## Dependencies
  * [libwebsockets](https://github.com/davidgaleano/libwebsockets)

## Usage example
```c++
#include "RohbotLib/Client/RohbotClient.hpp"

using namespace RohbotLib;

int main(char** argv, int argc)
{
	if (argc != 3)
	{
		printf("Usage:\nrohbotexample <username> <password>\n");
		return 0;
	}

	std::string username;
	std::string password;

	username = argv[1];
	password = argv[2];

	RohbotClient client;

	client.SetLoggedInCallback([&](const RohbotUser &user)
	{
		printf("Logged in as %s\n", user.GetUsername().c_str());
	});

	client.SetOtherLoggedInCallback([&](const RohbotUser &user)
	{
		printf("%s entered chat.\n", user.GetUsername().c_str());
	});

	client.SetOtherLoggedOutCallback([&](const RohbotUser &user)
	{
		printf("%s disconnected.\n", user.GetUsername().c_str());
	});

	client.SetMessageReceivedCallback([&](const RohbotUser &sender, const Chatroom& chatroom, const std::string message)
	{
		printf("Message from %s to room %s:\n%s\n", sender.GetUsername(), chatroom.GetName(), message.c_str());
	});

	client.SetSystemMessageReceivedCallback([&](std::string message)
	{
		printf("::rohbot_system::%s\n", message.c_str());
	});

	try
	{
		client.Connect("fpp.literallybrian.com", "/ws/", 443, username, password);
	}

	catch (std::exception exception)
	{
		printf("Failed to connect to Rohbot");
		continue;
	}

	while (client.LoggedIn())
	{
		client.Think();

		Sleep(50);
	}
}
```