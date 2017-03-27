//#include <SFML\Graphics.hpp>
//#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <time.h>

#include "Game.h"

enum State {
	connect,	// Per conectarse al servidor
	send,		// enviar paraula nova y que comenci partida
	play,		// mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points,		// Envia les puntuacions als jugadors y actualitza els seus logs
	win			// el joc sacaba
};

void cleanPlayers(int* playerChecks, int size = MAX_USERS) {
	for (int i = 0; i < size; i++)
	{
		playerChecks[i] = 0;
	}
}

bool playersReady(int* playerChecks, int size = MAX_USERS) {
	for (int i = 0; i < size; i++)
	{
		if (playerChecks[i] == 0) {
			return false;
		}
	}
	return true;
}

int main()
{
	//-- UDP --//

	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1"); //sf::IpAddress::getLocalAddress();
	sf::UdpSocket socket;
	std::queue<sf::IpAddress> ipQueue;							// On es guarden les ips no asignades dels nous jugadors
	std::queue<unsigned short> portQueue;						// On es guarden els ports no asigntas dels nous jugadors
	std::queue<std::string> serverCommands;						// Misatges del servidor per anar executant
	sf::Mutex mutex;											// Per evitar varis accesos a les cues
	std::string command;										// el misatge que envia als clients
	Send sender;												// Sender per enviar misatges
	Receive receiver;											// Receiver per rebre constanment misatges
	sf::Thread thread(&Receive::ReceiveMessages, &receiver);	// Thread per el receiver

	sender.command = &command;
	sender.socket = &socket;

	receiver.commands = &serverCommands;
	receiver.socket = &socket;
	receiver.ipQueue = &ipQueue;
	receiver.portQueue = &portQueue;
	receiver.mutex = &mutex;

	std::cout << "Port: ";										// Demanem port al client
	unsigned short port;
	std::cin >> port;											// Guardem el port del client
	sf::Socket::Status status = socket.bind(port);				// Bind al port del client
	if (status != sf::Socket::Done) {							// Si falla el bind, acaba el programa
		std::cout << "Error al intent de bind" << std::endl;
		return -1;
	}

	//-- CLIENT --//

	MessageManager protocol;	// Per llegir els segons el protocol
	Timer timer;			
	State state = connect;		// Comencem en connect per que es conecti al server
	std::vector<Player> player;	// Vector de jugadors
	Player playertmp;			// Amb el tmp es guardara a ell mateix i als altres en el vector player

	//-- GAME --//

	sf::Texture background;
	if (!background.loadFromFile("../Resources/Fons.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite sprite;
	sprite.setTexture(background);

	sf::Font font;
	if (!font.loadFromFile("cour.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	sf::Vector2i screenDimensions(918, 516);										// Dimensions pantalles
	sf::RenderWindow window;														// Creem la finestra del joc
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Buzz");	// Obrim la finestra del joc
	thread.launch();																// Comencem thread receive

	while (window.isOpen())
	{
		switch (state) {
		case connect:
			break;
		case send:
			break;
		case play:
			break;
		case points:
			break;
		case win:
			break;
		}

		window.draw(sprite);	// Pintem el fons
		window.display();		// Mostrem per la finestra
		window.clear();			// Netejem finestra
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}