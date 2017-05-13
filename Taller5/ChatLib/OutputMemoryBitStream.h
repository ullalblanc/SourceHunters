#pragma once
#include <memory>
#include <algorithm>
#include <string>
//#include "General.h"



class OutputMemoryBitStream
{
private:
	char* mBuffer;
	uint32_t mBitHead;
	uint32_t mBitCapacity;

	uint32_t max(uint32_t mBit, uint32_t nextBit) {
		if (mBit > nextBit) return mBit;
		else return nextBit;
	};

	void ReallocBuffer(uint32_t _inNewBitCapacity)
	{
		if (mBuffer == nullptr)
		{
			//Cuando es la primera vez que pedimos memoria
			//el n�mero de bytes a reservar, es _inNewBitCapacity/8 (para dividir shiftamos 3 posiciones)
			mBuffer = static_cast<char*>(std::malloc(_inNewBitCapacity >> 3));
			//Ponemos a cero las posiciones de memoria reservadas.
			memset(mBuffer, 0, _inNewBitCapacity >> 3);
		}
		else
		{
			//mBuffer ya tiene memoria pedida y necesitamos ampliarlo

			//Creamos un nuevo buffer temporal con la capacidad ampliada
			char* tempBuffer = static_cast<char*>(std::malloc(_inNewBitCapacity >> 3));
			//Lo ponemos todo a cero
			memset(tempBuffer, 0, _inNewBitCapacity >> 3);
			//Copiamos lo que hay en mBuffer al nuevo buffer
			memcpy(tempBuffer, mBuffer, mBitCapacity >> 3);
			//Liberamos el mBuffer
			std::free(mBuffer);
			//Ahora mBuffer pasa a ser el buffer temporal que acabo de crear
			mBuffer = tempBuffer;
		}

		//TODO::Controlar si no se ha podido pedir memoria
		mBitCapacity = _inNewBitCapacity;
	}

public:
	OutputMemoryBitStream() :
		mBitHead(0),
		mBuffer(nullptr)
	{
		ReallocBuffer(32);
	}

	OutputMemoryBitStream(const OutputMemoryBitStream &output)
	{
		mBuffer = output.mBuffer;
		mBitHead = output.mBitHead;
		mBitCapacity = output.mBitCapacity;
	}

	~OutputMemoryBitStream()
	{
	}

	void WriteBits(uint8_t _inData, size_t _inBitCount)
	{
		//Miramos donde quedar� en Head si le sumo los bits que se necesitan para guardar
		//la nueva informaci�n
		uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(_inBitCount);
		if (nextBitHead > mBitCapacity)
		{
			ReallocBuffer(max(mBitCapacity * 2, nextBitHead));
		}
		//Calculamos en qu� byte del buffer estamos dividiendo el head / 8
		uint32_t byteOffset = mBitHead >> 3;
		//Calculamos el bit offset con los 3 �ltimos bits del head.
		uint32_t bitOffset = mBitHead & 0x7;

		//Miramos qu� bits del byte actual hemos de mantener
		//~ -> NOT a nivel de bit
		uint8_t currentMask = ~(0xff << bitOffset);
		//Mantenemos la informaci�n que ya hab�a en este byte y a�adimos la nueva en el espacio que queda libre
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (_inData << bitOffset);

		//Calculamos cu�ntos bytes hab�an libres en el byte actual
		uint32_t bitsFreeThisByte = 8 - bitOffset;

		//Si no ha sido suficiente para copiar lo que necesit�bamos, hemos de movernos al siguiente byte
		if (bitsFreeThisByte < _inBitCount)
		{
			//Copiamos el resto de bytes que no han cabido antes
			mBuffer[byteOffset + 1] = _inData >> bitsFreeThisByte;
		}
		//Actualizamos el BitHead
		mBitHead = nextBitHead;
	}

	void WriteBitsOver8(void* _inData, size_t _inBitCount)
	{
		char* srcByte = static_cast<char*>(_inData);

		int numByte = 0;
		while (_inBitCount > 8)
		{
			WriteBits(srcByte[numByte], 8);
			_inBitCount -= 8;
			numByte++;
		}
		if (_inBitCount > 0)
		{
			WriteBits(srcByte[numByte], _inBitCount);
		}
	}

	char* GetBufferPtr() const
	{
		return mBuffer;
	}

	uint32_t GetBitLength() const
	{
		return mBitHead;
	}

	uint32_t GetByteLength() const
	{
		return (mBitHead + 7) >> 3;
	}

	void WriteBytes(void* _inData, size_t _inByteCount)
	{
		WriteBitsOver8(_inData, _inByteCount << 3);
	}

	void WriteString(const std::string inString)
	{
		uint32_t elementCount = static_cast< uint32_t >(inString.size());
		Write(elementCount);
		for (const char element : inString)
		{
			Write(element);
		}
	}

	template< typename T >
	void Write(std::vector<T>& _inData)
	{
		uint32_t length = _inData.size();
		Write(length);
		for (size_t i = 0; i < length; i++)
		{
			WriteBitsOver8(&(_inData[i]), sizeof(T) * 8);
		}
	}

	template< typename T >
	void Write(T _inData, uint32_t _inBitCount = sizeof(T) * 8)
	{

		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Este Write solo soporta tipos basicos.");
		WriteBitsOver8(&_inData, _inBitCount);
	}


};
