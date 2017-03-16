#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>

#include "Send.h"
#include "Receive.h"
#include "Game.h"
#include "MessageManager.h"

#define MAX_MENSAJES 30
#define MAX_USERS 2

// 1_N_1_vacio // Respuesta_index_Jugador_vacio // client envia respota
// 2_1_0_vacio // Estado_Start_null_null******************** // comença partida
// 2_2_0_vacio // Estado_Finish_null_null // acaba partida
// 2_3_0_Fuera de Tiempo. // Mensaje_Log_Jugador_vacio // s'acaba el temps per respondre
// 2_4_0_vacio // Mensaje_check_jugador_vacio // un jugador ha respos
// 3_N_0_vacio // NuevaPpregunta_index_null_null // el server envia index de pregunta nova
// 4_1_0_Manolo // Jugadores_IndiceJugador_0_Nombre // per enviar nom y index de jugadors
// 5_N_0_vacio // Puntuaciones_Puntuacion_Jugador_null // per enviar puntuacions actualitzades

enum State {
	send, // enviar paraula nova y que comenci partida
	play, // mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points, // Envia les puntuacions als jugadors y actualitza els seus logs
	win // el joc sacaba
};

void cleanPlayers(int* playerChecks) {
	for (int i = 0; i < MAX_USERS; i++)
	{
		playerChecks[i] = 0;
	}
}

bool playersReady(int* playerChecks) {
	for (int i = 0; i < MAX_USERS; i++)
	{
		if (playerChecks[i] == 0) {
			return false;
		}
	}
	return true;
}

void sendAll(Send* sender, std::vector<sf::TcpSocket*> sockets) { // per misatges iguals que s'envien a tots el jugadors
	for (int i = 0; i < MAX_USERS; i++)                           // podriam ficar un bool de parametre si volem que es faci blocking
	{
		sender->send = sockets[i];
		sender->SendMessages();
	}
}

int main()
{
	srand(time(NULL));
	// CHOSE SEVER/CLIENT
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.23.87"); //sf::IpAddress::getLocalAddress();
	std::vector<sf::TcpSocket*> sockets;
	sf::TcpSocket* sockettmp = new sf::TcpSocket;
	MessageManager protocol;
	Timer timer;

	std::vector<Question> questions = initQuestions();
	int questionIndex;
	
	// Crear players per guardar la info
	std::vector<Player> player(MAX_USERS);
	for (int i = 0; i < MAX_USERS; i++)
	{
		player[i]._num = i;
	}
	int playerChecks[MAX_USERS]; // per comprobar quins usuaris hem vist ja
	cleanPlayers(playerChecks);
	std::string textConsole = "Connected to: ";

	std::cout << "Server";
	std::string command; // el misatge que rep per saber que fer
	Send sender;
	sender.command = &command;
	Receive receiver;
	receiver.command = &command;
	
	State state = send;
	//std::string word;

	sf::TcpListener listener;
	// Escuchamos por el puerto 50000
	if (listener.listen(5000) != sf::Socket::Done) {
		std::cout << "No et pots vincular al port 50000" << std::endl;
		return -1;
	}
	//Accept per els dos jugadors

	for (int i = 0; i < MAX_USERS; i++)
	{
		if (listener.accept(*sockettmp) != sf::Socket::Done) {
			std::cout << "Error al acceptar conexió" << std::endl;
			return -1;
		}
		std::cout << "\n New user" << std::endl;
		sockettmp->setBlocking(false);
		sockets.push_back(sockettmp);
		sockettmp = new sf::TcpSocket;
	}
	listener.close();
	//Rebre noms dels jugadors
	int num = 0;
	while (num != 4) {
		for (int i = 0; i < player.size(); i++)
		{
			receiver.socket = sockets[i];
			if (receiver.ReceiveMessages()) {
				player[i]._name = protocol.GetWord(command);
				/*sender.send = sockets[i];
				command = protocol.CreateMessage(4,0, player[i]._num,player[i]._name);
				sender.SendMessages();*/
				num++;
			}
		}
	}

	for (int i = 0; i < MAX_USERS; i++)
	{
		sockets[i]->setBlocking(true);
		sender.send = sockets[i];
		for (int j = 0; j < MAX_USERS; j++)
		{
			command = protocol.CreateMessage(4, 0, player[j]._num, player[j]._name);
			sender.SendMessages();
		}
		sockets[i]->setBlocking(false);
	}

	// OPEN CHAT WINDOW
	std::vector<std::string> aMensajes;

	std::string mensaje = "";
	//sender.mensajes = &mensaje;
	bool serverOn = true;
	int sumScore = 3; // punt per respondre be
	int winner;

	while (serverOn)
	{
		switch (state) {
		case send:
			// TODO: enviar als dos jugadors
			questionIndex = rand() % 9; // agafar pregunta random
			command = protocol.CreateMessage(3, 0, 0, std::to_string(questionIndex)); // crear misatge amb index de la pregunta random
			sendAll(&sender, sockets); // enviar a tots els jugadors el command amb el index de la pregunta random
			sumScore = 3;
			cleanPlayers(playerChecks);
			state = play;
			timer.Start(); // començar timer
			break;
		case play:
			if (timer.Check()) { // si sha acabat el temps
				for (int i = 0; i < sockets.size(); i++)
				{
					if (playerChecks[i] != 1) {
						receiver.socket = sockets[i];
						if (receiver.ReceiveMessages()) { // si rep misatge
							if (protocol.GetType(command) == 1) { // si rep resposta e jugador
								if (protocol.GetSubType(command) == questions[questionIndex].correctAnswer) { // si es resposta correcta
									player[i]._score += sumScore; // suma punts depenent de l'ordre en que ha respos
									if (sumScore > 1) sumScore--; // si tots responen be, el 3er i el 4rt guanyen 1 punt
									playerChecks[i] = 1; // per saber que ha respos
								}
								command = protocol.CreateMessage(2, 4, i, "");
								sendAll(&sender, sockets); // enviar a tots la resposta que ha donat el jugador i
							}
						}
					}
				}
				if (playersReady(playerChecks)) { // tots han respost dins del temps
					state = points;
				}	
			} // TODO?: Enviar que s'ha acabat el temps
			else { 
				state = points;
			}
			break;
		case points:
			for (int i = 0; i < MAX_USERS; i++)
			{
				command = protocol.CreateMessage(5, player[i]._score, player[1]._num, "");
				sendAll(&sender, sockets); // enviar a tots les putnuacions actualitzades
			}

			for (int i = 0; i < MAX_USERS; i++)
			{
				if (player[i]._score > 10) {
					state = win;
					winner = i;
				}
			}
			if (state != win) state = send;
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