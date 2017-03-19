#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>

#define MAXTIME 10000

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
		
	questions[0].question = "¿Cuál de los cinco sentidos \n se desarrolla el primero?";
	questions[0].answer.push_back("1.El gusto");
	questions[0].answer[1] = "2.El olfato";
	questions[0].answer[2] = "3.La oida";
	questions[0].answer[3] = "4.El tacto";
	questions[0].correctAnswer = 1;

	questions[1].question = "¿Cuál es el único mamífero \n con cuatro rodillas?";
	questions[1].answer[0] = "1.El Elefante";
	questions[1].answer[1] = "2.El caballo";
	questions[1].answer[2] = "3.La girafa";
	questions[1].answer[3] = "4.El rinoceronte";
	questions[1].correctAnswer = 0;

	questions[2].question = "¿Con qué dos colores suele \n tener problemas un daltónico?";
	questions[2].answer[0] = "1.Azul y amarillo";
	questions[2].answer[1] = "2.Verde y amarillo";
	questions[2].answer[2] = "3.Azul y verde";
	questions[2].answer[3] = "4.Verde y rojo";
	questions[2].correctAnswer = 3;


	return questions;
}

Question initQuestion(int index) {
	Question questiontmp;
		switch (index) {
		case 0:
			questiontmp.question = "¿Cuál de los cinco sentidos \n se desarrolla el primero?";
			questiontmp.answer.push_back("1.El gusto");
			questiontmp.answer.push_back("2.El olfato");
			questiontmp.answer.push_back("3.La oida");
			questiontmp.answer.push_back("4.El tacto");
			questiontmp.correctAnswer = 1;
			break;
		case 1:
			questiontmp.question = "¿Cuál es el único mamífero \n con cuatro rodillas?";
			questiontmp.answer.push_back("1.El Elefante");
			questiontmp.answer.push_back("2.El caballo");
			questiontmp.answer.push_back("3.La girafa");
			questiontmp.answer.push_back("4.El rinoceronte");
			questiontmp.correctAnswer = 0;
			break;
		case 2:
			questiontmp.question = "¿Con qué dos colores suele \n tener problemas un daltónico?";
			questiontmp.answer.push_back("1.Azul y amarillo");
			questiontmp.answer.push_back("2.Verde y amarillo");
			questiontmp.answer.push_back("3.Azul y verde");
			questiontmp.answer.push_back("4.Verde y rojo");
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