#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <queue>
#include <vector>

#include "Send.h"
#include "MessageManager.h"

#define MAXTIME 10000

class Player {

public:
	int id;
	int x;
	int y;
	std::vector<std::string> keyCommands;
};

class ServerPlayer : public Player {
	
public:
	sf::IpAddress ip;
	unsigned short port;

};

class Timer {
	std::clock_t time = std::clock();
public:
	Timer() {};
	~Timer() {};

	void Start(float maxtime) {
		time = std::clock() + maxtime;
	};

	void Stop() {
		time = 0;
	};

	bool Check() {
		if (time < std::clock()) return true; //Torna True al Acabar
		else return false;
	};

	int GetTime() {
		return (int)((time - std::clock())/1000);
	};
};

class Receive
{
public:
	sf::UdpSocket *socket;
	std::queue<std::string> *commands;
	sf::Mutex *mutex;
	bool stopReceive = true;

	Receive() {};
	~Receive() {};

	virtual void ReceiveCommands() = 0;
};

class ServerReceive : public Receive {

public:
	std::vector<ServerPlayer> *players;
	ServerPlayer *playertmp;

	void ReceiveCommands() {
		do {
			char data[1500];
			std::size_t received;
			sf::IpAddress ip;
			unsigned short port;

			sf::Socket::Status status = socket->receive(data, 1500, received, ip, port);
			data[received] = '\0';

			if (status == sf::Socket::Done) {
				mutex->lock();
				//commands->push(data);

				if (data[0] = '1') { // save ip and port
					if (!players->empty()) {
						for (int i = 0; i < players->size(); i++) // recorre tots els jugadors
						{
							if (players->at(i).port == port && players->at(i).ip == ip) // Si es un dels jugadors existens
							{
								data[1] = players->at(i).id + 48; // marca la id
								break;							// acaba el for
							}
							else if (i == players->size() - 1) {	// si no existeix 
								playertmp->ip = ip;				// crea nou jugador
								playertmp->port = port;
								playertmp->id = players->at(players->size() - 1).id++; // un id mes al ultim de la llista

								players->push_back(*playertmp);
								data[1] = players->at(i).id + 48;	// marca la id
								data[2] = '1';						// marca que necesita posicio
							}
						}
					}
					else {
						playertmp->ip = ip;					// crea nou jugador
						playertmp->port = port;
						playertmp->id = 1; // un id mes al ultim de la llista

						players->push_back(*playertmp);
						data[1] = players->at(0).id + 48;	// marca la id
						data[2] = '1';						// marca que necesita posicio
					}
				}
				commands->push(data);
				mutex->unlock();
			}
		} while (stopReceive);
	};
};

class ClientReceive : public Receive {

public:
	std::vector<Player> *players;

	void ReceiveCommands() {
		do {
			char data[1500];
			std::size_t received;
			sf::IpAddress ip;
			unsigned short port;

			sf::Socket::Status status = socket->receive(data, 1500, received, ip, port);
			data[received] = '\0';

			if (status == sf::Socket::Done) {
				mutex->lock();
				commands->push(data);
				if (data[0] = '1') { // save ip and port

				}
				mutex->unlock();
			}
		} while (stopReceive);
	};
};