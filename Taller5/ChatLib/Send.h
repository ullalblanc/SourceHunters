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

	sf::UdpSocket *socket;
	std::string *command;
};