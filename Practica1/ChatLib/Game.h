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
	bool _answerCheck;
	//int _answer = 0;
	//std::string _word = "";
	//std::vector<std::string> _log;
	
};

struct Question {
	std::string question = "";
	std::vector<std::string> answer;
	int correctAnswer;
};

std::vector<Question> initQuestions() {
	std::vector<Question> questions;
		
	questions[0].question = "�Cu�l de los cinco sentidos \n se desarrolla el primero?";
	questions[0].answer.push_back("El gusto");
	questions[0].answer[1] = "El olfato";
	questions[0].answer[2] = "La oida";
	questions[0].answer[3] = "El tacto";
	questions[0].correctAnswer = 1;

	questions[1].question = "�Cu�l es el �nico mam�fero \n con cuatro rodillas?";
	questions[1].answer[0] = "El Elefante";
	questions[1].answer[1] = "El caballo";
	questions[1].answer[2] = "La girafa";
	questions[1].answer[3] = "El rinoceronte";
	questions[1].correctAnswer = 0;

	questions[2].question = "�Con qu� dos colores suele \n tener problemas un dalt�nico?";
	questions[2].answer[0] = "Azul y amarillo";
	questions[2].answer[1] = "Verde y amarillo";
	questions[2].answer[2] = "Azul y verde";
	questions[2].answer[3] = "Verde y rojo";
	questions[2].correctAnswer = 3;


	return questions;
}

Question initQuestion(int index) {
	Question questiontmp;
		switch (index) {
		case 0:
			questiontmp.question = "�Cu�l de los cinco sentidos \n se desarrolla el primero?";
			questiontmp.answer.push_back("El gusto");
			questiontmp.answer.push_back("El olfato");
			questiontmp.answer.push_back("La oida");
			questiontmp.answer.push_back("El tacto");
			questiontmp.correctAnswer = 1;
			break;
		case 1:
			questiontmp.question = "�Cu�l es el �nico mam�fero \n con cuatro rodillas?";
			questiontmp.answer.push_back("El Elefante");
			questiontmp.answer.push_back("El caballo");
			questiontmp.answer.push_back("La girafa");
			questiontmp.answer.push_back("El rinoceronte");
			questiontmp.correctAnswer = 0;
			break;
		case 2:
			questiontmp.question = "�Con qu� dos colores suele \n tener problemas un dalt�nico?";
			questiontmp.answer.push_back("Azul y amarillo");
			questiontmp.answer.push_back("Verde y amarillo");
			questiontmp.answer.push_back("Azul y verde");
			questiontmp.answer.push_back("Verde y rojo");
			questiontmp.correctAnswer = 3;
			break;
	}
		return questiontmp;
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
		if (time < std::clock()) return true; //Torna True al Acabar
		else return false;
	};

	int getTime() {
		return (int)(time - std::clock());
	};
};