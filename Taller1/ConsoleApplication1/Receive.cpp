#include "Receive.h"



Receive::Receive()
{
}


Receive::~Receive()
{
}

void Receive::ReceiveMessages()
{
	char data[100];
	std::size_t received;
	sf::Socket::Status status = receive->receive(data, 100, received);
	aMensajes->push_back(data);// guarda mensaje a la llista de mensajes
	if (aMensajes->size() > 25) // si supera el maxim de lines per pantalla, baixala
	{
		aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
	}
}
