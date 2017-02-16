#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <cstring>
#include <iostream>

int main()
{
	sf::IpAddress ip = sf::IpAddress::IpAddress("192.168.23.154"); //sf::IpAddress::getLocalAddress();
	sf::TcpSocket socket;
	char connectionType, mode;
	char buffer[2000];
	std::size_t received;
	std::string text = "Connected to: ";

	std::cout << "Enter (s) for Server, Enter (c) for Client: ";
	std::cin >> connectionType;

	if (connectionType == 's')
	{
		sf::TcpListener listener;
		listener.listen(5000);
		listener.accept(socket);
		text += "Server";
		mode = 's';
		listener.close();
	}
	else if (connectionType == 'c')
	{
		socket.connect(ip, 5000);
		text += "Client";
		mode = 'r';
	}

	socket.send(text.c_str(), text.length() + 1);
	socket.receive(buffer, sizeof(buffer), received);

	std::cout << buffer << std::endl;

	bool done = false;
	while (!done)
	{
		if (mode == 's')
		{
			std::getline(std::cin, text);
			if (text.length() > 0)
			{
				socket.send(text.c_str(), text.length() + 1);
				mode = 'r';
				if (text == "exit")
				{
					break;
				}
			}
		}
		else if (mode == 'r')
		{
			socket.receive(buffer, sizeof(buffer), received);
			if (received > 0)
			{
				std::cout << "Received: " << buffer << std::endl;
				mode = 's';
				if (strcmp(buffer, "exit") == 0)
				{
					break;
				}
			}
		}
	}

	socket.disconnect();
	return 0;
}
//
//#include <SFML\Network.hpp>
//#include <string>
//#include <iostream>
//
//int main()
//{
//	char rol;
//	std::cout << "Server (s) o Client (c) ... ";
//	std::cin >> rol;
//
//
//	if (rol == 's' || rol == 'S')
//	{
//		sf::TcpListener listener;
//
//		//Hay un segundo par�metro. Ese segundo par�metro es la IP donde se aplica ese puerto. 
//		//Si no se indica cada, por defecto se escucha el puerto 50000 venga a trav�s de la ip que venga
//		sf::Socket::Status status = listener.listen(5000);
//		if (status != sf::Socket::Status::Done)
//		{
//			std::cout << "Error al escuchar por el puerto 5000." << std::endl;
//		}
//		sf::TcpSocket sock1, sock2;
//		status = listener.accept(sock1);
//		if (status != sf::Socket::Status::Done)
//		{
//			std::cout << "Error al aceptar conexion 1" << std::endl;
//		}
//		std::cout << "Llega cliente remoto de " << sock1.getRemoteAddress() << ":" << sock1.getRemotePort() << std::endl;
//		std::cout << "Me comunico a traves del puerto " << sock1.getLocalPort() << std::endl;
//
//		status = listener.accept(sock2);
//		if (status != sf::Socket::Status::Done)
//		{
//			std::cout << "Error al aceptar conexion 2" << std::endl;
//		}
//		std::cout << "Llega cliente remoto de " << sock2.getRemoteAddress() << ":" << sock2.getRemotePort() << std::endl;
//		std::cout << "Me comunico a traves del puerto " << sock2.getLocalPort() << std::endl;
//
//		std::string mensaje = "Todos conectados. Empieza la sesion!";
//
//		status = sock1.send(mensaje.c_str(), mensaje.length());
//		if (status != sf::Socket::Status::Done)
//		{
//			std::cout << "Error al enviar al cliente 1" << std::endl;
//		}
//		status = sock2.send(mensaje.c_str(), mensaje.length());
//		if (status != sf::Socket::Status::Done)
//		{
//			std::cout << "Error al enviar al cliente 2" << std::endl;
//		}
//
//		sock1.disconnect();
//		sock2.disconnect();
//	}
//	else if (rol == 'c' || rol == 'C')
//	{
//		sf::TcpSocket sock;
//		sf::Socket::Status status;
//		do {
//			status = sock.connect("127.0.0.1", 5000, sf::seconds(0.5f));
//			if (status != sf::Socket::Done)
//			{
//				std::cout << "Fallo el intento de conexion: " << status << " en el puerto: 5000" << std::endl;
//			}
//			else if (status == sf::Socket::Done)
//			{
//				std::cout << "Se conecto en 5000." << std::endl;
//				break;
//			}
//		} while (status != sf::Socket::Done);
//
//		std::cout << "Se conecto al servidor." << std::endl;
//
//		char data[100];
//		size_t sizeRecv;
//		status = sock.receive(data, 100, sizeRecv);
//		std::cout << "Status: " << status << std::endl;
//		if (status != sf::Socket::Done)
//		{
//			std::cout << "Error al recibir." << std::endl;
//		}
//		data[sizeRecv] = '\0';
//		std::cout << "Recibo: " << data << std::endl;
//		sock.disconnect();
//	}
//	system("pause");
//	return 0;
//}