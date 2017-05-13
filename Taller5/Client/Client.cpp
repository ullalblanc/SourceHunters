#include <cstring>
#include <iostream>
#include <time.h>

#include "Game.h"

#define SpriteVelocityBot 0.05
#define SpriteVelocityTop 0.05
#define SpriteVelocityBlock 0.07

#define DISTANCIA_BODY 70
#define DISTANCIA_ATTACK 150

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
	sf::Mutex mutex;											// Per evitar varis accesos a les cues
	std::string command;										// el misatge que envia als clients
	Send sender;												// Sender per enviar misatges
	ClientReceive receiver;										// Receiver per rebre constanment misatges
	sf::Thread thread(&ClientReceive::ReceiveCommands, &receiver);	// Thread per el receiver
	std::vector<Player> player;									// Vector de jugadors

	sender.command = &command;
	sender.socket = &socket;

	receiver.commands = &serverCommands;
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
	std::vector<Accum> accum;	// Vector dels acumulats de moviment
	std::queue<Accum> enemyAccum;// Cua dels acumulats de moviment del contrincant

	playertmp.y = 750;
	player.push_back(playertmp);
	player.push_back(playertmp);
	Accum accumtmp; accumtmp.moveId = 0;
	accum.push_back(accumtmp);

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


	// set up AnimatedSprite
	AnimatedSprite p1Top(sf::seconds(SpriteVelocityTop), true, false); //(sf::Time frameTime, bool paused, bool looped)
	p1Top.setPosition(sf::Vector2f(player[0].x - 325, player[0].y));
	p1Top.play(idleAnimation1T);

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


	// set up AnimatedSprite
	AnimatedSprite p1Bot(sf::seconds(SpriteVelocityBot), true, false); //(sf::Time frameTime, bool paused, bool looped)
	p1Bot.setPosition(sf::Vector2f(player[0].x - 325, player[0].y + 275)); //275 d'alçada per compensar amb la cintura
	p1Bot.play(idleAnimation1B);

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
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 1, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 2, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 3, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 4, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 5, 0, 650, 650));//5
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 6, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 7, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 8, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 9, 0, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 0, 652, 650, 650));//10
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 1, 652, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 2, 652, 650, 650));
	attackAnimationTop2T.addFrame(sf::IntRect(652 * 3, 652, 650, 650));

	//Animacio attack mid player 1
	Animation attackAnimationMid2T;
	attackAnimationMid2T.setSpriteSheet(p2TextTop);
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

	// set up AnimatedSprite
	AnimatedSprite p2Top(sf::seconds(SpriteVelocityTop), true, false); //(sf::Time frameTime, bool paused, bool looped)
	p2Top.setPosition(sf::Vector2f(player[1].x - 325, player[1].y));
	p2Top.play(idleAnimation2T);

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

	//Animation* currentAnimation1B = &idleAnimation1B;

	// set up AnimatedSprite
	AnimatedSprite p2Bot(sf::seconds(SpriteVelocityBot), true, false); //(sf::Time frameTime, bool paused, bool looped)
	p2Bot.setPosition(sf::Vector2f(player[1].x - 325, player[1].y + 275)); //275 d'alçada per compensar amb la cintura
	p2Bot.play(idleAnimation2B);

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

	sf::Font font;
	if (!font.loadFromFile("cour.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	sf::Vector2i screenDimensions(1600, 900);											// Dimensions pantalles
	sf::RenderWindow window;															// Creem la finestra del joc
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Aoi Samurai");	// Obrim la finestra del joc
	window.setFramerateLimit(60); //FrameRate

	thread.launch();																	// Comencem thread receive

	while (window.isOpen())
	{
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

			if (!serverCommands.empty()) {
				int serverCase = 0; 
				serverCommands.front().Read(&serverCase, TYPE_SIZE);
				//std::cout << serverCase << std::endl;
				switch (serverCase) {

				case HELLO: {
					serverCommands.front().Read(&player[0].id, ID_SIZE);
					serverCommands.front().Read(&player[0].x, POSITION_SIZE);
					serverCommands.pop();
				}
					break;

				case CONNECTION: {
					serverCommands.front().Read(&player[1].id, ID_SIZE);
					serverCommands.front().Read(&player[1].x, POSITION_SIZE);
					serverCommands.pop();

					
					break;
				}
				}
			}
			if (player[0].x != 0 && player[1].x != 0)
			{
				OutputMemoryBitStream output;
				output.Write(CONNECTION, TYPE_SIZE);
				output.Write(player[0].id, ID_SIZE);
				sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
				state = play;
				std::cout << "play " << player[0].x << " " << player[1].x << std::endl;
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
				player[0].x += movement;
				accum.back().moveDelta += movement;
				
				// TODO: vigilar que no surtin de el mapa ni xoquin amb el enemic
				p1Bot.play(pas1B);

			}
			if (key.isKeyPressed(sf::Keyboard::Left)) {
				int movement = -2;
				player[0].x += 2;
				accum.back().moveDelta += movement;
				p1Top.play(attackAnimationTop1T);

			}

			//-- ACCUMULATED --//

			////-- PLAYER --////

			if (timerAccum.Check())
			{
				if (accum.back().moveDelta != 0)
				{
					accum.back().moveAbsolute = player[0].x;			// Marco el absolut del moviment
					OutputMemoryBitStream output;
					output.Write(MOVEMENT, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(accum.back().moveId, ACCUM_ID_SIZE);
					output.Write(accum.back().moveDelta, ACCUM_DELTA_SIZE);
					// TODO: Write de si hi ha animacio o no
					Accum accumtmp;										// Creo nou acumulat
					if (accum.back().moveId == 15) accumtmp.moveId = 0;	// Si el ultim acumulat te id 15, el nou torna a 0
					else accumtmp.moveId = accum.back().moveId + 1;     // Sino, el id es un mes que l'anterior
					// TODO: enviar acumulat
					// TODO: vigilar que el acumulat no pasi de +/- 64
					
					accum.push_back(accumtmp);
				}	
				timerAccum.Start(ACCUMTIME);
			}

			////-- ENEMY --////

			// TODO: simular enemic
			if (!enemyAccum.empty())
			{
				// enemyAccum.front()
			}

			//-- COMMANDS --//

			if (!serverCommands.empty()) {
				int serverCase = 0; 
				serverCommands.front().Read(&serverCase, TYPE_SIZE);
				switch (serverCase) {

				case HELLO: { // NO TINDRIA QUE REBRE 1
				}
					break;

				case CONNECTION: {
					OutputMemoryBitStream output;
					output.Write(CONNECTION, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					serverCommands.pop();
					break;
				}
				case PING: {
					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					command = "3" + std::to_string(player[0].id);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					serverCommands.pop();
					break;
				}
				case MOVEMENT: {

					int playerId = 0; 
					serverCommands.front().Read(&playerId, ID_SIZE);				// Guardem quin jugador es
					int accumId = 0; 
					serverCommands.front().Read(&accumId, ACCUM_ID_SIZE);			// Guardem la id del acumulat
					int accumDelta = 0; 
					serverCommands.front().Read(&accumDelta, ACCUM_DELTA_SIZE);	// Guardem el el delta acumulat

					if (playerId == player[0].id)				// Si es el id propi, comfirma el moviment
					{	// TODO: Check de trampas o problemes
						for (int i = 0; i < accum.size(); i++)	// Recorre tots els misatges de acumulacio
						{
							if (accum[i].moveId == accumId)		// Si troba el misatge de acumulacio
							{
								for (int j = 0; j < accum.size()-i; j++)		// Recorre els misatges que hi havien fins ara
								{
									accum.erase(accum.begin());					// Borrals
								}
								break;
							}
						}
					} 
					else							// Si es el id del contrincant, simula el moviment
					{
						Accum accumtmp;
						accumtmp.moveId = accumId;
						accumtmp.moveDelta = accumDelta;
						enemyAccum.push(accumtmp);	// Afegir acumulat a la cua
					}

					serverCommands.pop();
					break;
				}							  
				default:
					break;

				}
			}
		}
			break;

		case points: {
		}
			break;
		case win: {

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
		p1Bot.setPosition(player[0].x, 150 + 275);
		window.draw(p1Bot);
		//p1Top.play(*currentAnimation1T);
		p1Top.update(frameTime);
		p1Top.setPosition(player[0].x, 150);
		window.draw(p1Top);

		//P2
		p2Bot.update(frameTime);
		p2Bot.setPosition(player[1].x + 150, player[1].y + 275); //Aquests 150 en x son la desviació del sprite de les cames
		window.draw(p2Bot);

		p2Top.update(frameTime);
		p2Top.setPosition(player[1].x, player[1].y);
		window.draw(p2Top);// pintem el jugador

		window.draw(herba);

		window.display();		// Mostrem per la finestra
		window.clear();			// Netejem finestra
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}