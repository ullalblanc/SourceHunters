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

enum State {
	send, // enviar paraula nova y que comenci partida
	play, // mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points, // Envia les puntuacions als jugadors y actualitza els seus logs
	win // el joc sacaba
};

int main()
{
	MessageManager protocol;
	std::clock_t time = std::clock(); // pels timers					

	// Image draw

	sf::Texture background;
	if (!background.loadFromFile("../Resources/Fons.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite sprite;
	sprite.setTexture(background);
		


	// CHOSE SEVER/CLIENT
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.23.87"); //sf::IpAddress::getLocalAddress();
	sf::TcpSocket socket;
	//std::string name;
	std::string textConsole = "Connected to: ";

	std::string command; // el misatge que rep per saber que fer
	std::string name; // Nom auxiliar al crea el player
	int _indexClient; //Index del client

	Player player1;
	Player player2;
	Player player3;
	Player player4;

	State state = send;

	sf::Font font;
	if (!font.loadFromFile("courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	std::cout << "Enter a name: ";
	std::cin >> name;

	//Preparem els textos a la posició que els correspont
	sf::Text Pregunta("0", font, 14);
	Pregunta.setFillColor(sf::Color(60, 60, 200));
	Pregunta.setStyle(sf::Text::Bold);
	Pregunta.setPosition(0, 560);

	sf::Text Resposta1("0", font, 12);
	Resposta1.setFillColor(sf::Color(60, 60, 200));
	Resposta1.setStyle(sf::Text::Bold);
	Resposta1.setPosition(0, 560);

	sf::Text Resposta2("0", font, 12);
	Resposta2.setFillColor(sf::Color(60, 60, 200));
	Resposta2.setStyle(sf::Text::Bold);
	Resposta2.setPosition(0, 560);

	sf::Text Resposta3("0", font, 12);
	Resposta3.setFillColor(sf::Color(60, 60, 200));
	Resposta3.setStyle(sf::Text::Bold);
	Resposta3.setPosition(0, 560);

	sf::Text Resposta4("0", font, 12);
	Resposta4.setFillColor(sf::Color(60, 60, 200));
	Resposta4.setStyle(sf::Text::Bold);
	Resposta4.setPosition(0, 560);

	sf::Text Jugador1("Jugador 1", font, 12);
	Jugador1.setFillColor(sf::Color(60, 60, 200));
	Jugador1.setStyle(sf::Text::Bold);
	Jugador1.setPosition(0, 560);

	sf::Text Jugador2("Jugador 2", font, 12);
	Jugador2.setFillColor(sf::Color(60, 60, 200));
	Jugador2.setStyle(sf::Text::Bold);
	Jugador2.setPosition(0, 560);

	sf::Text Jugador3("Jugador 3", font, 12);
	Jugador3.setFillColor(sf::Color(60, 60, 200));
	Jugador3.setStyle(sf::Text::Bold);
	Jugador3.setPosition(0, 560);

	sf::Text Jugador4("Jugador 4", font, 12);
	Jugador4.setFillColor(sf::Color(60, 60, 200));
	Jugador4.setStyle(sf::Text::Bold);
	Jugador4.setPosition(0, 560);

	sf::Text Jugador1Score(std::to_string(player1._score), font, 12);
	Jugador1Score.setFillColor(sf::Color(60, 60, 200));
	Jugador1Score.setStyle(sf::Text::Bold);
	Jugador1Score.setPosition(0, 560);

	sf::Text Jugador2Score(std::to_string(player2._score), font, 12);
	Jugador2Score.setFillColor(sf::Color(60, 60, 200));
	Jugador2Score.setStyle(sf::Text::Bold);
	Jugador2Score.setPosition(0, 560);

	sf::Text Jugador3Score(std::to_string(player3._score), font, 12);
	Jugador3Score.setFillColor(sf::Color(60, 60, 200));
	Jugador3Score.setStyle(sf::Text::Bold);
	Jugador3Score.setPosition(0, 560);

	sf::Text Jugador4Score(std::to_string(player4._score), font, 12);
	Jugador4Score.setFillColor(sf::Color(60, 60, 200));
	Jugador4Score.setStyle(sf::Text::Bold);
	Jugador4Score.setPosition(0, 560);

	sf::Text Temps(std::to_string(10), font, 14);
	Temps.setFillColor(sf::Color(60, 60, 200));
	Temps.setStyle(sf::Text::Bold);
	Temps.setPosition(0, 560);


	/////////////////////////////////////////////////////////////////////////

	

	//Send sender;
	//sender.send = &socket;
	//sender.command = &command;
	//Receive receiver;
	//receiver.socket = &socket;
	//// bind puerto 50000 al socket receive
	//sf::Socket::Status status = socket.connect(ip, 5000, sf::seconds(5.f));
	//if (status != sf::Socket::Done) {
	//	std::cout << "Error al intent de conexió" << std::endl;
	//	return -1;
	//}
	// OPEN CHAT WINDOW
	
	//receiver.command = &command;
	sf::Vector2i screenDimensions(918, 516); //Dimensions pantalles

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Buzz");

	//Pintem els quadres

	//Horitzontals
	sf::RectangleShape separator0(sf::Vector2f(800, 5));
	separator0.setFillColor(sf::Color(200, 200, 200, 255));
	separator0.setPosition(0, 100);

	sf::RectangleShape separator1(sf::Vector2f(800, 5));
	separator1.setFillColor(sf::Color(200, 200, 200, 255));
	separator1.setPosition(0, 200);

	sf::RectangleShape separator2(sf::Vector2f(800, 5));
	separator2.setFillColor(sf::Color(200, 200, 200, 255));
	separator2.setPosition(0, 550);
	

	//Verticals
	sf::RectangleShape separator3(sf::Vector2f(5, 600));
	separator3.setFillColor(sf::Color(200, 200, 200, 255));
	separator3.setPosition(400, 0);

	sf::RectangleShape separator4(sf::Vector2f(5, 100));
	separator4.setFillColor(sf::Color(200, 200, 200, 255));
	separator4.setPosition(600, 0);
	

	//Enviem el nostre nom
	//command = protocol.CreateMessage(4,0,0,player._name);
	//sender.SendMessages();

	//receiver.ReceiveMessages(); //Rebem el nostre numero de jugador
	//player._num = protocol.GetPlayer(command);
	//
	//receiver.ReceiveMessages(); //Rebem oponent
	//Opponent._num= protocol.GetPlayer(command);
	//Opponent._name= protocol.GetWord(command);
	//
	//socket.setBlocking(false);

	while (window.isOpen())
	{
		sf::Event evento;
		switch (state) {
		case send: // rebre la paraula i començar el temps

			//receiver.ReceiveMessages();
			//word = protocol.GetWord(command); //Rebem la paraula

			socket.setBlocking(false);
			state = play;
			time += std::clock() + MAXTIME;
			break;
		case play:

			if (time > std::clock()) { // s'acaba el temps
				//Enviem missatge amb la paraula escrita
			}
			while (window.pollEvent(evento))
			{

			//	if (player._log.size() > 0) {
				//	if (player._log[player._log.size() - 1] == "$") {
				//	window.close();
				//	}
				////}
				//switch (evento.type)
				//{
				//case sf::Event::Closed:
				//	window.close();
				//	break;
				//case sf::Event::KeyPressed:
				//	if (evento.key.code == sf::Keyboard::Escape) {
				//		player._word.clear();
				//		player._word = sf::Keyboard::Escape; // Escape== $
				//		sender.SendMessages(); // envia mensaje
				//		window.close();
				//	}
				//	else if (evento.key.code == sf::Keyboard::Return)
				//	{
				//		sender.SendMessages(); // envia mensaje
				//		player._word = player._name + ": ";
				//	}
				//	break;
				//case sf::Event::TextEntered:
				//	if (evento.text.unicode >= 32 && evento.text.unicode <= 126) // si di dona a les tecles, escriu el misatge
				//		player._word += (char)evento.text.unicode;
				//	else if (evento.text.unicode == 8 && player._word.size() > 0) // si li dona al backspave, borra ultima lletra
				//		player._word.erase(player._word.size() - 1, player._word.size());
				//	break;
				//	}
			}
			break;
		case points:

			break;
		case win:

			break; 
		}

	
	


	//	std::string mensaje_ = player._word + "_";
		//Text.setString(mensaje_);

		/////////////////// PINTAT
		///
		window.draw(sprite);

		/// TEXT
		window.draw(Pregunta);
		window.draw(Resposta1);
		window.draw(Resposta2);
		window.draw(Resposta3);
		window.draw(Resposta4);
		window.draw(Jugador1);
		window.draw(Jugador2);
		window.draw(Jugador3);
		window.draw(Jugador4);
		window.draw(Jugador1Score);
		window.draw(Jugador2Score);
		window.draw(Jugador3Score);
		window.draw(Jugador4Score);
		window.draw(Temps);
		
		window.display();
		window.clear();
	}
	socket.disconnect();
	return 0;
}