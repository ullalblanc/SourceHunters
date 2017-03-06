#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>

#define TIME 5000

struct Player {

	int _num;
	std::string _name;
	int _score = 0;
	std::string _word = "";
	std::vector<std::string> _log;
	
};