#include "Send.h"



Send::Send()
{
}


Send::~Send()
{

}

void Send::SendMessages()
{
	if (send->send(mensajes->c_str(),mensajes->size()) != sf::Socket::Done)
	{
		std::cout << "Error al enviar" << mensajes << std::endl;
	}
}

