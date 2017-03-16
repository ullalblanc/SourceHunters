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

struct Question {
	std::string question = "";
	std::string answer[4];
	int correctAnswer;
};

std::vector<Question> initQuestions() {
	std::vector<Question> questions;
		
	questions[0].question = "¿Cuál de los cinco sentidos se desarrolla el primero?";
	questions[0].answer[0] = "El gusto";
	questions[0].answer[1] = "El olfato";
	questions[0].answer[2] = "La oida";
	questions[0].answer[3] = "El tacto";
	questions[0].correctAnswer = 2;

	questions[1].question = "¿Cuál es el único mamífero con cuatro rodillas?";
	questions[1].answer[0] = "El Elefante";
	questions[1].answer[1] = "El caballo";
	questions[1].answer[2] = "La girafa";
	questions[1].answer[3] = "El rinoceronte";
	questions[1].correctAnswer = 1;

	questions[2].question = "¿Con qué dos colores suele tener problemas un daltónico?";
	questions[2].answer[0] = "Azul y amarillo";
	questions[2].answer[1] = "Verde y amarillo";
	questions[2].answer[2] = "Azul y verde";
	questions[2].answer[3] = "Verde y rojo";
	questions[2].correctAnswer = 4;


	return questions;
}

class Timer {
	std::clock_t time = std::clock();
public:
	Timer() {};
	~Timer() {};

	void Start(float maxtime) {
		time = std::clock() + maxtime;
	};

	bool Check() {
		if (time < std::clock()) return true;
		else return false;
	};

	int getTime() {
		return (int)(time - std::clock());
	};
};