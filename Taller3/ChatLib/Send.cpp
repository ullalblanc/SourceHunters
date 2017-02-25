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
		status = send->send(mensajes->c_str(), mensajes->size(), sent);
		if (status != sf::Socket::Done)
		{
			std::cout << "Error al enviar" << mensajes << std::endl;
		}
	} while (status != sf::Socket::Done);

}