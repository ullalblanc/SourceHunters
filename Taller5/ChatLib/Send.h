#pragma once

#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>

class Send
{
public:
	Send();
	~Send();

	void SendMessages(sf::IpAddress ip, unsigned short port);

	sf::UdpSocket *socket;
	std::string *command;
};