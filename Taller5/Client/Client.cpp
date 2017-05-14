#include <cstring>
#include <iostream>
#include <time.h>

#include "Game.h"

#define SpriteVelocityBot 0.05
#define SpriteVelocityTop 0.05
#define SpriteVelocityBlock 0.07

#define POSICIO_INICIAL1 270
#define POSICIO_INICIAL2 800
#define POSICIO_Y 150

#define DISTANCIA_BODY 70


// Protocol: https://docs.google.com/spreadsheets/d/152EPpd8-f7fTDkZwQlh1OCY5kjCTxg6-iZ2piXvEgeg/edit?usp=sharing

enum State {
	connect,	// Per conectarse al servidor
	send,		// enviar paraula nova y que comenci partida
	play,		// mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points,		// Envia les puntuacions als jugadors y actualitza els seus logs
	win			// el joc sacaba
};

int main()
{
	//-- UDP --//

	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1"); //sf::IpAddress::getLocalAddress();
	unsigned short serverPort = 5000;
	sf::UdpSocket socket;
	std::queue<InputMemoryBitStream> serverCommands;			// Misatges del servidor per anar executant
	std::queue<Command> com;
	sf::Mutex mutex;											// Per evitar varis accesos a les cues
	std::string command;										// el misatge que envia als clients
	Send sender;												// Sender per enviar misatges
	ClientReceive receiver;										// Receiver per rebre constanment misatges
	sf::Thread thread(&ClientReceive::ReceiveCommands, &receiver);	// Thread per el receiver
	std::vector<Player> player;									// Vector de jugadors

	sender.command = &command;
	sender.socket = &socket;

	receiver.commands = &serverCommands;
	receiver.com = &com;
	receiver.socket = &socket;
	receiver.mutex = &mutex;
	receiver.players = &player;

	std::cout << "Port: ";										// Demanem port al client
	unsigned short port;
	std::cin >> port;											// Guardem el port del client
	sf::Socket::Status status = socket.bind(port);				// Bind al port del client
	if (status != sf::Socket::Done) {							// Si falla el bind, acaba el programa
		std::cout << "Error al intent de bind" << std::endl;
		return -1;
	}

	//-- CLIENT --//

	Timer timerConnect;			// timer per intentar conectarse a servidor
	Timer timerAccum;			// timer per el acumulats de moviment
	State state = connect;		// Comencem en connect per que es conecti al server
	Player playertmp;			// Amb el tmp es guardara a ell mateix i als altres en el vector player
	//std::vector<Accum> accum;	// Vector dels acumulats de moviment
	//std::queue<Accum> enemyAccum;// Cua dels acumulats de moviment del contrincant

	playertmp.y = 150;
	player.push_back(playertmp);
	player.push_back(playertmp);
	Accum accumtmp; accumtmp.id = 0;
	//accum.push_back(accumtmp);
	player[0].accum.push_back(accumtmp);

	//-- GAME --//	

	////-- SPRITES --////

	//fons
	sf::Texture texture;
	if (!texture.loadFromFile("../Resources/Fons.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite fons; // fons
	fons.setTexture(texture);

	//gespa
	sf::Texture gespaText;
	if (!gespaText.loadFromFile("../Resources/front.png")) { //FALLA AQUI
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite herba; // fons
	herba.setTexture(gespaText);
	herba.setPosition(0, 600);

	//boira
	sf::Texture BoiraText;
	if (!BoiraText.loadFromFile("../Resources/moviment.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite Boira; // fons
	Boira.setTexture(BoiraText);
	sf::Sprite Boira2; // fons
	Boira2.setTexture(BoiraText);

	//PLAYER 1 
	//TOP

	sf::Texture p1TextTop;
	if (!p1TextTop.loadFromFile("../Resources/SpriteEsquerrav1.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	//Animation idle
	Animation idleAnimation1T;
	idleAnimation1T.setSpriteSheet(p1TextTop);
	idleAnimation1T.addFrame(sf::IntRect(0, 0, 650, 650));

	//Animacio attack top player 1
	Animation attackAnimationTop1T;
	attackAnimationTop1T.setSpriteSheet(p1TextTop);
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 0, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 1, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 2, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 3, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 4, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 5, 0, 650, 650));//5
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 6, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 7, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 8, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 9, 0, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 0, 652, 650, 650));//10
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 1, 652, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 2, 652, 650, 650));
	attackAnimationTop1T.addFrame(sf::IntRect(652 * 3, 652, 650, 650));

	//Animacio attack mid player 1
	Animation attackAnimationMid1T;
	attackAnimationMid1T.setSpriteSheet(p1TextTop);
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 0, 0, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 4, 652, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 5, 652, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 6, 652, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 7, 652, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 8, 652, 650, 650));//5
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 9, 652, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 0, 652 * 2, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 1, 652 * 2, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 2, 652 * 2, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 3, 652 * 2, 650, 650));//10
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 4, 652 * 2, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 5, 652 * 2, 650, 650));
	attackAnimationMid1T.addFrame(sf::IntRect(652 * 6, 652 * 2, 650, 650));

	//Animacio attack bot player 1
	Animation attackAnimationBot1T;
	attackAnimationBot1T.setSpriteSheet(p1TextTop);
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 0, 652 * 0, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 7, 652 * 2, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 8, 652 * 2, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 9, 652 * 2, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 0, 652 * 3, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 1, 652 * 3, 650, 650));//5
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 2, 652 * 3, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 3, 652 * 3, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 4, 652 * 3, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 5, 652 * 3, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 6, 652 * 3, 650, 650));//10
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 7, 652 * 3, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 8, 652 * 3, 650, 650));
	attackAnimationBot1T.addFrame(sf::IntRect(652 * 9, 652 * 3, 650, 650));

	//Animacio Bloqueig
	Animation BlockAnimation1T;
	BlockAnimation1T.setSpriteSheet(p1TextTop);
	BlockAnimation1T.addFrame(sf::IntRect(652 * 2, 0, 650, 650));
	BlockAnimation1T.addFrame(sf::IntRect(652 * 3, 0, 650, 650));
	BlockAnimation1T.addFrame(sf::IntRect(652 * 4, 0, 650, 650));
	BlockAnimation1T.addFrame(sf::IntRect(652 * 5, 0, 650, 650));
	BlockAnimation1T.addFrame(sf::IntRect(652 * 6, 0, 650, 650));//5
	BlockAnimation1T.addFrame(sf::IntRect(652 * 7, 0, 650, 650));
	BlockAnimation1T.addFrame(sf::IntRect(652 * 8, 0, 650, 650));


	//Animation* currentAnimation1T = &idleAnimation1T;


	//Bot
	sf::Texture p1TextBot;
	if (!p1TextBot.loadFromFile("../Resources/PassosEsq.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	//Animation idle
	Animation idleAnimation1B;
	idleAnimation1B.setSpriteSheet(p1TextBot);
	idleAnimation1B.addFrame(sf::IntRect(0, 0, 500, 380));

	//Animation Pas Ofensiu
	Animation pas1B;
	pas1B.setSpriteSheet(p1TextBot);
	pas1B.addFrame(sf::IntRect(502 * 1, 382 * 0, 500, 380));
	pas1B.addFrame(sf::IntRect(502 * 0, 382 * 1, 500, 380));
	pas1B.addFrame(sf::IntRect(502 * 1, 382 * 1, 500, 380));
	pas1B.addFrame(sf::IntRect(502 * 0, 382 * 2, 500, 380));
	pas1B.addFrame(sf::IntRect(502 * 1, 382 * 2, 500, 380));//-·--·--·--
	pas1B.addFrame(sf::IntRect(502 * 0, 382 * 2, 500, 380));
	pas1B.addFrame(sf::IntRect(502 * 1, 382 * 1, 500, 380));
	pas1B.addFrame(sf::IntRect(502 * 0, 382 * 1, 500, 380));
	pas1B.addFrame(sf::IntRect(502 * 1, 382 * 0, 500, 380));

	//Animation* currentAnimation1B = &idleAnimation1B;


	//Jugador 2 
	//TOP

	sf::Texture p2TextTop;
	if (!p2TextTop.loadFromFile("../Resources/SpriteJugadorDreta.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	//Animation idle
	Animation idleAnimation2T;
	idleAnimation2T.setSpriteSheet(p2TextTop);
	idleAnimation2T.addFrame(sf::IntRect(0, 0, 650, 650));

	//Animacio attack top player 1
	Animation attackAnimationTop2T;
	attackAnimationTop2T.setSpriteSheet(p2TextTop);
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 0, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 1, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 2, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 3, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 4, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 5, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 6, 0, 650, 650));//5
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 7, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 8, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 9, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 0, 652, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 1, 652, 650, 650));//10
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 2, 652, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 3, 652, 650, 650));

	//Animacio attack mid player 1
	Animation attackAnimationMid2T;
	attackAnimationMid2T.setSpriteSheet(p2TextTop);
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 0, 0, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 4, 652, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 5, 652, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 6, 652, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 7, 652, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 8, 652, 650, 650));//5
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 9, 652, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 0, 652 * 2, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 1, 652 * 2, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 2, 652 * 2, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 3, 652 * 2, 650, 650));//10
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 4, 652 * 2, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 5, 652 * 2, 650, 650));
	attackAnimationMid2T.addFrame(sf::IntRect(652 * 6, 652 * 2, 650, 650));


	//Animacio attack bot player 1
	Animation attackAnimationBot2T;
	attackAnimationBot2T.setSpriteSheet(p2TextTop);
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 0, 652 * 0, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 7, 652 * 2, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 8, 652 * 2, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 9, 652 * 2, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 0, 652 * 3, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 1, 652 * 3, 650, 650));//5
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 2, 652 * 3, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 3, 652 * 3, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 4, 652 * 3, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 5, 652 * 3, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 6, 652 * 3, 650, 650));//10
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 7, 652 * 3, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 8, 652 * 3, 650, 650));
	attackAnimationBot2T.addFrame(sf::IntRect(652 * 9, 652 * 3, 650, 650));

	//Animacio Bloqueig
	Animation BlockAnimation2T;
	BlockAnimation2T.setSpriteSheet(p2TextTop);
	BlockAnimation2T.addFrame(sf::IntRect(652 * 2, 0, 650, 650));
	BlockAnimation2T.addFrame(sf::IntRect(652 * 3, 0, 650, 650));
	BlockAnimation2T.addFrame(sf::IntRect(652 * 4, 0, 650, 650));
	BlockAnimation2T.addFrame(sf::IntRect(652 * 5, 0, 650, 650));
	BlockAnimation2T.addFrame(sf::IntRect(652 * 6, 0, 650, 650));//5
	BlockAnimation2T.addFrame(sf::IntRect(652 * 7, 0, 650, 650));
	BlockAnimation2T.addFrame(sf::IntRect(652 * 8, 0, 650, 650));

	//Animation* currentAnimation1T = &idleAnimation2T;


	//Bot
	sf::Texture p2TextBot;
	if (!p2TextBot.loadFromFile("../Resources/PassosDreta.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	//Animation idle
	Animation idleAnimation2B;
	idleAnimation2B.setSpriteSheet(p2TextBot);
	idleAnimation2B.addFrame(sf::IntRect(0, 0, 500, 380));

	//Animation Pas Ofensiu
	Animation pas2B;
	pas2B.setSpriteSheet(p2TextBot);
	pas2B.addFrame(sf::IntRect(502 * 1, 382 * 0, 500, 380));
	pas2B.addFrame(sf::IntRect(502 * 0, 382 * 1, 500, 380));
	pas2B.addFrame(sf::IntRect(502 * 1, 382 * 1, 500, 380));
	pas2B.addFrame(sf::IntRect(502 * 0, 382 * 2, 500, 380));
	pas2B.addFrame(sf::IntRect(502 * 1, 382 * 2, 500, 380));//-·--·--·--
	pas2B.addFrame(sf::IntRect(502 * 0, 382 * 2, 500, 380));
	pas2B.addFrame(sf::IntRect(502 * 1, 382 * 1, 500, 380));
	pas2B.addFrame(sf::IntRect(502 * 0, 382 * 1, 500, 380));
	pas2B.addFrame(sf::IntRect(502 * 1, 382 * 0, 500, 380));

	//Partycle System

	sf::Texture PSText;
	if (!PSText.loadFromFile("../Resources/AtlasPS.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}

	Animation PSBasic;
	PSBasic.setSpriteSheet(PSText);
	PSBasic.addFrame(sf::IntRect(400 * 2, 400 * 1, 400, 400));
	PSBasic.addFrame(sf::IntRect(400 * 0, 400 * 0, 400, 400));
	PSBasic.addFrame(sf::IntRect(400 * 1, 400 * 0, 400, 400));
	PSBasic.addFrame(sf::IntRect(400 * 2, 400 * 0, 400, 400));
	PSBasic.addFrame(sf::IntRect(400 * 3, 400 * 0, 400, 400));
	PSBasic.addFrame(sf::IntRect(400 * 0, 400 * 1, 400, 400));
	PSBasic.addFrame(sf::IntRect(400 * 1, 400 * 1, 400, 400));

	//Animation* currentAnimation1B = &idleAnimation1B;	

	AnimatedSprite p1Top(sf::seconds(SpriteVelocityTop), true, false); //(sf::Time frameTime, bool paused, bool looped)
	AnimatedSprite p1Bot(sf::seconds(SpriteVelocityBot), true, false); //(sf::Time frameTime, bool paused, bool looped)
	AnimatedSprite p2Bot(sf::seconds(SpriteVelocityBot), true, false); //(sf::Time frameTime, bool paused, bool looped)
	AnimatedSprite p2Top(sf::seconds(SpriteVelocityTop), true, false); //(sf::Time frameTime, bool paused, bool looped)
	AnimatedSprite PSAnimated(sf::seconds(SpriteVelocityBot), false, false); //(sf::Time frameTime, bool paused, bool looped)

	int FoggOffset = 0;
	int DireccioAtacJugador1 = 0; // 0=Idle 1=Top 2=Mid 3=Bot
	int DireccioAtacJugador2 = 0; // 0=Idle 1=Top 2=Mid 3=Bot

	bool distancia = false;
	bool distanciaAtac = false;

	sf::Clock frameClock;//Preparem el temps

	sf::Texture texture3;
	if (!texture3.loadFromFile("../Resources/Fucsia.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite fucsia; // enemic
	fucsia.setTexture(texture3);
	fucsia.setOrigin(20, 20);

	//TEXT
	sf::Font font;
	if (!font.loadFromFile("../Resources/Samurai.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}
	sf::Text text1(/*std::to_string(puntsJugador1)*/"0", font, 50); //Aqui va la variable de puntuacio de cada jugador
	//text1.setColor(sf::Color::White);
	text1.setPosition(150, 750);
	sf::Text text2(/*std::to_string(puntsJugador2)*/"0", font, 50);
	//text2.setColor(sf::Color::White);
	text2.setPosition(1450, 750);
	sf::Text PointText("", font, 100);
	PointText.setPosition(300, 250);
	sf::Text Instructions("Prem 'ENTER' per iniciar la propera ronda", font, 30);
	Instructions.setPosition(300, 750);

	sf::Vector2i screenDimensions(1600, 900);											// Dimensions pantalles
	sf::RenderWindow window;															// Creem la finestra del joc
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Aoi Samurai");	// Obrim la finestra del joc
	window.setFramerateLimit(60); //FrameRate

	thread.launch();																	// Comencem thread receive

	int left = 0;
	int right = 1;

	while (window.isOpen())
	{
		sf::Event event; //Si no la finestra no detecta el ratolí i no es pot moure
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
		}

		sf::Time frameTime = frameClock.restart();
		switch (state) {
		case connect: {

			if (timerConnect.Check()) {
				if (player[0].x == 0) {
					OutputMemoryBitStream output;
					output.Write(HELLO, TYPE_SIZE);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					
					//timerConnect.Stop();
				}
				timerConnect.Start(5000);
			}

			if (!com.empty()) {
				switch (com.front().type) {

				case HELLO: {

					player[0].id = com.front().id;
					player[0].x = com.front().position;

					if (player[0].id == 1)
					{
						left = 1;
						right = 0;
					}

					com.pop();
				}
					break;

				case CONNECTION: {
					player[1].id = com.front().id;
					player[1].x = com.front().position;

					// set up AnimatedSprite	
					p1Top.setPosition(sf::Vector2f(player[left].x /*- 325*/, player[0].y));
					p1Top.play(idleAnimation1T);

					// set up AnimatedSprite					
					p1Bot.setPosition(sf::Vector2f(player[left].x /*- 325*/, player[0].y + 275)); //275 d'alçada per compensar amb la cintura
					p1Bot.play(idleAnimation1B);

					// set up AnimatedSprite					
					p2Bot.setPosition(sf::Vector2f(player[right].x /*- 325*/, player[1].y + 275)); //275 d'alçada per compensar amb la cintura
					p2Bot.play(idleAnimation2B);

					// set up AnimatedSprite
					p2Top.setPosition(sf::Vector2f(player[right].x /*- 325*/, player[1].y));
					p2Top.play(idleAnimation2T);

					com.pop();

				}
					break;
				}
			}
			if (player[0].x != 0 && player[1].x != 0)
			{
				OutputMemoryBitStream output;
				output.Write(CONNECTION, TYPE_SIZE);
				output.Write(player[0].id, ID_SIZE);
				sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
				state = play;
				//std::cout << "play " << player[0].x << " " << player[1].x << std::endl;
			}
		}
			break;

		case send: {}
			break;

		case play: {

			//-- MOVEMENT --//

			sf::Keyboard key;
			if (key.isKeyPressed(sf::Keyboard::Right)) {
				int movement = 2;
				int distance = player[1].x - (player[0].x + movement);
				if (distance < 0) distance = -distance;
				if (distance > DISTANCIA_BODY)
				{
					if ((player[0].x + movement) < RIGHT_LIMIT)
					{
						player[0].x += movement;
						player[0].accum.back().delta += movement;
					}
				}
				// TODO: vigilar que no xoquin amb el enemic
				//p1Bot.play(pas1B);

			}
			if (key.isKeyPressed(sf::Keyboard::Left)) {
				int movement = -2;
				int distance = player[1].x - (player[0].x + movement);
				if (distance < 0) distance = -distance;
				if (distance > DISTANCIA_BODY)
				{
					if ((player[0].x + movement) > LEFT_LIMIT)
					{
						player[0].x += movement;
						player[0].accum.back().delta += movement;
					}
				}
				//p1Top.play(attackAnimationTop1T);
			}

			if (key.isKeyPressed(sf::Keyboard::Z) && player[0].attack == 0) {
				if (!left) {
					p1Top.play(attackAnimationTop1T);
				}
				else {
					p2Top.play(attackAnimationTop2T);
				}
				player[0].attack = 1;

				OutputMemoryBitStream output;
				output.Write(ATTACK, TYPE_SIZE);
				output.Write(player[0].id, ID_SIZE);
				output.Write(player[0].attack, ATTACK_SIZE);

				sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
			}

			if (key.isKeyPressed(sf::Keyboard::X) && player[0].attack == 0) {
				if (!left) {
					p1Top.play(attackAnimationMid1T);
				}
				else {
					p2Top.play(attackAnimationMid2T);
				}
				player[0].attack = 2;

				OutputMemoryBitStream output;
				output.Write(ATTACK, TYPE_SIZE);
				output.Write(player[0].id, ID_SIZE);
				output.Write(player[0].attack, ATTACK_SIZE);

				sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
			}

			if (key.isKeyPressed(sf::Keyboard::C) && player[0].attack == 0) {
				if (!left) {
					p1Top.play(attackAnimationBot1T);
				}
				else {
					p2Top.play(attackAnimationBot2T);
				}
				player[0].attack = 3;

				OutputMemoryBitStream output;
				output.Write(ATTACK, TYPE_SIZE);
				output.Write(player[0].id, ID_SIZE);
				output.Write(player[0].attack, ATTACK_SIZE);

				sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
			}

			if (player[0].attack != 0)
			{
				if (!left) 
				{
					if (p1Top.m_currentFrame == 12) {
						OutputMemoryBitStream output;
						output.Write(ATTACK, TYPE_SIZE);
						output.Write(player[0].id, ID_SIZE);
						output.Write(player[0].attack, ATTACK_SIZE);

						sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
						player[0].attack = 0;
					}
					
				}
				else
				{
					if (p2Top.m_currentFrame == 12) {
						OutputMemoryBitStream output;
						output.Write(ATTACK, TYPE_SIZE);
						output.Write(player[0].id, ID_SIZE);
						output.Write(player[0].attack, ATTACK_SIZE);

						sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
						player[0].attack = 0;
					}
					
				}
				
			}
			// TODO: Attack

			//-- ACCUMULATED --//

			////-- PLAYER --////

			if (timerAccum.Check())
			{
				if (player[0].accum.back().delta != 0)
				{
					//int negative = 0; // 0 = positiu, 1 = negatiu
					if (player[0].accum.back().delta < 0) {
						player[0].accum.back().sign = 1;
						player[0].accum.back().delta = -player[0].accum.back().delta;
					}

					player[0].accum.back().absolute = player[0].x;			// Marco el absolut del moviment
					OutputMemoryBitStream output;
					output.Write(MOVEMENT, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(player[0].accum.back().id, ACCUM_ID_SIZE);
					output.Write(player[0].accum.back().sign, ID_SIZE);
					output.Write(player[0].accum.back().delta, ACCUM_DELTA_SIZE);
					output.Write(player[0].accum.back().absolute, POSITION_SIZE);

					//std::cout << "Enviat " << player[0].accum.back().delta << std::endl;

					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());

					Accum accumtmp;										// Creo nou acumulat
					if (player[0].accum.back().id == 15) accumtmp.id = 0;	// Si el ultim acumulat te id 15, el nou torna a 0
					else accumtmp.id = player[0].accum.back().id + 1;     // Sino, el id es un mes que l'anterior
					
					player[0].accum.push_back(accumtmp);
				}	
				timerAccum.Start(ACCUMTIME);
			}

			////-- ENEMY --////

			if (!player[1].accum.empty())
			{
				int movement = 2;
				if (player[1].accum.front().absolute != player[1].x)
				{
					if (player[1].accum.front().delta < 0) movement = -movement;
					player[1].x += movement;
				}
				else
				{
					player[1].accum.erase(player[1].accum.begin());
				}
			}

			//-- COMMANDS --//
			
			if (!com.empty()) {
				//int serverCase = 0; 
				//serverCommands.front().Read(&serverCase, TYPE_SIZE);
				switch (com.front().type) {

				case HELLO: { // NO TINDRIA QUE REBRE 1
				}
					break;

				case CONNECTION: {
					OutputMemoryBitStream output;
					output.Write(CONNECTION, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					com.pop();
					break;
				}
				case PING: {
					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					com.pop();
					break;
				}
				case MOVEMENT: {

					if (com.front().id == player[0].id)				// Si es el id propi, comfirma el moviment
					{	// TODO: Check de trampas o problemes
						for (int i = 0; i < player[0].accum.size(); i++)	// Recorre tots els misatges de acumulacio
						{
							if (player[0].accum[i].id == com.front().accum.id)		// Si troba el misatge de acumulacio
							{
								for (int j = 0; j < player[0].accum.size()-i; j++)		// Recorre els misatges que hi havien fins ara
								{
									player[0].accum.erase(player[0].accum.begin());					// Borrals
								}
								break;
							}
						}
					} 
					else							// Si es el id del contrincant, simula el moviment
					{
						Accum accumtmp = com.front().accum;
						//accumtmp.absolute = player[1].x + accumtmp.delta;
						player[1].accum.push_back(accumtmp);	// Afegir acumulat a la cua
					}

					com.pop();
					//serverCommands.pop();					
				}
				break;

				case ATTACK:
				{
					player[1].attack = com.front().position;
					if (!left) {
						if (player[1].attack == 1)
						{
							p2Top.play(attackAnimationTop2T);
							p2Top.m_currentFrame++;
						}
						else if (player[1].attack == 2)
						{
							p2Top.play(attackAnimationMid2T);
							p2Top.m_currentFrame++;
						}
						else if (player[1].attack == 3)
						{
							p2Top.play(attackAnimationBot2T);
							p2Top.m_currentFrame++;
						}												
					}
					else {
						if (player[1].attack == 1)
						{
							p1Top.play(attackAnimationTop1T);
							p1Top.m_currentFrame++;
						}
						else if (player[1].attack == 2)
						{
							p1Top.play(attackAnimationMid1T);
							p1Top.m_currentFrame++;
						}
						else if (player[1].attack == 3)
						{
							p1Top.play(attackAnimationBot1T);
							p1Top.m_currentFrame++;
						}
					}
					com.pop();
				}
				break;

				case SCORE:
				{
					player[com.front().id].score++;
					
					text1.setString(std::to_string(player[0].score));
					text2.setString(std::to_string(player[1].score));

					if (!left)
					{
						player[0].x = 270;
						player[1].x = 800;
					} 
					else 
					{
						player[1].x = 270;
						player[0].x = 800;
					}					

					player[0].attack = 0;
					player[1].attack = 0;
					p1Top.setAnimation(idleAnimation1T);
					p2Top.setAnimation(idleAnimation2T);

					timerAccum.Start(ACCUMTIME);

					for (int j = 0; j < player.size(); j++)
					{
						for (int i = 0; i < player[j].accum.size(); i++)
						{
							player[j].accum.erase(player[j].accum.begin());
						}					
					}				
					Accum accumtmp;
					player[0].accum.push_back(accumtmp);

					com.pop();
				}
				break;

				default:
					break;

				}
			}
		}
			break;
		}

		window.draw(fons);	// Pintem el fons
		FoggOffset++;
		if (FoggOffset * 0.3 >= 1600) {
			FoggOffset = 0;
		}
		else {
			Boira.setPosition(1600 - FoggOffset * 0.3, 0);
		}
		Boira2.setPosition(-FoggOffset * 0.3, 0);

		window.draw(Boira);
		window.draw(Boira2);
		//if (player.size() > 0) { 
		//p1Bot.play(*currentAnimation1B);

		//
		p1Bot.update(frameTime);
		p1Bot.setPosition(player[left].x, player[left].y + 275);
		window.draw(p1Bot);
		//p1Top.play(*currentAnimation1T);
		p1Top.update(frameTime);
		p1Top.setPosition(player[left].x, player[left].y);
		window.draw(p1Top);

		//P2
		p2Bot.update(frameTime);
		p2Bot.setPosition(player[right].x + 150, player[right].y + 275); //Aquests 150 en x son la desviació del sprite de les cames
		window.draw(p2Bot);

		p2Top.update(frameTime);
		p2Top.setPosition(player[right].x , player[right].y);
		window.draw(p2Top);// pintem el jugador

		window.draw(herba);

		PSAnimated.update(frameTime); //Actualitzem el sistema de particules
		window.draw(PSAnimated); //Pintem el sistema de particules, s'ha de cridar amb PSAnimated.play(PSBasic)
		window.draw(text1); //Text de puntuacions
		window.draw(text2);

		if (state == points || state == win) { //Pintem el text si el estat es point o win
			window.draw(PointText);
			window.draw(Instructions);
		}

		window.display();		// Mostrem per la finestra
		window.clear();			// Netejem finestra
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}