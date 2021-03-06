#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <time.h>

#include "Game.h"
#include "Send.h"
#include "Receive.h"
#include "MessageManager.h"
#define MAX_MENSAJES 30
#define MAX_USERS 2

enum State {
	send, // enviar paraula nova y que comenci partida
	play, // mentres els jugadors estan escribint. comproba si sacaba el temps i si alg� ha encertat la partida
	points, // Envia les puntuacions als jugadors y actualitza els seus logs
	win // el joc sacaba
};

void cleanPlayers(int* playerChecks, int size = MAX_USERS) {
	for (int i = 0; i < size; i++)
	{
		playerChecks[i] = 0;
	}
}

bool playersReady(int* playerChecks, int size = MAX_USERS) {
	for (int i = 0; i < size; i++)
	{
		if (playerChecks[i] == 0) {
			return false;
		}
	}
	return true;
}

int main()
{
	MessageManager protocol;
	Timer timer;
	//std::vector<Question> questions = initQuestions(); //Preguntes
	int questionIndex = 0;
	Question question;
	bool answerSent = false;
	int playerChecks[MAX_USERS]; // per comprobar quins usuaris hem vist ja
	// Image draw

	sf::Texture background;
	if (!background.loadFromFile("../Resources/Fons.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite sprite;
	sprite.setTexture(background);
		
	sf::Texture CheckText;
	if (!CheckText.loadFromFile("../Resources/Check.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite check;
	check.setTexture(CheckText);


	// CHOSE CLIENT
	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1"); //sf::IpAddress::getLocalAddress();
	sf::TcpSocket socket;
	std::string textConsole = "Connected to: ";

	std::string command; // el misatge que rep per saber que fer
	std::string name; // Nom auxiliar al crea el player
	int _indexClient; //Index del client

	std::vector<Player> player(MAX_USERS);
	for (int i = 0; i < MAX_USERS; i++)
	{
		player[i]._num = i;
	}

	State state = send;

	sf::Font font;
	if (!font.loadFromFile("cour.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	std::cout << "Introdueix les teves inicials: ";
	std::cin >> name;
	name.resize(3);

	//Preparem els textos a la posici� que els correspont
	sf::Text Pregunta("%%Con qu� dos colores suele \n tener problemas un dalt�nico?", font, 30);
	Pregunta.setFillColor(sf::Color(255, 255, 255));
	Pregunta.setStyle(sf::Text::Bold);
	Pregunta.setPosition(60, 120);

	sf::Text Resposta1("Resposta1", font, 22);
	Resposta1.setFillColor(sf::Color(255, 255, 255));
	Resposta1.setStyle(sf::Text::Bold);
	Resposta1.setPosition(50, 360);

	sf::Text Resposta2("Resposta2", font, 22);
	Resposta2.setFillColor(sf::Color(255, 255, 255));
	Resposta2.setStyle(sf::Text::Bold);
	Resposta2.setPosition(380, 360);

	sf::Text Resposta3("Verde y amarillo", font, 22);
	Resposta3.setFillColor(sf::Color(255, 255, 255));
	Resposta3.setStyle(sf::Text::Bold);
	Resposta3.setPosition(50, 440);

	sf::Text Resposta4("Resposta4", font, 22);
	Resposta4.setFillColor(sf::Color(255, 255, 255));
	Resposta4.setStyle(sf::Text::Bold);
	Resposta4.setPosition(380, 440);

	sf::Text Jugador1("Jugador 1", font, 20);
	Jugador1.setFillColor(sf::Color(255, 255, 255));
	Jugador1.setStyle(sf::Text::Bold);
	Jugador1.setPosition(760, 120);

	sf::Text Jugador2("Jugador 2", font, 20);
	Jugador2.setFillColor(sf::Color(255, 255, 255));
	Jugador2.setStyle(sf::Text::Bold);
	Jugador2.setPosition(760, 240);

	/*sf::Text Jugador3("Jugador 3", font, 20);
	Jugador3.setFillColor(sf::Color(255, 255, 255));
	Jugador3.setStyle(sf::Text::Bold);
	Jugador3.setPosition(760, 365);

	sf::Text Jugador4("Jugador 4", font, 20);
	Jugador4.setFillColor(sf::Color(255, 255, 255));
	Jugador4.setStyle(sf::Text::Bold);
	Jugador4.setPosition(760, 483);*/

	sf::Text Jugador1Score(std::to_string(player[0]._score), font, 25);
	Jugador1Score.setFillColor(sf::Color(255, 255, 255));
	Jugador1Score.setStyle(sf::Text::Bold);
	Jugador1Score.setPosition(865, 60);

	sf::Text Jugador2Score(std::to_string(player[1]._score), font, 25);
	Jugador2Score.setFillColor(sf::Color(255, 255, 255));
	Jugador2Score.setStyle(sf::Text::Bold);
	Jugador2Score.setPosition(865, 185);

	/*sf::Text Jugador3Score(std::to_string(player[2]._score), font, 25);
	Jugador3Score.setFillColor(sf::Color(255, 255, 255));
	Jugador3Score.setStyle(sf::Text::Bold);
	Jugador3Score.setPosition(865, 305);

	sf::Text Jugador4Score(std::to_string(player[3]._score), font, 25);
	Jugador4Score.setFillColor(sf::Color(255, 255, 255));
	Jugador4Score.setStyle(sf::Text::Bold);
	Jugador4Score.setPosition(865, 427);*/

	sf::Text Temps(std::to_string(10), font, 50);
	Temps.setFillColor(sf::Color(255, 255, 255));
	Temps.setStyle(sf::Text::Bold);
	Temps.setPosition(30, 30);


	/////////////////////////////////////////////////////////////////////////

	

	Send sender;
	sender.send = &socket;
	sender.command = &command;
	Receive receiver;
	receiver.socket = &socket;
	// bind puerto 50000 al socket receive
	sf::Socket::Status status = socket.connect(ip, 5000, sf::seconds(5.f));
	if (status != sf::Socket::Done) {
		std::cout << "Error al intent de conexi�" << std::endl;
		return -1;
	}
	// OPEN CHAT WINDOW
	
	receiver.command = &command;
	sf::Vector2i screenDimensions(918, 516); //Dimensions pantalles

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Buzz");

	//Enviem el nostre nom
	command = protocol.CreateMessage(4,0,0,name);
	sender.SendMessages();

	receiver.ReceiveMessages(); //Rebem el primer nom
	if (name == protocol.GetWord(command)) {
		_indexClient = protocol.GetPlayer(command);
	}
	player[protocol.GetPlayer(command)]._name = protocol.GetWord(command);
	Jugador1.setString(protocol.GetWord(command));

	receiver.ReceiveMessages(); //Rebem el segon nom
	if (name == protocol.GetWord(command)) {
		_indexClient = protocol.GetPlayer(command);
	}
	player[protocol.GetPlayer(command)]._name = protocol.GetWord(command);
	Jugador2.setString(protocol.GetWord(command));

	//receiver.ReceiveMessages(); //Rebem el tercer nom
	//if (name == protocol.GetWord(command)) {
	//	_indexClient = protocol.GetPlayer(command);
	//}
	//player[protocol.GetPlayer(command)]._name = protocol.GetWord(command);
	//Jugador3.setString(protocol.GetWord(command));

	//receiver.ReceiveMessages(); //Rebem el quart nom
	//if (name == protocol.GetWord(command)) {
	//	_indexClient = protocol.GetPlayer(command);
	//}
	//player[protocol.GetPlayer(command)]._name = protocol.GetWord(command);
	//Jugador4.setString(protocol.GetWord(command));//Actualitzem la String
	
	while (window.isOpen())
	{
		sf::Event evento;
		switch (state) {
		case send: // rebre la paraula i comen�ar el temps
			socket.setBlocking(true);

			if (receiver.ReceiveMessages()) {
				if (protocol.GetType(command) == 3) {
					questionIndex = protocol.GetSubType(command); // agafar index de la pregunta
					question = initQuestion(questionIndex); // canviar a nova pregunta
					Pregunta.setString(question.question); // actualitzar eltext per pantalla
					Resposta1.setString(question.answer[0]);
					Resposta2.setString(question.answer[1]);
					Resposta3.setString(question.answer[2]);
					Resposta4.setString(question.answer[3]);

					socket.setBlocking(false);
					state = play;
					answerSent = false; //Preparem per enviar la resposta
					for (int i = 0; i < MAX_USERS; i++)
					{
						player[i]._answerCheck = false;
					}
					timer.Start(MAXTIME);
					cleanPlayers(playerChecks);
					command = protocol.CreateMessage(4, 0, 0, "");
					sender.SendMessages();
				}
				else if (protocol.GetType(command) == 2) {
					if (protocol.GetSubType(command) == 5)
					{
						if (protocol.GetPlayer(command) == 9)
						{
							state = win;
						}
					}
				}				
			}
			
			break;
		case play:		
			sf::Keyboard key;
			if (!timer.Check()) {
				if (key.isKeyPressed(sf::Keyboard::Escape)) { // si el jugador vol tancar la comunicacio
					command = protocol.CreateMessage(6, 0, _indexClient, ""); // enviem missatge de adios
					sender.SendMessages(); //enviem missatge
					//socket.setBlocking(true);
				}
				if (!answerSent) {

					if (key.isKeyPressed(sf::Keyboard::Num1)) {
						command = protocol.CreateMessage(1, 0, _indexClient, ""); //enviem resposta 1
						sender.SendMessages(); //enviem missatge
						answerSent = true; //Resposta enviada
						//socket.setBlocking(true);
					}
					else if (key.isKeyPressed(sf::Keyboard::Num2)) {
						command = protocol.CreateMessage(1, 1, _indexClient, ""); //enviem resposta 2
						sender.SendMessages(); //enviem missatge
						answerSent = true; //Resposta enviada
						//socket.setBlocking(true);
					}
					else if (key.isKeyPressed(sf::Keyboard::Num3)) {
						command = protocol.CreateMessage(1, 2, _indexClient, ""); //enviem resposta 3
						sender.SendMessages(); //enviem missatge
						answerSent = true; //Resposta enviada
						//socket.setBlocking(true);
					}
					else if (key.isKeyPressed(sf::Keyboard::Num4)) {
						command = protocol.CreateMessage(1, 3, _indexClient, ""); //enviem resposta 4
						sender.SendMessages(); //enviem missatge
						answerSent = true; //Resposta enviada
						//socket.setBlocking(true);
					}

				}
				if (receiver.ReceiveMessages()) {//Rebem missatge durant el gameplay
					if (protocol.GetType(command) == 2) { //missatge de servidor
						switch (protocol.GetSubType(command)) {
						case (5)://Un jugador a sortit i acabem la partida.
							state = win;
							timer.Stop();
							break;
						case (3)://Han respost tots o a acabat el temps
							//socket.setBlocking(true);
							cleanPlayers(playerChecks);
							timer.Stop();
							state = points;
							break;
						case (4)://un jugador ha respost
							player[protocol.GetPlayer(command)]._answerCheck = true;
							break;
						default:
							std::cout << "Error al rebre missatge" << std::endl;
							break;
						}
					}
				}
			}
			else {
				cleanPlayers(playerChecks);
				state = points;
				timer.Stop();
			}
			break;

		case points:
			socket.setBlocking(true);
			answerSent = false; //Preparem per enviar la resposta
			for (int i = 0; i < MAX_USERS; i++)
			{
				if (receiver.ReceiveMessages()) {
					if (playerChecks[protocol.GetPlayer(command)] == 0) {
						player[protocol.GetPlayer(command)]._score = protocol.GetSubType(command);
						playerChecks[protocol.GetPlayer(command)] = 1;
					}
				}
			}
			if (playersReady(playerChecks)) {
				command = protocol.CreateMessage(4, 0, _indexClient, "");
				sender.SendMessages();
				Jugador1Score.setString(std::to_string(player[0]._score));
				Jugador2Score.setString(std::to_string(player[1]._score));
				state = send;
			}
			if (protocol.GetType(command) == 2) {
				if (protocol.GetSubType(command) == 5)
				{
					if (protocol.GetPlayer(command) == 9)
					{
						state = win;
					}
				}
			}
			break;
		case win:
			int index = 0;
			for (int i = 0; i < MAX_USERS; i++)
			{
				if (player[index]._score < player[i]._score) {
					index = i;
				}
			}
			Pregunta.setString("El jugador: "+player[index]._name+"\n ha guanyat la partida");
			//sf::Keyboard keyEsc;
			if (key.isKeyPressed(sf::Keyboard::Escape)) window.close();
			break;
		}
		/////////////////// PINTAT
		///Fons
		window.draw(sprite);

		for (int i = 0; i < MAX_USERS; i++)
		{
			if (player[i]._answerCheck == true) {
				check.setPosition(680, 130*i);
				window.draw(check);
			}
		}
	

		/// TEXT
		window.draw(Pregunta);
		window.draw(Resposta1);
		window.draw(Resposta2);
		window.draw(Resposta3);
		window.draw(Resposta4);
		window.draw(Jugador1);
		window.draw(Jugador2);/*
		window.draw(Jugador3);
		window.draw(Jugador4);*/
		window.draw(Jugador1Score);
		window.draw(Jugador2Score);/*
		window.draw(Jugador3Score);
		window.draw(Jugador4Score);*/
		//Actualitzem temps
		if (!timer.Check()) {
			Temps.setString(std::to_string(timer.getTime()));
			window.draw(Temps);
		}
			
		//else
		//	Temps.setString("0");
		
		
		window.display();
		window.clear();
	}
	socket.disconnect();
	return 0;
}