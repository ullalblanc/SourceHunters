#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <vector>

#include "Send.h"
#include "Receive.h"

#define MAX_MENSAJES 30
#define MAX_USERS 4

int main()
{
	// CHOSE SEVER/CLIENT
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.1.11"); //sf::IpAddress::getLocalAddress();
	std::vector<sf::TcpSocket*> sockets;
	sockets[0] = new sf::TcpSocket;

	std::size_t received;
	std::string textConsole = "Connected to: ";

	std::cout << "Server";
	Send sender;
	//sender.send = &sockets;
	Receive receiver;
	//receiver.receive = &sockets;

	sf::TcpListener listener;
	// Escuchamos por el puerto 50000
	if (listener.listen(5000) != sf::Socket::Done) {
		std::cout << "No et pots vincular al port 50000" << std::endl;
		return -1;
	}
	// puerto 50000 al socket send
	if (listener.accept(*sockets[0]) != sf::Socket::Done) {
		std::cout << "Error al acceptar conexió" << std::endl;
		return -1;
	}
	//listener.close();
	
	// OPEN CHAT WINDOW
	std::vector<std::string> aMensajes;
	receiver.aMensajes = &aMensajes;

	std::string mensaje = "";

	sockets[0]->setBlocking(false);

	int newSocket = 1;
	sockets[newSocket] = new sf::TcpSocket;

	bool serverOn = true;
	while (serverOn)
	{
		if (sockets.size() < MAX_USERS) {
			if (listener.accept(*sockets[newSocket]) == sf::Socket::Done) {
				std::cout << "New user" << std::endl;
				newSocket++;
				sockets[newSocket] = new sf::TcpSocket;
			}
		}
		sf::Event event;

		for (int i = 0; i < sockets.size(); i++)
		{
			receiver.socket = sockets[i];
			if (receiver.ReceiveMessages()) {
				mensaje = aMensajes[aMensajes.size()];
				for (int j = 0; j < sockets.size(); j++)
				{
					sender.send = sockets[j];
					sender.SendMessages();
				}
			}
		}	
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				mensaje.clear();
				mensaje = sf::Keyboard::Escape;
				sender.SendMessages(); // envia mensaje
				serverOn = false;
			}
		}
		aMensajes.push_back(mensaje);
	}		
	//socket.disconnect();
	return 0;
}