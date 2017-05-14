#include <cstring>

#include "Game.h"

// Protocol: https://docs.google.com/spreadsheets/d/152EPpd8-f7fTDkZwQlh1OCY5kjCTxg6-iZ2piXvEgeg/edit?usp=sharing

enum State {
	connect,	// Esperar a que es conectin els dos jugadors
	play,		// Partida
	points,		// Un jugador mata a l'altre i se li suma un punt
	win			// el joc sacaba
};


/*int sendAll(Send* sender, std::vector<ServerPlayer>* player, char* command, size_t size) { // per misatges iguals que s'envien a tots el jugadors

	int commandPush = 0;
	bool foundMessage = false;									// Per saber si hi ha un misatge igual
	for (int i = 0; i < player->size(); i++)
	{
		sender->SendMessages(player->at(i).ip, player->at(i).port, command, size);
		for (int j = 0; j < player->at(i).keyCommands.size(); j++)
		{
			if (strcmp(player->at(i).keyCommands[j].GetBufferPtr(), command)) { //TODO: Petarmel
				foundMessage = true;
				break;
			}
		}
		if (!foundMessage)
		{
			//OutputMemoryBitStream tmp(*output);
			commandPush += i + 1;
			//player->at(i).keyCommands.push_back(*output);
		}

		foundMessage = false;
	}
	return commandPush;
}*/

