#include "Send.h"

Send::Send()
{
}

Send::~Send()
{
}

void Send::SendMessages(sf::IpAddress ip, unsigned short port, OutputMemoryBitStream output)
{
	sf::Socket::Status status;
	status = socket->send(output.GetBufferPtr(), output.GetBitLength(), ip, port);
	//status = socket->send(command->c_str(), command->size(), ip, port);
}