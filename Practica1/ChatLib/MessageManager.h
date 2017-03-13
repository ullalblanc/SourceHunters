#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>

class MessageManager
{
private:
	std::string _auxText = "";


public:

	int GetType(std::string RawMessage);
	int GetPlayer(std::string RawMessage);
	int GetSubType(std::string RawMessage);
	std::string GetWord(std::string RawMessage);
	std::string CreateMessage(int type, int subtype, int indexPlayer, std::string Word);

	MessageManager();
	~MessageManager();
};
