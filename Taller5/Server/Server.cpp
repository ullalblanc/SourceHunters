//#include <SFML\Graphics.hpp>
//#include <SFML\Network.hpp>
#include <cstring>

#include "Game.h"

// 1_0_0_0 // Hello_index_Jugador_vacio // client vol conectarse
// 2_i_i_0 // Conexion_Start_Jugador_vacio // server avisa al client que sha conectat y li envia el id

enum State {
	send,	// enviar paraula nova y que comenci partida
	play,	// mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points, // Envia les puntuacions als jugadors y actualitza els seus logs
	win		// el joc sacaba
};


//void sendAll(Send* sender, sf::UdpSocket socket, std::vector<ServerPlayer>* player) { // per misatges iguals que s'envien a tots el jugadors
//	for (int i = 0; i < player->size(); i++)								
//	{		
//		sender->SendMessages(player[i].ip, player[i].port);
//	}
//}

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
	receiver.playertmp = &playertmp;

	//-- SERVER --//
	srand(time(NULL));
	MessageManager protocol;
	Timer timer;
	Timer positionTimer;
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
				int clientCase = protocol.GetType(clientCommands.front());
				switch (clientCase) {
				case 1:	// Un client es vol conectar

					for (int i = 0; i < player.size(); i++)
					{
						if (protocol.GetSubType(clientCommands.front()) == player[i].id) {	// el jugador que diu Hello
							if (protocol.GetFirst(clientCommands.front()) == 1) {			// necesita posicio
								player[i].x = rand() % 9; // TODO: asegurar que no es repeteixen caselles
								player[i].y = rand() % 9;
								std::cout << "\n New user" << std::endl;
							}
							command = protocol.CreateMessage(1, player[i].id, player[i].x, player[i].y); // 1_0_0_vacio // WELCOME_id_x_y
							sender.SendMessages(player[i].ip, player[i].port);
							clientCommands.pop();
						}
					}

					// WOW
					//if (!player.empty()) { // si no esta buit
					//	for (int i = 0; i < player.size(); i++)
					//	{
					//		if (player[i].port == playertmp.port && player[i].ip == playertmp.ip) { // Si conincideix amb un player existent
					//			command = protocol.CreateMessage(1, player[i].id, player[i].x, player[i].y); // 1_0_0_vacio // WELCOME_id_x_y
					//			sender.SendMessages(player[player.size() - 1].ip, player[player.size() - 1].port);
					//			break;
					//		} else if (i == player.size()-1) {	// Si no coincideix amb un player existent i ja els ha comparat a tots
					//			player.push_back(playertmp);
					//			player[player.size() - 1].x = rand() % 9; // TODO: asegurar que no es repeteixen caselles
					//			player[player.size() - 1].y = rand() % 9;

					//			command = protocol.CreateMessage(1, player[player.size() - 1].id, player[player.size() - 1].x, player[player.size() - 1].y); // 1_0_0_vacio // WELCOME_id_x_y
					//			sender.SendMessages(player[player.size() - 1].ip, player[player.size() - 1].port);

					//			command = protocol.CreateMessage(2, player[player.size() - 1].id, player[player.size() - 1].x, player[player.size() - 1].y); // 2_0_0_vacio // POSITION_id_x_y 
					//			for (int j = 0; j < player.size(); j++) // Enviar a la resta de jugador la posicio
					//			{
					//				if (j != i) {
					//					sender.SendMessages(player[j].ip, player[j].port);
					//				}							
					//			}
					//			std::cout << "\n New user" << std::endl;
					//		} 
					//	}
					//} //else { // si esta buit
					//	player.push_back(playertmp);
					//	int newplayer = player.size() - 1;
					//	player[newplayer].id = newplayer;
					//	player[newplayer].x = rand() % 9;
					//	player[newplayer].y = rand() % 9;

					//	command = protocol.CreateMessage(1, player[player.size() - 1].id, player[player.size() - 1].x, player[player.size() - 1].y); // 1_0_0_vacio // WELCOME_id_x_y
					//	sender.SendMessages(player[player.size() - 1].ip, player[player.size() - 1].port);

					//	std::cout << "\n New user" << std::endl;
					//}
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