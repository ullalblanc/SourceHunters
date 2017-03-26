//#include <SFML\Graphics.hpp>
//#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <vector>

#include "Game.h"

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
	for (int i = 0; i < sockets.size(); i++)								
	{
		if (block) sockets[i]->setBlocking(true);
		sender->socket = sockets[i];
		sender->SendMessages();
		if (block) sockets[i]->setBlocking(false);
	}
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
	Receive receiver;											// Receiver per rebre constanment misatges
	sf::Thread thread(&Receive::ReceiveMessages, &receiver);	// Thread per el receiver

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
	receiver.ipQueue = &ipQueue;
	receiver.portQueue = &portQueue;
	receiver.mutex = &mutex;

	//-- SERVER --//

	MessageManager protocol;
	Timer timer;
	State state = play;
	std::vector<Player> player;	// Vector de jugadors
	Player playertmp;

	//-- GAME --//

	bool gameOn = true;
	thread.launch();

	std::cout << "Server";
	std::cout << "\n New user" << std::endl;

	while (gameOn)
	{
		sf::Keyboard key;
		if (key.isKeyPressed(sf::Keyboard::BackSpace)) { // si el server vol tancar la comunicacio
			state = win;
		}

		switch (state) {
		case play:
			// TODO: switch que agafi el misatge de la cua i faci el que toqui. Tindriem que mirar primer el protocol
			break;
		}
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}