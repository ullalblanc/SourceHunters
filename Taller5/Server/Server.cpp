#include <cstring>

#include "Game.h"

// Protocol: https://docs.google.com/spreadsheets/d/152EPpd8-f7fTDkZwQlh1OCY5kjCTxg6-iZ2piXvEgeg/edit?usp=sharing

enum State {
	connect,	// Esperar a que es conectin els dos jugadors
	play,		// Partida
	points,		// Un jugador mata a l'altre i se li suma un punt
	win			// el joc sacaba
};


int sendAll(Send* sender, std::vector<ServerPlayer>* player, char* command, size_t size) { // per misatges iguals que s'envien a tots el jugadors

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
}

int main()
{
	//-- UDP --//

	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1");	// sf::IpAddress::getLocalAddress();
	sf::UdpSocket socket;										// El socket del servidor
	std::queue<InputMemoryBitStream> clientCommands;			// Misatges dels jugadors per anar executant
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
		case connect: { // que es conectin els dos jugadors
			if (!playersConected) {
				mutex.lock();
				if (!clientCommands.empty()) {
					int clientCase = 0;
					//InputMemoryBitStream input = clientCommands.front();//(clientCommands.front().GetBufferPtr(), )
					clientCommands.front().Read(&clientCase, TYPE_SIZE);
					//int clientCase = protocol.GetType(clientCommands.front());
					switch (clientCase) {
					case HELLO: {	// Un client es vol conectar

						for (int i = 0; i < player.size(); i++)
						{
							if (clientCommands.front().GetNewId() == player[i].id) {	// el jugador que diu Hello
								if (player[i].x <= 0) {//protocol.GetFirst(clientCommands.front()) == 1) {			// necesita posicio
									if (player[i].id == 0) {
										player[i].x = 270;// jugador 1 a 270
									}
									else {
										player[i].x = 1330; // jugador 2 a 1330
									}
									player[i].y = 750;
									std::cout << "\n New user" << std::endl;
								}
								//command = protocol.CreateMessage(std::vector<int>{HELLO, player[i].id, player[i].x});
								OutputMemoryBitStream output;
								output.Write(HELLO, TYPE_SIZE);
								output.Write(player[i].id, ID_SIZE);
								output.Write(player[i].x, POSITION_SIZE);
								//command = protocol.CreateMessageP(1, player[i].id, player[i].x); // 1_0_0_vacio // WELCOME_id_x_y
								sender.SendMessages(player[i].ip, player[i].port, output.GetBufferPtr(), output.GetByteLength());
								clientCommands.pop();
							}
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
				mutex.unlock();
			}
			else
			{
				if (timerReady.Check()) {
					OutputMemoryBitStream output;
					output.Write(CONNECTION, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(player[0].x, POSITION_SIZE);
					//command = protocol.CreateMessageP(2, player[0].id, player[0].x); // 2_0_0_vacio // WELCOME_id_x_y
					sender.SendMessages(player[1].ip, player[1].port, output.GetBufferPtr(), output.GetByteLength());
					for (int i = 0; i < player[1].keyCommands.size(); i++)
					{
						if (player[1].keyCommands[i] == output.GetBufferPtr()) {
							break;
						}
						else if (i == player[1].keyCommands.size() - 1) {
							player[1].keyCommands.push_back(output.GetBufferPtr());
						}
					}
					OutputMemoryBitStream output2;
					output2.Write(CONNECTION, TYPE_SIZE);
					output2.Write(player[1].id, ID_SIZE);
					output2.Write(player[1].x, POSITION_SIZE);
					//command = protocol.CreateMessageP(2, player[1].id, player[1].x); // 2_0_0_vacio // WELCOME_id_x_y
					sender.SendMessages(player[0].ip, player[0].port, output2.GetBufferPtr(), output2.GetByteLength());
					for (int i = 0; i < player[0].keyCommands.size(); i++)
					{
						if (player[0].keyCommands[i] == output.GetBufferPtr()) {
							break;
						}
						else if (i == player[0].keyCommands.size() - 1) {
							player[0].keyCommands.push_back(output.GetBufferPtr());
						}
					}
					timerReady.Start(3000);
				}
				mutex.lock();
				if (!clientCommands.empty()) {
					int clientCase = 0; 
					clientCommands.front().Read(&clientCase, TYPE_SIZE);
					//int clientCase = protocol.GetType(clientCommands.front());
					switch (clientCase) {
					case HELLO:
						clientCommands.pop();
						break;
					case CONNECTION:	// Un client es vol conectar
						int id = 0; 
						clientCommands.front().Read(&id, ID_SIZE); //protocol.GetSubType(clientCommands.front());

						for (int i = 0; i < player[id].keyCommands.size(); i++) // Recorrer tots els keycommands
						{
							InputMemoryBitStream intmp(player[id].keyCommands[i].c_str(), player[id].keyCommands[i].size());
							int typetmp = 0;  
							intmp.Read(&typetmp, 1);
							//std::string commandToCheck = player[id].keyCommands[i];
							if (typetmp == CONNECTION) {								// si es un keycommand de ready
								player[id].keyCommands.erase(player[id].keyCommands.begin() + i);	// borral
								break;
							}
						}
						break;
					}
				}
				mutex.unlock();
				if (player[0].keyCommands.empty() && player[1].keyCommands.empty())
				{
					state = play;
				}
			}
		}
			break;

	//-- PLAY --//
		case play: {
			////-- PING --////
			if (timerPing.Check()) {
				// Check si un o els dos jugadors s'ha desconectat
				bool toConect = false;
				for (int i = 0; i < TOTALPLAYERS; i++)
				{
					for (int j = 0; j < player[i].keyCommands.size(); j++)
					{
						InputMemoryBitStream intmp(player[i].keyCommands[j].GetBufferPtr(), player[i].keyCommands[j].GetByteLength() * 8);
						int typetmp = 0;  
						intmp.Read(&typetmp, 1);
						if (typetmp == PING) {
							OutputMemoryBitStream output;
							output.Write(DISCONNECTION, TYPE_SIZE);
							output.Write(i, ID_SIZE);
							//command = "4" + std::to_string(i); // Misatge que s'ha desconectat el jugador i

							int commandPush = sendAll(&sender, &player, output.GetBufferPtr(), output.GetByteLength());
							switch (commandPush)
							{
							case 1:
								player[0].keyCommands.push_back(output);
								break;
							case 2:
								player[1].keyCommands.push_back(output);
								break;
							case 3:
								player[0].keyCommands.push_back(output);
								player[1].keyCommands.push_back(output);
								break;
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
					//command = "3";
					int commandPush = sendAll(&sender, &player, output.GetBufferPtr(), output.GetByteLength());
					switch (commandPush)
					{
					case 1:
						player[0].keyCommands.push_back(output);
						break;
					case 2:
						player[1].keyCommands.push_back(output);
						break;
					case 3:
						player[0].keyCommands.push_back(output);
						player[1].keyCommands.push_back(output);
						break;
					}
					timerPing.Start(3000);
				}

			}

			////-- CLIENT COMMANDS --////
			mutex.lock();
			if (!clientCommands.empty()) {
				int clientCase = 0; 
				clientCommands.front().Read(&clientCase, TYPE_SIZE);
				switch (clientCase) {

				case HELLO: {	// Un client es vol conectar
					clientCommands.pop();
				}
					break;

				case CONNECTION: {
					clientCommands.pop();
				}
					break;

				case PING: {
					int playerId = 0; 
					clientCommands.front().Read(&playerId, ID_SIZE);
					//= protocol.GetSubType(clientCommands.front());
					for (int i = 0; i < player[playerId].keyCommands.size(); i++)
					{
						InputMemoryBitStream intmp(player[playerId].keyCommands[i].GetBufferPtr(), player[playerId].keyCommands[i].GetByteLength() * 8);
						int typetmp = 0;  
						intmp.Read(&typetmp, 1);
						if (clientCase == typetmp) {
							player[playerId].keyCommands.erase(player[playerId].keyCommands.begin() + 1);
							break;
						}
					}
					clientCommands.pop();
				}
					break;

				case DISCONNECTION: {
					int playerId = 0; 
					clientCommands.front().Read(&playerId, ID_SIZE);
					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					output.Write(playerId, ID_SIZE);
					int commandPush = sendAll(&sender, &player, output.GetBufferPtr(), output.GetByteLength());
					switch (commandPush)
					{
					case 1:
						player[0].keyCommands.push_back(output);
						break;
					case 2:
						player[1].keyCommands.push_back(output);
						break;
					case 3:
						player[0].keyCommands.push_back(output);
						player[1].keyCommands.push_back(output);
						break;
					}
					player.erase(player.begin() + playerId);
					state = connect;
					clientCommands.pop();
					break;
				}
				case MOVEMENT: {
					int playerId = 0; 
					clientCommands.front().Read(&playerId, ID_SIZE);
					int position = 0; 
					clientCommands.front().Read(&position, POSITION_SIZE);

					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					output.Write(playerId, ID_SIZE);
					output.Write(position, POSITION_SIZE);
					int commandPush = sendAll(&sender, &player, output.GetBufferPtr(), output.GetByteLength());
					switch (commandPush)
					{
					case 1:
						player[0].keyCommands.push_back(output);
						break;
					case 2:
						player[1].keyCommands.push_back(output);
						break;
					case 3:
						player[0].keyCommands.push_back(output);
						player[1].keyCommands.push_back(output);
						break;
					}
					clientCommands.pop();
					break;
				}
				case ATTACK: {

				}
					break;
				}
			}
			mutex.unlock();
		}
			break;
		}
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}