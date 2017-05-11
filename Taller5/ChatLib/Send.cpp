#include "Send.h"

Send::Send()
{
}

Send::~Send()
{
}

void Send::SendMessages(sf::IpAddress ip, unsigned short port, char* command, size_t size)
{
	sf::Socket::Status status;
	status = socket->send(command, size, ip, port);
	//status = socket->send(command->c_str(), command->size(), ip, port);
}