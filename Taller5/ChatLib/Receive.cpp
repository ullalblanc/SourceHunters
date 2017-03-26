#include "Receive.h"

Receive::Receive()
{
	stopReceive = true;
}

Receive::~Receive()
{
}

void Receive::ReceiveMessages()
{
	do {
		char data[1500];
		std::size_t received;
		sf::IpAddress ip;
		unsigned short port;

		sf::Socket::Status status = socket->receive(data, 1500, received, ip, port);
		data[received] = '\0';

		if (status == sf::Socket::Done) {
			mutex->lock();
			commands->push(data);
			if (data[0] = '1') { // save ip and port
				ipQueue->push(ip);
				portQueue->push(port);
			}
			mutex->unlock();
		}
	} while (stopReceive);
}