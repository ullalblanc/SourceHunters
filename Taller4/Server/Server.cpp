#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>

#include "Send.h"
#include "Receive.h"
#include "Game.h"

#define MAX_MENSAJES 30
#define MAX_USERS 2

enum State {
	send, // enviar paraula nova y que comenci partida
	play, // mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points, // Envia les puntuacions als jugadors y actualitza els seus logs
	win // el joc sacaba
};

std::vector<std::string> words = {
	"anomaly",
	"cattle",
	"pharmacy",
	"composite",
	"sensible",
	"nickname",
	"assured",
	"sacrifice",
	"beer",
	"software"
};

int main()
{
	srand(time(NULL));
	// CHOSE SEVER/CLIENT
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.23.87"); //sf::IpAddress::getLocalAddress();
	std::vector<sf::TcpSocket*> sockets;
	sf::TcpSocket* sockettmp = new sf::TcpSocket;
	
	// Crear players per guardar la info
	std::vector<Player> player;

	std::string textConsole = "Connected to: ";

	std::cout << "Server";
	std::string command; // el misatge que rep per saber que fer
	Send sender;
	Receive receiver;
	receiver.command = &command;
	
	State state = send;
	std::string word;

	sf::TcpListener listener;
	// Escuchamos por el puerto 50000
	if (listener.listen(5000) != sf::Socket::Done) {
		std::cout << "No et pots vincular al port 50000" << std::endl;
		return -1;
	}
	//Accept per els dos jugadors
	if (listener.accept(*sockettmp) != sf::Socket::Done) {
		std::cout << "Error al acceptar conexió" << std::endl;
		return -1;
	}
	std::cout << "\n New user" << std::endl;
	sockettmp->setBlocking(false);
	sockets.push_back(sockettmp);
	sockettmp = new sf::TcpSocket;
	if (listener.accept(*sockettmp) != sf::Socket::Done) {
		std::cout << "Error al acceptar conexió" << std::endl;
		return -1;
	}
	std::cout << "\n New user" << std::endl;
	sockettmp->setBlocking(false);
	sockets.push_back(sockettmp);
	
	//Rebre noms dels jugadors
	int num = 1;
	while (num != 3) {
		for (int i = 0; i < player.size(); i++)
		{
			receiver.socket = sockets[i];
			if (receiver.ReceiveMessages()) {
				// TODO: asignar els noms y els nums als jugadors
				num++;
			}
		}
	}

	// OPEN CHAT WINDOW
	std::vector<std::string> aMensajes;
	//receiver.aMensajes = &aMensajes;

	std::string mensaje = "";
	sender.mensajes = &mensaje;
	bool serverOn = true;

	while (serverOn)
	{
		switch (state) {
		case send:
			// TODO: enviar als dos jugadors
			word = words[rand() % 10 + 1];
			break;
		case play:
			// TODO: rebre paraula dels jugadors i enviarles
			// TODO: rebre paraula comnfirmada i fer check
			// TODO: si s'acaba el temps, tornar a enviar paraula
			break;
		case points:
			// TODO: Si un jugador fa be una paraula, actualitzar i enviar les puntuacions. Si un te 5 punts, acabar partida
			break;
		case win:
			// TODO: acabar partida.
			break;

		}
		sf::Keyboard key;

		/*for (int i = 0; i < sockets.size(); i++)
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
		}*/ 
	}
	for (int i = 0; i < sockets.size(); i++)
	{
		sockets[i]->disconnect();
		delete sockets[i];
	}
	return 0;
}