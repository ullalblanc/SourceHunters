#include "Receive.h"

Receive::Receive()
{
}

Receive::~Receive()
{
}

bool Receive::ReceiveMessages()
{
	char data[1500];
	std::size_t received;
	sf::Socket::Status status = socket->receive(data, 1500, received);
	data[received] = '\0';

	if (status == sf::Socket::Done) {
		*command = data;
		return true;
	}
	return false;
}