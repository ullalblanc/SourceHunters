#include "Send.h"

Send::Send()
{
}

Send::~Send()
{
}

void Send::SendMessages()
{
	size_t sent;
	sf::Socket::Status status;
	//do {
		status = send->send(command->c_str(), command->size(), sent);
		/*if (status == sf::Socket::Partial)
		{
			command->erase(0, sent);
		}
	} while (status != sf::Socket::Partial);*/
}