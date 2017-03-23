#include "MessageManager.h"


// 0=null

// _auxText[0] -> Type
// _auxText[1] -> Subtype
// _auxText[2] -> Player
// _auxText[3..] -> Message

// 1_N_1_vacio // Mensaje_index_Jugador_vacio
// 2_1_0_vacio // Estado_Start_null_null********************
// 2_2_0_vacio // Estado_Finish_null_null
// 2_3_0_Fuera de Tiempo. // Mensaje_Log_Jugador_vacio
// 2_4_0_vacio // Mensaje_check_jugador_vacio
// 3_N_0_vacio // NuevaPpregunta_index_null_null
// 4_1_0_Manolo // Jugadores_IndiceJugador_0_Nombre
// 5_N_0_vacio // Puntuaciones_Puntuacion_Jugador_null

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
