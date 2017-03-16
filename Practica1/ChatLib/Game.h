#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>

#define MAXTIME 5000

struct Player {

	int _num;
	std::string _name;
	int _score = 0;
	//int _answer = 0;
	//std::string _word = "";
	//std::vector<std::string> _log;
	
};

class Timer {
	std::clock_t time = std::clock();
public:
	Timer() {};
	~Timer() {};

	void Start() {
		time = std::clock() + MAXTIME;
	};

	bool Check() {
		if (time < std::clock()) return true;
		else return false;
	};

	int getTime() {
		return (int)(time - std::clock());
	};
};