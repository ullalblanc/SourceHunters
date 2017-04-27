#include <cstring>
#include <iostream>
#include <time.h>

#include "Game.h"

// 1_i_x_y // WELCOME_id_x // Welcome al client, amb la seva id i posicions
// 2_i_x_y // POSITION_id_x // Posicio del jugador id
// 3 // Ping
// Protocol: https://docs.google.com/spreadsheets/d/152EPpd8-f7fTDkZwQlh1OCY5kjCTxg6-iZ2piXvEgeg/edit?usp=sharing

enum State {
	connect,	// Per conectarse al servidor
	send,		// enviar paraula nova y que comenci partida
	play,		// mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points,		// Envia les puntuacions als jugadors y actualitza els seus logs
	win			// el joc sacaba
};

int main()
{
	//-- UDP --//

	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1"); //sf::IpAddress::getLocalAddress();
	unsigned short serverPort = 5000;
	sf::UdpSocket socket;
	std::queue<sf::IpAddress> ipQueue;							// On es guarden les ips no asignades dels nous jugadors
	std::queue<unsigned short> portQueue;						// On es guarden els ports no asigntas dels nous jugadors
	std::queue<std::string> serverCommands;						// Misatges del servidor per anar executant
	sf::Mutex mutex;											// Per evitar varis accesos a les cues
	std::string command;										// el misatge que envia als clients
	Send sender;												// Sender per enviar misatges
	ClientReceive receiver;										// Receiver per rebre constanment misatges
	sf::Thread thread(&ClientReceive::ReceiveCommands, &receiver);	// Thread per el receiver
	std::vector<Player> player;	// Vector de jugadors

	sender.command = &command;
	sender.socket = &socket;

	receiver.commands = &serverCommands;
	receiver.socket = &socket;
	receiver.mutex = &mutex;
	receiver.players = &player;

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
	Timer timerConnect;			// timer per intentar conectarse a servidor	
	State state = connect;		// Comencem en connect per que es conecti al server
	Player playertmp;			// Amb el tmp es guardara a ell mateix i als altres en el vector player

	playertmp.x = -50;
	playertmp.y = 750;
	player.push_back(playertmp);
	player.push_back(playertmp);

	//-- GAME --//
	

	//carreguem imatges

	sf::Texture texture;
	if (!texture.loadFromFile("../Resources/Fons.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite fons; // fons
	fons.setTexture(texture);

	sf::Texture texture2;
	if (!texture2.loadFromFile("../Resources/Blue.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite blue; // jugador
	blue.setTexture(texture2);
	blue.setOrigin(20, 20);
	sf::Texture texture3;
	if (!texture3.loadFromFile("../Resources/Fucsia.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite fucsia; // enemic
	fucsia.setTexture(texture3);
	fucsia.setOrigin(20, 20);

	sf::Font font;
	if (!font.loadFromFile("cour.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	sf::Vector2i screenDimensions(1600, 900);											// Dimensions pantalles
	sf::RenderWindow window;															// Creem la finestra del joc
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Aoi Samurai");	// Obrim la finestra del joc
	window.setFramerateLimit(60); //FrameRate

	thread.launch();																	// Comencem thread receive

	while (window.isOpen())
	{
		switch (state) {
		case connect:

			if (timerConnect.Check()) {
				command = protocol.CreateMessage(1, 0, 0, 0);
				sender.SendMessages(ip, serverPort);
				timerConnect.Start(5000);
				timerConnect.Stop();
			}
			if (!serverCommands.empty()) {
				switch (protocol.GetType(serverCommands.front())) {

				case 1:
					player[0].id = protocol.GetSubType(serverCommands.front());
					player[0].x = protocol.GetPosition(serverCommands.front());
					serverCommands.pop();
					break;

				case 2:
					player[1].id = protocol.GetSubType(serverCommands.front());
					player[1].x = protocol.GetPosition(serverCommands.front());
					serverCommands.pop();

					command = protocol.CreateMessage(2, player[0].id, 0, 0);
					sender.SendMessages(ip, serverPort);
					state = play;
					break;

				default:
					break;

				}
			}		
			break;

		case send:
			break;
		case play:

			if (!serverCommands.empty()) {
				switch (protocol.GetType(serverCommands.front())) {

				case 1: // NO TINDRIA QUE REBRE 1
					break;

				case 2: 		
					command = protocol.CreateMessage(2, player[0].id, 0, 0);
					sender.SendMessages(ip, serverPort);
					serverCommands.pop();
					break;

				case 3:
					command = "3" + std::to_string(player[0].id);
					sender.SendMessages(ip, serverPort);
					serverCommands.pop();
				default:
					break;

				}
			}
			break;

		case points:
			break;
		case win:
			break;
		}

		window.draw(fons);	// Pintem el fons

		if (player.size() > 0) { 
			blue.setPosition(player[0].x, player[0].y);
			window.draw(blue); // pintem el jugador
			if (player.size() > 1) {
				for (int i = 1; i < player.size(); i++)
				{
					fucsia.setPosition(player[i].x, player[i].y);
					window.draw(fucsia); // pintem el enemic
				}
			}
		}
		window.display();		// Mostrem per la finestra
		window.clear();			// Netejem finestra
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}