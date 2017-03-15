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

// 1_1_1_r // Mensaje_Typing_Jugador_letra
// 1_2_1_pelota // Mensaje_Palabra_Jugador_palabra
// 1_3_1_Fuera de Tiempo. // Mensaje_Log_Jugador_Texto
// 2_1_0_vacio // Estado_Start_null_null
// 2_1_0_vacio // Estado_Finish_null_null
// 3_0_0_Palabra // NuevaPalabra_null_null_Palabra
// 4_0_1_Manolo // Jugadores_IndiceJugador_0_Nombre

enum State {
	send, // enviar paraula nova y que comenci partida
	play, // mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points, // Envia les puntuacions als jugadors y actualitza els seus logs
	win // el joc sacaba
};
//
//std::vector<std::string> words = {
//	"anomaly",
//	"cattle",
//	"pharmacy",
//	"composite",
//	"sensible",
//	"nickname",
//	"assured",
//	"sacrifice",
//	"beer",
//	"software"
//};

void cleanPlayers(int* playerChecks) {
	for (int i = 0; i < MAX_USERS; i++)
	{
		playerChecks[i] = 0;
	}
}

void sendAll(Send* sender, std::vector<sf::TcpSocket*> sockets) { // per misatges iguals que s'envien a tots el jugadors
	for (int i = 0; i < MAX_USERS; i++)
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
	
	// Crear players per guardar la info
	std::vector<Player> player(MAX_USERS);
	player[0]._num = 0;
	player[1]._num = 1;
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
	int question;
	//std::string word;

	sf::TcpListener listener;
	// Escuchamos por el puerto 50000
	if (listener.listen(5000) != sf::Socket::Done) {
		std::cout << "No et pots vincular al port 50000" << std::endl;
		return -1;
	}
	//Accept per els dos jugadors
	if (listener.accept(*sockettmp) != sf::Socket::Done) {
		std::cout << "Error al acceptar conexió" << std::endl;
		return -1;
	}
	std::cout << "\n New user" << std::endl;
	sockettmp->setBlocking(false);
	sockets.push_back(sockettmp);
	sockettmp = new sf::TcpSocket;
	if (listener.accept(*sockettmp) != sf::Socket::Done) {
		std::cout << "Error al acceptar conexió" << std::endl;
		return -1;
	}
	listener.close();
	std::cout << "\n New user" << std::endl;
	sockettmp->setBlocking(false);
	sockets.push_back(sockettmp);
	
	//Rebre noms dels jugadors
	int num = 1;
	while (num != 3) {
		for (int i = 0; i < player.size(); i++)
		{
			receiver.socket = sockets[i];
			if (receiver.ReceiveMessages()) {
				player[i]._name = protocol.GetWord(command);
				sender.send = sockets[i];
				command = protocol.CreateMessage(4,0, player[i]._num,player[i]._name);
				sender.SendMessages();
				num++;
			}
		}
	}

	//sockets[0]->setBlocking(true);
	sender.send = sockets[0];
	command = protocol.CreateMessage(4, 0, player[1]._num, player[1]._name);
	sender.SendMessages();
	//sockets[0]->setBlocking(false);

	//sockets[1]->setBlocking(true);
	sender.send = sockets[1];
	command = protocol.CreateMessage(4, 0, player[0]._num, player[0]._name);
	sender.SendMessages();
	//sockets[1]->setBlocking(false);

	// OPEN CHAT WINDOW
	std::vector<std::string> aMensajes;

	std::string mensaje = "";
	//sender.mensajes = &mensaje;
	bool serverOn = true;
	int points = 3;

	while (serverOn)
	{
		switch (state) {
		case send:
			// TODO: enviar als dos jugadors
			question = rand() % 9; // agafar pregunta random
			command = protocol.CreateMessage(3, 0, 0, std::to_string(question)); // crear misatge amb index de la pregunta random
			sendAll(&sender, sockets); // enviar a tots els jugadors el command amb el index de la pregunta random
			/*for (int i = 0; i < sockets.size(); i++)
			{
				//sender.send = &
			}*/
			sender.SendMessages();
			state = play;
			break;
		case play:
			// TODO: rebre resposta dels jugadors i enviarles
			for (int i = 0; i < sockets.size(); i++)
			{
				receiver.socket = sockets[i];
				if (receiver.ReceiveMessages()) {
					playerChecks[i] = 1;
					for (int j = 0; j < sockets.size(); j++)
					{
						sender.send = sockets[j];					
						sender.SendMessages();
					}
				}
				//sender.send = &
			}
			// TODO: si tots responen dins del temps, actualitzar puntuacions
			// TODO: si s'acaba el temps, tornar a enviar paraula
			break;
		case points:
			// TODO: actualitzar les puntuacions de la pregunta i enviar les puntuacions. Si un te 5 punts, acabar partida
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