#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <queue>

class Receive
{
public:
	sf::UdpSocket *socket;
	std::vector<
	std::queue<std::string> *commands;
	sf::Mutex *mutex;
	bool stopReceive;

	Receive();
	~Receive();

	void ReceiveServer();
	void ReceiveClient();
};