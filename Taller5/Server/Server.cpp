//#include <SFML\Graphics.hpp>
//#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <vector>

#include "Game.h"

// 1_0_0_vacio // Hello_index_Jugador_vacio // client vol conectarse
// 2_0_1_vacio // Conexion_Start_Jugador_vacio // server avisa al client que sha conectat y li envia el id

enum State {
	send,	// enviar paraula nova y que comenci partida
	play,	// mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points, // Envia les puntuacions als jugadors y actualitza els seus logs
	win		// el joc sacaba
};

/*
void sendAll(Send* sender, std::vector<sf::TcpSocket*> sockets, bool block = false) { // per misatges iguals que s'envien a tots el jugadors
	for (int i = 0; i < sockets.size(); i++)								
	{
		if (block) sockets[i]->setBlocking(true);
		sender->socket = sockets[i];
		sender->SendMessages();
		if (block) sockets[i]->setBlocking(false);
	}
}*/

int main()
{
	//-- UDP --//

	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1");	// sf::IpAddress::getLocalAddress();
	sf::UdpSocket socket;										// El socket del servidor
	std::queue<sf::IpAddress> ipQueue;							// On es guarden les ips no asignades dels nous jugadors
	std::queue<unsigned short> portQueue;						// On es guarden els ports no asigntas dels nous jugadors
	std::queue<std::string> clientCommands;						// Misatges dels jugadors per anar executant
	sf::Mutex mutex;											// Per evitar varis accesos a les cues
	std::string command;										// el misatge que envia als clients
	Send sender;												// Sender per enviar misatges
	ServerReceive receiver;										// Receiver per rebre constanment misatges
	sf::Thread thread(&Receive::ReceiveCommands, &receiver);	// Thread per el receiver
	std::vector<ServerPlayer> player;							// Vector de jugadors
	ServerPlayer playertmp;

	sf::Socket::Status status = socket.bind(5000);				// Bind al port 5000
	if (status != sf::Socket::Done) {
		std::cout << "Error al intent de bind" << std::endl;
		return -1;
	}
	socket.setBlocking(false);									// Fiquem socketa non Blocking

	sender.command = &command;
	sender.socket = &socket;

	receiver.commands = &clientCommands;
	receiver.socket = &socket;
	receiver.mutex = &mutex;
	//receiver.players = &player;
	receiver.playertmp = &playertmp;

	//-- SERVER --//
	srand(time(NULL));
	MessageManager protocol;
	Timer timer;
	State state = play;		

	//-- GAME --//

	bool gameOn = true;
	thread.launch();

	std::cout << "Server";

	while (gameOn)
	{
		sf::Keyboard key;
		if (key.isKeyPressed(sf::Keyboard::BackSpace)) { // si el server vol tancar la comunicacio
			state = win;
		}

		switch (state) {
		case play:
			// TODO: switch que agafi el misatge de la cua i faci el que toqui. Tindriem que mirar primer el protocol
			if (!clientCommands.empty()) {
				switch (clientCommands.front()[0]) {
				case 1:	// Un client es vol conectar

					if (!player.empty()) { // si no esta buit
						for (int i = 0; i < player.size(); i++)
						{
							if (player[i].port == playertmp.port && player[i].ip == playertmp.ip) {
								command = protocol.CreateMessage(1, player[i].id, player[i].x, player[i].y); // 1_0_0_vacio // WELCOME_id_x_y
								sender.SendMessages(player[player.size() - 1].ip, player[player.size() - 1].port);
							}
							if (player[i].port != playertmp.port && player[i].ip != playertmp.ip) {	// Si no coincideix amb un player existent
								player.push_back(playertmp);
								player[player.size() - 1].id = player.size() - 1;
								player[player.size() - 1].x = rand() % 9;
								player[player.size() - 1].y = rand() % 9;

								command = protocol.CreateMessage(1, player[player.size() - 1].id, player[player.size() - 1].x, player[player.size() - 1].y); // 1_0_0_vacio // WELCOME_id_x_y
								sender.SendMessages(player[player.size() - 1].ip, player[player.size() - 1].port);
							} 
						}
					} else { // si esta buit
						player.push_back(playertmp);
						player[player.size() - 1].id = player.size() - 1;
						player[player.size() - 1].x = rand() % 9;
						player[player.size() - 1].y = rand() % 9;

						command = protocol.CreateMessage(1, player[player.size() - 1].id, player[player.size() - 1].x, player[player.size() - 1].y); // 1_0_0_vacio // WELCOME_id_x_y
						sender.SendMessages(player[player.size() - 1].ip, player[player.size() - 1].port);
					}
					std::cout << "\n New user" << std::endl;
					break;

				}
			}
			break;
		}
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}