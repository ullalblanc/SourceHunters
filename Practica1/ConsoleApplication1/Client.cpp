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

	std::clock_t time = std::clock(); // per els timers
	// CHOSE SEVER/CLIENT
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.23.87"); //sf::IpAddress::getLocalAddress();
	sf::TcpSocket socket;
	//std::string name;
	std::string textConsole = "Connected to: ";
	sf::Image background;
	if (!background.loadFromFile("../Resources/Fons.png"))
		return -1;

	
	std::string command; // el misatge que rep per saber que fer
	std::string word;

	Player player;
	Player Opponent;

	State state = send;

	sf::Font font;
	if (!font.loadFromFile("courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	std::cout << "Enter a name: ";
	
	std::cin >> player._name;

	//sf::RenderWindow window;
	//window.create(sf::VideoMode(960, 540), "Buzz");
    //window.draw(background);



	//sf::Text Text(player._word, font, 14);
	//sf::Text chatText(player._word, font, 12);
	//chatText.setFillColor(sf::Color(60, 60, 200));
	//Text.setFillColor(sf::Color(60, 60, 200));
	//chatText.setStyle(sf::Text::Bold);
	//Text.setStyle(sf::Text::Bold);
	//Text.setPosition(0, 560);

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
	sf::Vector2i screenDimensions(960, 540);

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
	command = protocol.CreateMessage(4,0,0,player._name);
	sender.SendMessages();

	receiver.ReceiveMessages(); //Rebem el nostre numero de jugador
	player._num = protocol.GetPlayer(command);
	
	receiver.ReceiveMessages(); //Rebem oponent
	Opponent._num= protocol.GetPlayer(command);
	Opponent._name= protocol.GetWord(command);
	
	//socket.setBlocking(false);

	while (window.isOpen())
	{
		
		sf::Event evento;
		switch (state) {
		case send: // rebre la paraula i començar el temps

			receiver.ReceiveMessages();
			word = protocol.GetWord(command); //Rebem la paraula

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
					//if (player._log[player._log.size() - 1] == "$") {
					//window.close();
					//}
				//}
				switch (evento.type)
				{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					if (evento.key.code == sf::Keyboard::Escape) {
						//player._word.clear();
						//player._word = sf::Keyboard::Escape; // Escape== $
						sender.SendMessages(); // envia mensaje
						window.close();
					}
					else if (evento.key.code == sf::Keyboard::Return)
					{
						sender.SendMessages(); // envia mensaje
						//player._word = player._name + ": ";
					}
					break;
				case sf::Event::TextEntered:
					if (evento.text.unicode >= 32 && evento.text.unicode <= 126) // si di dona a les tecles, escriu el misatge
						//player._word += (char)evento.text.unicode;
					//else if (evento.text.unicode == 8 && player._word.size() > 0) // si li dona al backspave, borra ultima lletra
						//player._word.erase(player._word.size() - 1, player._word.size());
					break;
					}
			}
			break;
		case points:

			break;
		case win:

			break; 
		}

		
		window.draw(separator0);
		window.draw(separator1);
		window.draw(separator2);
		window.draw(separator3);
		window.draw(separator4);


	/*	for (size_t i = 0; i < player._log.size(); i++)
		{
			std::string chatting = player._log[i];
			chatText.setPosition(sf::Vector2f(0, 20 * (float)i));
			chatText.setString(chatting);
			window.draw(chatText);
		}*/

		//std::string mensaje_ = player._word + "_";
		//Text.setString(mensaje_);
		//window.draw(Text);
		window.display();
		window.clear();
	}
	socket.disconnect();
	return 0;
}