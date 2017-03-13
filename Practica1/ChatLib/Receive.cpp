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
	//std::string mensaje = "";
	sf::Socket::Status status = socket->receive(data, 1500, received);
	data[received] = '\0';

	if (status == sf::Socket::Done) {
		*command = data;
		/*mensaje = data;
		aMensajes->push_back(mensaje);// guarda mensaje a la llista de mensajes

		if (aMensajes->size() > 25)
		{
			aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
		}*/
		return true;
	}
	return false;
}