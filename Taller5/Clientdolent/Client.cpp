#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <time.h>

#include "Game.h"

#define MAX_MENSAJES 30
#define MAX_USERS 2

enum State {
	send, // enviar paraula nova y que comenci partida
	play, // mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
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

	//Preparem els textos a la posició que els correspont
	sf::Text Pregunta("%%Con qué dos colores suele \n tener problemas un daltónico?", font, 30);
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

	sf::Text Jugador1Score(std::to_string(player[0]._score), font, 25);
	Jugador1Score.setFillColor(sf::Color(255, 255, 255));
	Jugador1Score.setStyle(sf::Text::Bold);
	Jugador1Score.setPosition(865, 60);

	sf::Text Jugador2Score(std::to_string(player[1]._score), font, 25);
	Jugador2Score.setFillColor(sf::Color(255, 255, 255));
	Jugador2Score.setStyle(sf::Text::Bold);
	Jugador2Score.setPosition(865, 185);

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
		std::cout << "Error al intent de conexió" << std::endl;
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

	while (window.isOpen())
	{
		switch (state) {
		case send:
			break;
		case play:
			break;
		case points:
			break;
		case win:
			break;
		}
		/////////////////// PINTAT
		///Fons
		window.draw(sprite);

		// TEXT
		window.draw(Pregunta);
		window.draw(Resposta1);
		window.draw(Resposta2);
		window.draw(Resposta3);
		window.draw(Resposta4);
		window.draw(Jugador1);
		window.draw(Jugador2);
		window.draw(Jugador1Score);
		window.draw(Jugador2Score);

		//Actualitzem temps
		if (!timer.Check()) {
			Temps.setString(std::to_string(timer.getTime()));
			window.draw(Temps);
		}
		window.display();
		window.clear();
	}
	socket.disconnect();
	return 0;
}