int main()
{
	//-- UDP --//

	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1");	// sf::IpAddress::getLocalAddress();
	sf::UdpSocket socket;										// El socket del servidor
	std::queue<InputMemoryBitStream> clientCommands;			// Misatges dels jugadors per anar executant
	std::queue<Command> com;
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
	receiver.com = &com;
	receiver.socket = &socket;
	receiver.mutex = &mutex;
	receiver.players = &player;

	//-- SERVER --//

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
		mutex.lock();
		switch (state) {

	//-- CONECT --//
		case connect: { // que es conectin els dos jugadors
			if (!playersConected) {
				//mutex.lock();
				if (!com.empty()) {
					switch (com.front().type) {
					case HELLO: {	// Un client es vol conectar

						for (int i = 0; i < player.size(); i++)
						{
							if (com.front().id == player[i].id) {	// el jugador que diu Hello
								if (player[i].x <= 0) {		// necesita posicio
									if (player[i].id == 0) {
										player[i].x = 270;// jugador 1 a 270
									}
									else {
										player[i].x = 1330; // jugador 2 a 1330
									}
									player[i].y = 750;
									std::cout << "\n New user" << std::endl;
								}
								OutputMemoryBitStream output;
								output.Write(HELLO, TYPE_SIZE);
								output.Write(player[i].id, ID_SIZE);
								output.Write(player[i].x, POSITION_SIZE);
								sender.SendMessages(player[i].ip, player[i].port, output.GetBufferPtr(), output.GetByteLength());
								//clientCommands.pop();
								com.pop();
							}
						}
					}
						break;
					}
				}
				if (player.size() == TOTALPLAYERS) { // Si existeixen 2 jugadors
					if (player[0].x > 0 && player[1].x > 0) {// si els 2 jugadors tenen posicions valides, estan correctament conectats
						playersConected = true;
						for (int j = 0; j < com.size(); j++)
						{
							com.pop();
							//clientCommands.pop();
						}
					}
				}
				//mutex.unlock();
			}
			else
			{
				if (timerReady.Check()) {
					OutputMemoryBitStream output;
					output.Write(CONNECTION, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(player[0].x, POSITION_SIZE);
					sender.SendMessages(player[1].ip, player[1].port, output.GetBufferPtr(), output.GetByteLength());
					for (int i = 0; i < player[1].keyComs.size(); i++)
					{
						if (player[1].keyComs[i].type == CONNECTION) {
							break;
						}
						else if (i == player[1].keyComs.size() - 1) {
							Command comtmp;
							comtmp.type = CONNECTION;
							player[1].keyComs.push_back(comtmp);
						}
					}
					OutputMemoryBitStream output2;
					output2.Write(CONNECTION, TYPE_SIZE);
					output2.Write(player[1].id, ID_SIZE);
					output2.Write(player[1].x, POSITION_SIZE);
					sender.SendMessages(player[0].ip, player[0].port, output2.GetBufferPtr(), output2.GetByteLength());
					for (int i = 0; i < player[0].keyCommands.size(); i++)
					{
						if (player[0].keyComs[i].type == CONNECTION) {
							break;
						}
						else if (i == player[0].keyComs.size() - 1) {
							Command comtmp;
							comtmp.type = CONNECTION;
							player[0].keyComs.push_back(comtmp);
						}
					}
					timerReady.Start(5000);
				}
				//mutex.lock();
				if (!com.empty()) {
					switch (com.front().type) {
					case HELLO:
						playersConected = false;
						
						//clientCommands.pop();
						break;
					case CONNECTION:	// Un client es vol conectar

						player[com.front().id].ready = 1;
						for (int i = 0; i < player[com.front().id].keyComs.size(); i++) // Recorrer tots els keycommands
						{
							if (player[com.front().id].keyComs[i].type == CONNECTION) {								// si es un keycommand de ready							
								for (int j = 0; j < player[com.front().id].keyComs.size(); j++)
								{
									player[com.front().id].keyComs.erase(player[com.front().id].keyComs.begin() + j);	// borral
									std::cout << "All messages deleted for player " << com.front().id << std::endl;
								}							
								break;
							}
						}
						com.pop();
						break;
					}
				}
				//mutex.unlock();
				if (player[0].ready == 1 && player[1].ready == 1)
				{
					state = play;
				}
			}
		}
			break;

	//-- PLAY --//

		case play: {
			//mutex.lock();
			////-- PING --////

			if (timerPing.Check()) {
				// Check si un o els dos jugadors s'ha desconectat
				bool toConect = false;
				for (int i = 0; i < TOTALPLAYERS; i++)
				{
					for (int j = 0; j < player[i].keyComs.size(); j++)
					{
						if (player[i].keyComs[j].type == PING) {
							OutputMemoryBitStream output;
							output.Write(DISCONNECTION, TYPE_SIZE);
							output.Write(i, ID_SIZE); // Misatge que s'ha desconectat el jugador i

							std::cout << "player " << i << " seems to be disconected" << std::endl;

							for (int k = 0; k < player.size(); k++)
							{
								sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
							}

							player.erase(player.begin() + i);
							toConect = true;
						}
					}
				}
				if (toConect) { // si hi ha algun jugador desconectat
					state = connect;
				}
				else {
					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					bool foundMessage = false;									// Per saber si hi ha un misatge igual
					for (int k = 0; k < player.size(); k++)
					{
						sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
						for (int l = 0; l < player[k].keyComs.size(); l++)
						{
							if ( player[k].keyComs[l].type == PING) {
								foundMessage = true;
								break;
							}
						}
						if (!foundMessage)
						{
							Command com;
							com.type = PING;
							player[k].keyComs.push_back(com);
						}

						foundMessage = false;
					}
					
					timerPing.Start(3000);
				}

			}

			////-- CLIENT COMMANDS --////

			
			if (!com.empty()) {
				
				std::cout << "And now Client Case is " << com.front().type << std::endl;

				switch (com.front().type) {

				case HELLO: {	// Un client es vol conectar
					//state = connect;
					com.pop();
				}
					break;

				case CONNECTION: {
					com.pop();
				}
					break;

				case PING: {
					std::cout << "Player " << com.front().id << " Pinged" << std::endl;
					for (int i = 0; i < player[com.front().id].keyComs.size(); i++)
					{
						if (player[com.front().id].keyComs[i].type == PING) {
							player[com.front().id].keyComs.erase(player[com.front().id].keyComs.begin());
							break;
						}
					}
					com.pop();

				}
					break;

				case DISCONNECTION: {
					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					output.Write(com.front().id, ID_SIZE);
					//bool foundMessage = false;									// Per saber si hi ha un misatge igual
					for (int k = 0; k < player.size(); k++)
					{
						sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
					}
					player.erase(player.begin() + com.front().id);
					state = connect;
					com.pop();

				}
									break;
				case MOVEMENT: {
					Accum accumtmp = com.front().accum;
					accumtmp.absolute = player[com.front().id].x + accumtmp.delta;
					// TODO: Comprobacions de trampas i limits

					player[com.front().id].accum.push_back(accumtmp);

					OutputMemoryBitStream output;
					output.Write(MOVEMENT, TYPE_SIZE);
					output.Write(com.front().id, ID_SIZE);
					output.Write(player[com.front().id].accum.back().id, ACCUM_ID_SIZE);
					output.Write(player[com.front().id].accum.back().sign, ID_SIZE); // TODO: Cambiar per que funcioni amb playerId accum
					output.Write(player[com.front().id].accum.back().delta, ACCUM_DELTA_SIZE);

					for (int k = 0; k < player.size(); k++)
					{
						sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
					}
					com.pop();
					
				}
							   break;
				case ATTACK: {

				}
					break;
				}
			}
			//mutex.unlock();
		}
			break;
		}
		mutex.unlock();
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}