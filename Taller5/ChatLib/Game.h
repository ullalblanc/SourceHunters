#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <queue>

#include "Send.h"
#include "Receive.h"
#include "MessageManager.h"

#define MAXTIME 10000

class Player {

	int id;
	int x;
	int y;

};

class ServerPlayer : public Player {
	
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

	Receive();
	~Receive();

	virtual void ReceiveCommands() = 0;
};

class ServerReceive : public Receive {

	std::vector<ServerPlayer> *players;

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

class ClientReceive : public Receive {

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