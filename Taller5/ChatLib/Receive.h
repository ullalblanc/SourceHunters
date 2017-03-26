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
	std::queue<sf::IpAddress> *ipQueue;		// On es guarden les ips no asignades dels nous jugadors
	std::queue<unsigned short> *portQueue;	// On es guarden els ports no asigntas dels nous jugadors
	std::queue<std::string> *commands;
	sf::Mutex *mutex;
	bool stopReceive;

	Receive();
	~Receive();

	void ReceiveMessages();	
};