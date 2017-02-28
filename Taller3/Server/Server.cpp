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
	//sf::TcpSocket sockets[4];
	//int numUsers = 0;
	sf::TcpSocket* sockettmp = new sf::TcpSocket;
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
	if (listener.accept(*sockettmp) != sf::Socket::Done) {
		std::cout << "Error al acceptar conexió" << std::endl;
		return -1;
	}
	sockettmp->setBlocking(false);
	sockets.push_back(sockettmp);
	sockettmp = new sf::TcpSocket;
	
	//numUsers++;
	//listener.close();
	
	// OPEN CHAT WINDOW
	std::vector<std::string> aMensajes;
	receiver.aMensajes = &aMensajes;

	std::string mensaje = "";
	sender.mensajes = &mensaje;
	

	//int newSocket = 1;
	//sockets[newSocket] = new sf::TcpSocket;

	bool serverOn = true;
	listener.setBlocking(false);
	while (serverOn)
	{
		if (sockets.size() < MAX_USERS) {
			if (listener.accept(*sockettmp) == sf::Socket::Done) {
				std::cout << "New user" << std::endl;
				sockettmp->setBlocking(false); 
				sockets.push_back(sockettmp);
				sockettmp = new sf::TcpSocket;
			}
		}
	
		sf::Keyboard key;

		for (int i = 0; i < sockets.size(); i++)
		{
			receiver.socket = sockets[i];
			if (receiver.ReceiveMessages()) {
				if (aMensajes[aMensajes.size()-1] == "$") {
					sockets[i]->disconnect();
					std::cout << "User leaved" << std::endl;
					delete sockets[i];
					sockets.erase(sockets.begin() + i);
				}
				else {
					mensaje.clear();
					mensaje = aMensajes[aMensajes.size() - 1];
					for (int j = 0; j < sockets.size(); j++)
					{
						sender.send = sockets[j];
						sender.SendMessages();
					}
				}		
			}
		}	
		if (key.isKeyPressed(sf::Keyboard::BackSpace)) {
			mensaje.clear();
			mensaje = sf::Keyboard::Escape;
			for (int i = 0; i < sockets.size(); i++)
			{
				sender.send = sockets[i];
				sender.SendMessages();
			}
			serverOn = false;
		} 
	}
	for (int i = 0; i < sockets.size(); i++)
	{
		sockets[i]->disconnect();
		delete sockets[i];
	}
	//socket.disconnect();
	return 0;
}