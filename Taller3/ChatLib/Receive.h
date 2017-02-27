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

	bool ReceiveMessages();

	bool stopReceive = false;

	sf::TcpSocket *socket;
	std::vector<std::string> *aMensajes;
};