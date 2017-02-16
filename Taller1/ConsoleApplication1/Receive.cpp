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
		
		mutex->lock();
		sf::Socket::Status status = receive->receive(data, 1500, received);
		std::cout << data;
		data[received] = '\0';
		
		mensaje = data;
		//std::cout << data;
		aMensajes->push_back(mensaje);// guarda mensaje a la llista de mensajes
		//if (aMensajes->size() > 25) // si supera el maxim de lines per pantalla, baixala
		//{
		//	aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
		//}
		mutex->unlock();
	}
}
