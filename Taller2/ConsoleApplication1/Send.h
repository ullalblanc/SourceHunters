#pragma once

#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>

class Send
{
public:
	Send();
	~Send();

	void SendMessages();

	sf::TcpSocket *send;
	std::string *mensajes;
};

