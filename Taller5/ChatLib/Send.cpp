#include "Send.h"

Send::Send()
{
}

Send::~Send()
{
}

void Send::SendMessages(sf::IpAddress ip, unsigned short port)
{
	sf::Socket::Status status;
	//do {
		status = socket->send(command->c_str(), command->size(), ip, port);
	//} while (status != sf::Socket::Done);
}