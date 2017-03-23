#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>

#include "Send.h"
#include "Receive.h"
#include "MessageManager.h"

#define MAXTIME 10000

struct Player {

	int _num;
	int x;
	int y;

};

class Timer {
	std::clock_t time = std::clock();
public:
	Timer() {};
	~Timer() {};

	void Start(float maxtime) {
		time = std::clock() + maxtime;
	};

	void Stop() {
		time = 0;
	};

	bool Check() {
		if (time < std::clock()) return true; //Torna True al Acabar
		else return false;
	};

	int getTime() {
		return (int)((time - std::clock())/1000);
	};

	
};