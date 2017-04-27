//#include <SFML\Graphics.hpp>
//#include <SFML\Network.hpp>
#include <cstring>

#include "Game.h"

// 1_0_0_0 // Hello // client vol conectarse
// 2_i_i_0 // Conexion_Start_Jugador_vacio // client comfirma que esta ready per jugar
// 3_i // Ping_index // Comfirmacio de ping de client

enum State {
	connect,	// Esperar a que es conectin els dos jugadors
	play,		// Partida
	points,		// Un jugador mata a l'altre i se li suma un punt
	win			// el joc sacaba
};


void sendAll(Send* sender, std::vector<ServerPlayer>* player) { // per misatges iguals que s'envien a tots el jugadors

	bool foundMessage = false;									// Per saber si hi ha un misatge igual

	sender->SendMessages(player->at(1).ip, player->at(1).port);
	for (int i = 0; i < player->at(1).keyCommands.size(); i++)
	{
		if (player->at(1).keyCommands[i] == *sender->command) {
			foundMessage = true;
			break;
		}
	}
	if (!foundMessage) player->at(1).keyCommands.push_back(*sender->command);

	foundMessage = false;

	sender->SendMessages(player->at(0).ip, player->at(0).port);
	for (int i = 0; i < player->at(0).keyCommands.size(); i++)
	{
		if (player->at(0).keyCommands[i] == *sender->command) {
			foundMessage = true;
			break;
		}
	}
	if (!foundMessage) player->at(0).keyCommands.push_back(*sender->command);
}

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
	sf::Thread thread(&ServerReceive::ReceiveCommands, &receiver);	// Thread per el receiver
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
	receiver.players = &player;
	//receiver.playertmp = &playertmp;

	//-- SERVER --//
	srand(time(NULL));
	MessageManager protocol;
	Timer timerReady;
	Timer timerPing;
	State state = connect;
	bool playersConected = false;

	timerReady.Start(0);
	timerPing.Start(0);

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

	//-- CONECT --//
		case connect: // que es conectin els dos jugadors
			if (!playersConected) {
				if (!clientCommands.empty()) {
					int clientCase = protocol.GetType(clientCommands.front());
					switch (clientCase) {
					case 1:	// Un client es vol conectar

						for (int i = 0; i < player.size(); i++)
						{
							if (protocol.GetSubType(clientCommands.front()) == player[i].id) {	// el jugador que diu Hello
								if (protocol.GetFirst(clientCommands.front()) == 1) {			// necesita posicio
									if (player[i].id == 1) { 
										player[i].x = 270;// jugador 1 a 270
									} else {
										player[i].x = 1330; // jugador 2 a 1330
									}
									player[i].y = 750;
									std::cout << "\n New user" << std::endl;
								}
								//command = protocol.CreateMessage(std::vector<int>{HELLO, player[i].id, player[i].x});
								command = protocol.CreateMessageP(1, player[i].id, player[i].x); // 1_0_0_vacio // WELCOME_id_x_y
								sender.SendMessages(player[i].ip, player[i].port);
								clientCommands.pop();
							}
						}
						break;

					}
				}
				if (player.size() == TOTALPLAYERS) { // Si existeixen 2 jugadors
					if (player[0].x > 0 && player[1].x > 0) {// si els 2 jugadors tenen posicions valides, estan correctament conectats
						playersConected = true;
					}
				}
			}
			else
			{
				if (timerReady.Check()) {
					command = protocol.CreateMessageP(2, player[0].id, player[0].x); // 2_0_0_vacio // WELCOME_id_x_y
					sender.SendMessages(player[1].ip, player[1].port);
					for (int i = 0; i < player[0].keyCommands.size(); i++)
					{
						if (player[0].keyCommands[i] == command) {
							break;
						}
						else if (i == player[0].keyCommands.size() - 1) {
							player[0].keyCommands.push_back(command);
						}
					}

					command = protocol.CreateMessageP(2, player[1].id, player[1].x); // 2_0_0_vacio // WELCOME_id_x_y
					sender.SendMessages(player[0].ip, player[0].port);
					for (int i = 0; i < player[1].keyCommands.size(); i++)
					{
						if (player[1].keyCommands[i] == command) {
							break;
						}
						else if (i == player[1].keyCommands.size() - 1) {
							player[1].keyCommands.push_back(command);
						}
					}
					timerReady.Start(3000);
				}

				if (!clientCommands.empty()) {
					int clientCase = protocol.GetType(clientCommands.front());
					switch (clientCase) {
					case 1:
						clientCommands.pop();
						break;
					case 2:	// Un client es vol conectar
						int id = protocol.GetSubType(clientCommands.front());
						for (int i = 0; i < player[id].keyCommands.size(); i++) // Recorrer tots els keycommands
						{
							//std::string commandToCheck = player[id].keyCommands[i];
							if (player[id].keyCommands[i][0] == '2') {								// si es un keycommand de ready
								player[id].keyCommands.erase(player[id].keyCommands.begin() + i);	// borral
								break;
							}
						}
						break;
					}
				}

				if (player[0].keyCommands.empty() && player[1].keyCommands.empty())
				{
					state = play;
				}			
			}
			break;

	//-- PLAY --//
		case play:
			// PING
			if (timerPing.Check()) {
				command = "3";
				//TODO: 
				sendAll(&sender, &player);
				timerPing.Start(3000);
			}

	//-- CLIENT COMMANDS --//
			if (!clientCommands.empty()) {
				int clientCase = protocol.GetType(clientCommands.front());
				switch (clientCase) {
				case 1:	// Un client es vol conectar
					clientCommands.pop();
					break;
				case 2:
					clientCommands.pop();
					break;
				case 3:
					int playerId = protocol.GetSubType(clientCommands.front());
					for (int i = 0; i < player[playerId].keyCommands.size(); i++)
					{
						if (protocol.GetType(clientCommands.front()) == protocol.GetType(player[playerId].keyCommands[i])) {
							player[playerId].keyCommands[i].erase();
							break;
						}
					}
					clientCommands.pop();
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