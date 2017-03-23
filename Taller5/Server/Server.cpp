#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>

#include "Game.h"

#define MAX_MENSAJES 30
#define MAX_USERS 2

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
	// CHOSE SEVER/CLIENT
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.1.10"); //sf::IpAddress::getLocalAddress();
	std::vector<sf::TcpSocket*> sockets;
	sf::TcpSocket* sockettmp = new sf::TcpSocket;
	MessageManager protocol;
	Timer timer;
	State state = send;

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
			//player[i]._name = protocol.GetWord(command);
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
			break;
		case play:
			break;
		case points:
			break;
		case win:
			break;
		}
	}	
	return 0;
}