#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
class Receive
{
public:
	Receive();
	~Receive();

	void ReceiveMessages();

	bool stopReceive = false;

	sf::TcpSocket *receive;
	std::vector<std::string> *aMensajes;
};

