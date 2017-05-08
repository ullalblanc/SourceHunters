#pragma once

#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>

#include "InputMemoryBitStream.h"
#include "OutputMemoryBitStream.h"

class Send
{
public:
	Send();
	~Send();

	void SendMessages(sf::IpAddress ip, unsigned short port, OutputMemoryBitStream output);

	sf::UdpSocket *socket;
	std::string *command;
};