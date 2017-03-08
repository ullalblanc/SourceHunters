#include "MessageManager.h"


// 0=null

// _auxText[0] -> Type
// _auxText[1] -> Subtype
// _auxText[2] -> Player
// _auxText[3..] -> Message

// 1_1_1_r // Mensaje_Typing_Jugador_letra
// 1_2_1_pelota // Mensaje_Palabra_Jugador_palabra
// 1_3_1_Fuera de Tiempo. // Mensaje_Log_Jugador_Texto
// 2_1_0_vacio // Estado_Start_null_null
// 2_1_0_vacio // Estado_Finish_null_null
// 3_0_0_Palabra // NuevaPalabra_null_null_Palabra
// 4_1_0_Manolo // Jugadores_IndiceJugador_0_Nombre

//Funciones std::String
//http://www.cplusplus.com/reference/string/string/operator[]/
//http://www.cplusplus.com/reference/string/string/replace/
//http://www.cplusplus.com/reference/string/string/clear/
//http://www.cplusplus.com/reference/string/string/erase/



int MessageManager::GetType(std::string RawMessage)
{
	std::string _auxText = "";
	_auxText = RawMessage;
	int _auxInt = _auxText[0];
	_auxInt = _auxInt - 48;
	return _auxInt;
}

int MessageManager::GetSubType(std::string RawMessage)
{
	std::string _auxText = "";
	_auxText = RawMessage;
	int _auxInt = _auxText[1];
	_auxInt = _auxInt - 48;
	return _auxInt;
}

int MessageManager::GetPlayer(std::string RawMessage)
{

	std::string _auxText = "";
	_auxText = RawMessage;
	int _auxInt = _auxText[2];
	_auxInt = _auxInt - 48;
	return _auxInt;
}


std::string MessageManager::GetWord(std::string RawMessage)
{
	std::string _auxText = "";
	_auxText = RawMessage;
	_auxText = _auxText.erase(0, 3);
	return _auxText;
}

std::string MessageManager::CreateMessage(int type, int subtype, int indexPlayer, std::string Word)
{
	std::string message = "";
	message = std::to_string(type) + std::to_string(subtype) + std::to_string(indexPlayer) + Word;
	return message;
}

MessageManager::MessageManager()
{
}


MessageManager::~MessageManager()
{
}
