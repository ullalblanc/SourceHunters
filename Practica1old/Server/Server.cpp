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
#define MAX_USERS 4

// 1_N_1_vacio // Respuesta_index_Jugador_vacio // client envia respota
// 2_1_0_vacio // Estado_Start_null_null******************** // comença partida
// 2_2_0_vacio // Estado_Finish_null_null // acaba partida
// 2_3_0_Fuera de Tiempo. // Mensaje_Log_Jugador_vacio // s'acaba el temps per respondre
// 2_4_0_vacio // Mensaje_check_jugador_vacio // un jugador ha respos
// 3_N_0_vacio // NuevaPpregunta_index_null_null // el server envia index de pregunta nova
// 4_1_0_Manolo // Jugadores_IndiceJugador_0_Nombre // per enviar nom y index de jugadors// en state points sirve para notificar al server que ha recibido todas las puntuaciones y esta listo para la siguiente pregunta
// 5_N_0_vacio // Puntuaciones_Puntuacion_Jugador_null // per enviar puntuacions actualitzades
// 6_0_0_vacio // desconexion_null_jugador_null // para desconectar un cliente. tambien se usa para acabar la partida

enum State {
	send, // enviar paraula nova y que comenci partida
	play, // mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points, // Envia les puntuacions als jugadors y actualitza els seus logs
	win // el joc sacaba
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

void sendAll(Send* sender, std::vector<sf::TcpSocket*> sockets, bool block = false) { // per misatges iguals que s'envien a tots el jugadors
	for (int i = 0; i < sockets.size(); i++)                           // podriam ficar un bool de parametre si volem que es faci blocking
	{
		if (block) sockets[i]->setBlocking(true);
		sender->send = sockets[i];
		sender->SendMessages();
		if (block) sockets[i]->setBlocking(false);
	}
}

int main()
{
	srand(time(NULL));
	// CHOSE SEVER/CLIENT
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.1.10"); //sf::IpAddress::getLocalAddress();
	std::vector<sf::TcpSocket*> sockets;
	sf::TcpSocket* sockettmp = new sf::TcpSocket;
	MessageManager protocol;
	Timer timer;

	//std::vector<Question> questions = initQuestions();
	int questionIndex = -1;
	Question question = initQuestion(questionIndex);
	
	// Crear players per guardar la info
	std::vector<Player> player(MAX_USERS);
	for (int i = 0; i < player.size(); i++)
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

	sf::TcpListener listener;
	// Escuchamos por el puerto 50000
	if (listener.listen(5000) != sf::Socket::Done) {
		std::cout << "No et pots vincular al port 50000" << std::endl;
		return -1;
	}
	//Accept per els dos jugadors

	for (int i = 0; i < player.size(); i++)
	{
		if (listener.accept(*sockettmp) != sf::Socket::Done) {
			std::cout << "Error al acceptar conexió" << std::endl;
			return -1;
		}
		std::cout << "\n New user" << std::endl;
		//sockettmp->setBlocking(false);
		sockets.push_back(sockettmp);
		sockettmp = new sf::TcpSocket;
	}
	listener.close();
	//Rebre noms dels jugadors
	for (int i = 0; i < player.size(); i++)
	{
		receiver.socket = sockets[i];
		if (receiver.ReceiveMessages()) {
			player[i]._name = protocol.GetWord(command);
		}
	}

	for (int i = 0; i < player.size(); i++)
	{
		command = protocol.CreateMessage(4, 0, player[i]._num, player[i]._name);
		sendAll(&sender, sockets, true);
	}

	// OPEN CHAT WINDOW
	bool serverOn = true;
	int sumScore; // punt per respondre be
	bool clientsReady = true; // per saber si els jugadors estan llestos per la següent pregunta
	for (int i = 0; i < sockets.size(); i++)
	{
		sockets[i]->setBlocking(false);
	}

	while (serverOn)
	{
		sf::Keyboard key;
		if (key.isKeyPressed(sf::Keyboard::BackSpace)) { // si el server vol tancar la comunicacio
			state = win;
		}
		switch (state) {
		case send:
			//if (timer.Check()) {
				questionIndex++;
				if (questionIndex >= 3) {
					state = win;
				}
				else {
					question = initQuestion(questionIndex);
					command = protocol.CreateMessage(3, questionIndex, 0, ""); // crear misatge amb index de la pregunta random
					sendAll(&sender, sockets, true); // enviar a tots els jugadors el command amb el index de la pregunta random
					sumScore = 3;
					state = play;
					timer.Start(MAXTIME); // començar timer
				}				
			//}
			break;
		case play:
			if (!timer.Check()) { // si sha acabat el temps
				for (int i = 0; i < sockets.size(); i++)
				{
					if (playerChecks[i] != 1) { // si el jugador i ya ha enviado respueste, se le omite
						receiver.socket = sockets[i];
						if (receiver.ReceiveMessages()) { // si rep misatge
							if (protocol.GetType(command) == 1) { // si rep resposta e jugador
								if (protocol.GetSubType(command) == question.correctAnswer) { // si es resposta correcta
									player[i]._score += sumScore; // suma punts depenent de l'ordre en que ha respos
									if (sumScore > 1) sumScore--; // si tots responen be, el 3er i el 4rt guanyen 1 punt
									playerChecks[i] = 1; // per saber que ha respos
								}
								command = protocol.CreateMessage(2, 4, i, "");
								sendAll(&sender, sockets); // enviar a tots la resposta que ha donat el jugador i
							}
							else if (protocol.GetType(command) == 6) { // si rep que el client es vol desconectar
								state = win; // decidim qui es el guanyador i acabem la partida.
							}
						}
					}
				}
				if (playersReady(playerChecks)) { // tots han respost dins del temps
					command = protocol.CreateMessage(2, 3, 0, ""); // s'ha acabat el temps
					sendAll(&sender, sockets, true);
					state = points;
					//cleanPlayers(playerChecks);
				}	
			} 
			else { 
				command = protocol.CreateMessage(2,3,0,""); // s'ha acabat el temps
				sendAll(&sender, sockets, true);
				state = points;
				//cleanPlayers(playerChecks);
			}
			break;
		case points:
			if (clientsReady) {
				for (int i = 0; i < player.size(); i++)
				{
					command = protocol.CreateMessage(5, player[i]._score, player[i]._num, "");
					sendAll(&sender, sockets, true); // enviar a tots les putnuacions actualitzades
				}
				clientsReady = false;
				cleanPlayers(playerChecks);
			}
			else {
				for (int i = 0; i < sockets.size(); i++)
				{
					if (playerChecks[i] != 1) { // si el jugador i ya ha enviado respueste, se le omite
						receiver.socket = sockets[i];
						if (receiver.ReceiveMessages()) { // si rep misatge
							if (protocol.GetType(command) == 4) { // si rep resposta e jugador
								playerChecks[i] = 1; // per saber que ha rebut be les puntuacions
							}
						}
					}
				}
				if (playersReady(playerChecks, player.size())) { // si tots han rebut be les puntuacions
					for (int i = 0; i < player.size(); i++)
					{
						if (player[i]._score > 10) {
							state = win;
						}
					}
					if (state != win) state = send;
				}
			}	
			//timer.Start(1000); // dejar un segundo antes de realizar la siguiente accion
			break;
		case win:
			//if (timer.Check()) {
				command = protocol.CreateMessage(2, 2, 0, ""); // enviem que s'acaba la partida, el client s'encarrega de mostrar el guanyador i tencar la comunicacio per la seva part
				sendAll(&sender, sockets, true);
				for (int i = 0; i < sockets.size(); i++)
				{
					sockets[i]->disconnect();
					delete sockets[i];
				}
				serverOn = false;
			//}
			break;
		}
	}	
	return 0;
}