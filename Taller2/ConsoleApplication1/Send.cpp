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
	sf::Socket::Status status = send->send(mensajes->c_str(), mensajes->size(), sent);
	if (status == sf::Socket::Error)
	{
		std::cout << "Error al enviar" << mensajes << std::endl;
	}
	else if (status == sf::Socket::Partial) {
				
	}
}

