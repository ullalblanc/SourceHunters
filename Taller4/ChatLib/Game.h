#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>

#define TIME 5000

struct Player {

	std::string _name;
	int _score;
	std::string _word;
	std::vector<std::string> _log;
	
};