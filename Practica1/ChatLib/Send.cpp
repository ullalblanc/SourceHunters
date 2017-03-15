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
	do {
		status = send->send(command->c_str(), command->size(), sent);
		if (status != sf::Socket::Done)
		{
			std::cout << "Error al enviar" << command << std::endl;
		}
	} while (status != sf::Socket::Done);
	//*command = " ";
}