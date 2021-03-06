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
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.23.87"); //sf::IpAddress::getLocalAddress();
	//std::vector<sf::TcpSocket*> sockets;
	sf::TcpSocket sockets[4];
	int numUsers = 0;
	//sockets[0] = new sf::TcpSocket;

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
	if (listener.accept(sockets[0]) != sf::Socket::Done) {
		std::cout << "Error al acceptar conexi�" << std::endl;
		return -1;
	}
	numUsers++;
	//listener.close();
	
	// OPEN CHAT WINDOW
	std::vector<std::string> aMensajes;
	receiver.aMensajes = &aMensajes;

	std::string mensaje = "";
	sender.mensajes = &mensaje;
	sockets[0].setBlocking(false);

	//int newSocket = 1;
	//sockets[newSocket] = new sf::TcpSocket;

	bool serverOn = true;
	listener.setBlocking(false);
	while (serverOn)
	{
		if (numUsers < MAX_USERS) {
			if (listener.accept(sockets[numUsers]) == sf::Socket::Done) {
				std::cout << "New user" << std::endl;
				sockets[numUsers].setBlocking(false);
				numUsers++;
			}
		}
	
		sf::Keyboard key;

		for (int i = 0; i < numUsers; i++)
		{
			receiver.socket = &sockets[i];
			if (receiver.ReceiveMessages()) {
				if (aMensajes[aMensajes.size()-1] == "$") {
					sockets[i].disconnect();
					/*for (int j = i; j < numUsers; j++)
					{
						sockets[j] = sockets[j + 1];
					}*/
				}
				else {
					mensaje.clear();
					mensaje = aMensajes[aMensajes.size() - 1];
					for (int j = 0; j < numUsers; j++)
					{
						sender.send = &sockets[j];
						sender.SendMessages();
					}
				}		
			}
		}	
		if (key.isKeyPressed(sf::Keyboard::Escape)) {
			mensaje.clear();
			mensaje = sf::Keyboard::Escape;
			sender.SendMessages(); // envia mensaje
			serverOn = false;
		} 
	}

	//socket.disconnect();
	return 0;
}