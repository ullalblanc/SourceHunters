
#include "Chat.h"

Chat::Chat()
{
}

Chat::~Chat()
{
}

void Chat::StartChat()
{
	Init();
	Update();
}

void Chat::Sender()
{
	if (send.send(mensaje.c_str(), mensaje.size()) != sf::Socket::Done) {
		std::cout << "Error al enviar " << mensaje << std::endl;
	}
}

void Chat::Receiver()
{
	char data[100];
	//data[100];
	std::size_t received;
	sf::Socket::Status status = receive.receive(data, 100, received);
	aMensajes.push_back(data);// guarda mensaje a la llista de mensajes
	if (aMensajes.size() > 25) // si supera el maxim de lines per pantalla, baixala
	{
		aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
	}
}

void Chat::Init()
{
	// CHOSE SEVER/CLIENT
	ip = sf::IpAddress::IpAddress("192.168.23.87"); //sf::IpAddress::getLocalAddress();
	//std::string textConsole = "Connected to: ";

	std::cout << "Enter (s) for Server, Enter (c) for Client: ";
	std::cin >> connectionType;

	if (connectionType == 's')
	{
		sf::TcpListener listener;
		// Escuchamos por el puerto 50000
		if (listener.listen(50000) != sf::Socket::Done) {
			std::cout << "No et pots vincular al port 50000" << std::endl;
			return;
		}
		// puerto 50000 al socket send
		if (listener.accept(send) != sf::Socket::Done) {
			std::cout << "Error al acceptar conexió" << std::endl;
			return;
		}

		// Escuchamos por el puerto 50001
		if (listener.listen(50001) != sf::Socket::Done) {
			std::cout << "No et pots vincular al port 50001" << std::endl;
			return;
		}
		// puerto 50001 al socket receive
		if (listener.accept(receive) != sf::Socket::Done) {
			std::cout << "Error al acceptar conexió" << std::endl;
			return;
		}
		listener.close();
	}
	else if (connectionType == 'c')
	{
		// bind puerto 50000 al socket receive
		sf::Socket::Status status = receive.connect("127.0.0.1", 50000, sf::seconds(5.f));
		if (status != sf::Socket::Done) {
			std::cout << "Error l'intent de conexió" << std::endl;
			return;
		}
		// bind puerto 50001 al socket send
		status = send.connect("127.0.0.1", 50001, sf::seconds(5.f));
		if (status != sf::Socket::Done) {
			std::cout << "Error l'intent de conexió" << std::endl;
			return;
		}
	}
}

void Chat::Update()
{
	// OPEN CHAT WINDOW
	sf::Vector2i screenDimensions(800, 600);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");

	sf::Font font;
	if (!font.loadFromFile("../Resources/courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	mensaje = " >";

	sf::Text chattingText(mensaje, font, 14);
	sf::Text text(mensaje, font, 14);
	if (connectionType == 's') {
		chattingText.setFillColor(sf::Color(0, 160, 0));
		text.setFillColor(sf::Color(0, 160, 0));
	}
	else {
		chattingText.setFillColor(sf::Color(60, 60, 200));
		text.setFillColor(sf::Color(60, 60, 200));
	}

	chattingText.setStyle(sf::Text::Bold);

	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 560);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);

	sf::Thread thread(&Chat::Receiver);
	thread.launch();

	while (window.isOpen())
	{
		sf::Event evento;
		while (window.pollEvent(evento))
		{
			switch (evento.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Escape)
					window.close();
				else if (evento.key.code == sf::Keyboard::Return)
				{
					Sender(); // envia mensaje
					aMensajes.push_back(mensaje);
					if (aMensajes.size() > 25)
					{
						aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
					}
					mensaje = ">";
				}
				break;
			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126) // si di dona a les tecles, escriu el misatge
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.size() > 0) // si li dona al backspave, borra ultima lletra
					mensaje.erase(mensaje.size() - 1, mensaje.size());
				break;
			}
		}
		window.draw(separator);
		for (size_t i = 0; i < aMensajes.size(); i++)
		{
			std::string chatting = aMensajes[i];
			chattingText.setPosition(sf::Vector2f(0, 20 * i));
			chattingText.setString(chatting);
			window.draw(chattingText);
		}
		std::string mensaje_ = mensaje + "_";
		text.setString(mensaje_);
		window.draw(text);


		window.display();
		window.clear();
	}
}
