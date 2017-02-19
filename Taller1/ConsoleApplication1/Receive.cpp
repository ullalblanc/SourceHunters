#include "Receive.h"




Receive::Receive()
{
}


Receive::~Receive()
{
}

void Receive::ReceiveMessages()
{
	char data[1500];
	std::size_t received;
	std::string mensaje = "";
	while (!stopReceive) {	
		sf::Socket::Status status = receive->receive(data, 1500, received);
		data[received] = '\0';
		
		mensaje = data;
		mutex->lock();
		aMensajes->push_back(mensaje);// guarda mensaje a la llista de mensajes
		mutex->unlock();	
	}
}
