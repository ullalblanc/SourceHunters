#include <cstring>
#include <iostream>
#include <time.h>

#include "Game.h"

// 1_i_x_y // WELCOME_id_x_y // Welcome al client, amb la seva id i posicions
// 2_i_x_y // POSITION_id_x_y // Posicio del jugador id 

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

	//-- GAME --//

	sf::Texture texture;
	if (!texture.loadFromFile("../Resources/Fons.jpg")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite sprite; // fons
	sprite.setTexture(texture);

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

	sf::Vector2i screenDimensions(500, 500);											// Dimensions pantalles
	sf::RenderWindow window;															// Creem la finestra del joc
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Taller5");	// Obrim la finestra del joc
	thread.launch();																	// Comencem thread receive

	while (window.isOpen())
	{
		switch (state) {
		case connect:

			if (timerConnect.Check()) {
				command = protocol.CreateMessage(1, 0, 0, 0);
				sender.SendMessages(ip, serverPort);
				timerConnect.Start(2000);
			}
			if (!serverCommands.empty()) {
				switch (protocol.GetType(serverCommands.front())) {

				case 1:
					playertmp.id = protocol.GetSubType(serverCommands.front());
					playertmp.x = protocol.GetFirst(serverCommands.front());
					playertmp.y = protocol.GetSecond(serverCommands.front());

					player.push_back(playertmp);
					state = play;
					serverCommands.pop();
					break;

				default:
					//serverCommands.push(serverCommands.front());
					//serverCommands.pop();
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

				case 2: // Posició de jugador
					for (int i = 0; i < player.size(); i++)
					{
						if (player[i].id == protocol.GetSubType(command)) { // si es un dels jugadors que ja coneix
							player[i].x = protocol.GetFirst(command);
							player[i].y = protocol.GetSecond(command);
							//player[i].keyCommands.push_back(command);
							std::cout << "cookies";
							serverCommands.pop();
							break;
						}
					}
					playertmp.id = protocol.GetSubType(command);
					playertmp.x = protocol.GetFirst(command);
					playertmp.y = protocol.GetSecond(command);
					playertmp.keyCommands.push_back(command);
					serverCommands.pop();
					break;

				default:
					serverCommands.push(serverCommands.front());
					serverCommands.pop();
					break;

				}
			}
			break;

		case points:
			break;
		case win:
			break;
		}
		window.draw(sprite);	// Pintem el fons
		if (player.size() > 0) { 
			blue.setPosition(player[0].x * 25 + 25, player[0].y * 25 + 25);
			window.draw(blue); // pintem el jugador
			if (player.size() > 1) {
				for (int i = 1; i < player.size(); i++)
				{
					fucsia.setPosition(player[i].x * 25 + 25, player[i].y * 25 + 25);
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