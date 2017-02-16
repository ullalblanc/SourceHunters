#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <iostream>

#include <vector>

#define MAX_MENSAJES 30

class Chat
{
	sf::IpAddress ip;
	sf::TcpSocket send;
	sf::TcpSocket receive;
	std::size_t received;
	char connectionType, mode;
	char buffer[2000];

	std::vector<std::string> aMensajes;
	std::string mensaje;

	void Sender();
	void Receiver();
	void Init();
	void Update();
public:
	Chat();
	~Chat();	
	
	void StartChat();
};

