#include "AES.h"
#include"GaloisService.h"
#include<iostream>

#define BYTE 8 // bits per byte
#define WORD 4 // bytes per word

AES::AES(int keySize, int stateSize, uint64_t polynom)
{
	if (keySize != 128 && keySize != 192 && keySize != 256)
		throw BlockEncryptionException("invalid key size value!!! key size can be: 128, 192, 256");
	if (stateSize != 128 && stateSize != 192 && stateSize != 256)
		throw BlockEncryptionException("invalid state size value!!! state size can be: 128, 192, 256");
	if (!GaloisService::isIrreducible(polynom))
		throw BlockEncryptionException("the polynom is not irreducible");

	this->keySize = keySize;
	this->stateSize = stateSize;
	this->irrPol = polynom;

	init();
	
}

void AES::encrypt(uint8_t* text, const uint8_t* key) const
{
	std::vector<uint32_t> expandedKey = generateRoundKeys(key);
	const uint8_t* roundKey;

	// все раунды кроме последнего:
	for (int i = 0; i < numberOfRounds - 1; ++i)
	{
		subBytes(text, false);
		//log(text, "subBytes");
		shiftRows(text, false);
		//log(text, "shiftRows");
		mixColumns(text, false);
		//log(text, "mixColumns");
		addRoundKey(text, expandedKey, i);
		//log(text, "ADDROUNDKEY");
	}
	// последний раунд без MixColumns
	subBytes(text, false);
	//log(text, "LAST subBytes");
	shiftRows(text, false);
	//log(text, "LAST shiftRows");
	addRoundKey(text, expandedKey, numberOfRounds - 1);
	//log(text, "LAST ADDROUNDKEY");
}

void AES::decrypt(uint8_t* text, const uint8_t* key) const
{
	std::vector<uint32_t> expandedKey = generateRoundKeys(key);
	const uint8_t* roundKey;

	// последний раунд без MixColumns
	//log(text, "LAST ADDROUNDKEY");
	addRoundKey(text, expandedKey, numberOfRounds - 1);
	//log(text, "LAST shiftRows");
	shiftRows(text, true);
	//log(text, "LAST subBytes");
	subBytes(text, true);

	// все раунды кроме последнего:
	for (int i = numberOfRounds - 2; i >= 0; --i)
	{
		//log(text, "ADDROUNDKEY");
		addRoundKey(text, expandedKey, i);
		//log(text, "shiftRows");
		mixColumns(text, true);
		//log(text, "mixColumns");
		shiftRows(text, true);
		//log(text, "subBytes");
		subBytes(text, true);
	}
}

uint64_t AES::getBlockSize() const
{
	return stateSize;
}

uint64_t AES::getKeySize() const
{
	return keySize;
}

uint8_t AES::subByte(uint8_t byte, bool inv) const
{
	if (sbox == nullptr || rsbox == nullptr)
		throw BlockEncryptionException("SubBytes error: sbox is null");
	if (!inv)
		return sbox[byte];
	else
		return rsbox[byte];
}

void AES::subBytes(uint8_t* bytes, bool inv) const
{
	for (int i = 0; i < stateSize / 8; ++i)
	{
		bytes[i] = subByte(bytes[i], inv);
	}
}

void AES::shiftRows(uint8_t* state, bool inv) const
{
	int numberOfRows = 4;
	int columnsNum = stateSize / numberOfRows / 8;
	for (int i = 1; i < numberOfRows; ++i)
	{
		shiftRow(state + i * columnsNum, columnsNum, i, inv);
	}
}

void AES::shiftRow(uint8_t* state, int columnsNum, int shift, bool inv) const
{
	if (inv)
	{
		reverse(state, shift);
		reverse(state + shift, columnsNum - shift);
		reverse(state, columnsNum);
	}
	else
	{
		reverse(state, columnsNum - shift);
		reverse(state + columnsNum - shift, shift);
		reverse(state, columnsNum);
	}
}

//uint32_t& AES::cicleMove(uint32_t& row, int shift)
//{
//	// cicle move right
//	row = (row << (32 - shift)) | (row >> shift);
//	return row;
//}

void AES::mixColumns(uint8_t* state, bool inv) const
{
	// every polynom is vector of bytes
	// we treat state like 4x4 array, with elements = bytes
	// vec[0] - junior coefficent
	int rowNum = 4;
	int colNum = stateSize / rowNum / BYTE;
	std::vector<uint8_t> polynom(rowNum, 0);
	for (int i = 0; i < colNum; ++i)
	{
		for (int j = 0; j < rowNum; ++j)
		{
			polynom[j] = state[i + j * colNum];
		}
		mixColumnsMult(polynom, inv);
		for (int j = 0; j < rowNum; ++j)
		{
			state[i + j * colNum] = polynom[j];
		}
	}

}

void AES::addRoundKey(uint8_t* state, const std::vector<uint32_t>& key, int index) const
{
	//// высчитываем по номеру текущего раунда слова текущего раундового ключа
	//uint32_t* state_32 = (uint32_t*)state;
	//const uint32_t* key_32 = key.data();
	//int wordsPerRoundKey = stateSize / BYTE / WORD;
	//key_32 += keySize / BYTE / WORD; // доходим до первого раундового ключа
	//key_32 += wordsPerRoundKey * index;
	//std::cout << "round key: ";

	//for (int i = 0; i < wordsPerRoundKey; ++i)
	//{
	//	std::cout << key_32[i] << " ";
	//	state_32[i] ^= key_32[i];
	//}
	//std::cout << std::endl;
	// высчитываем по номеру текущего раунда слова текущего раундового ключа
	uint8_t* state_32 = state;
	const uint8_t* key_32 = (uint8_t*)key.data();
	int wordsPerRoundKey = stateSize / BYTE;
	key_32 += keySize / BYTE; // доходим до первого раундового ключа
	key_32 += wordsPerRoundKey * index;

	for (int i = 0; i < wordsPerRoundKey; ++i)
	{
		state_32[i] ^= key_32[i];
	}
}

void AES::mixColumnsMult(std::vector<uint8_t>& polynom, bool inv) const
{
	// standart polynom
	const std::vector<uint8_t> c_forward = { 2, 1, 1, 3 };
	const std::vector<uint8_t> c_inv = { 0xe, 0x9, 0xd, 0xb };
	const std::vector<uint8_t> c = inv ? c_inv : c_forward;
	std::vector<uint8_t> result(4, 0);
	int l;
	for (int j = 0; j <= 6; ++j)
	{
		for (int k = 0; k <= j; ++k)
		{
			l = j - k;
			if (l < 4 && k < 4)
				result[j % 4] = GaloisService::sum(GaloisService::mult(polynom[l], c[k], irrPol), result[j % 4]);
		}
	}
	polynom = std::move(result);
}

void AES::generateSBox()
{
	uint8_t mask = 241; // 10001111
	uint8_t c = 99;
	for (int i = 0; i < 256; ++i)
	{
		uint8_t tmp = 0;
		mask = 241;
		sbox[i] = GaloisService::invert(i, irrPol);
		for (int j = 0; j < 8; ++j)
		{
			tmp |= ((uint8_t)(GaloisService::xorAllBits(sbox[i] & mask)) << j);
			GaloisService::cicleMove(mask, 1);
		}
		tmp ^= c;
		sbox[i] = tmp;
	}
}

void AES::generateInvSBox()
{
	int sboxSize = 256;
	if (sbox == nullptr)
		throw std::exception("sbox is null");
	for (int i = 0; i < sboxSize; ++i)
	{
		rsbox[sbox[i]] = i;
	}
}

void AES::determineNumberOfRounds()
{
	switch ((keySize / 8 / 4) * 10 + (stateSize / 8 / 4))
	{
	case 44:
		numberOfRounds = 10;
		break;
	case 46:
		numberOfRounds = 12;
		break;
	case 48:
		numberOfRounds = 14;
		break;
	case 64:
		numberOfRounds = 12;
		break;
	case 66:
		numberOfRounds = 12;
		break;
	case 68:
		numberOfRounds = 14;
		break;
	case 84:
		numberOfRounds = 14;
		break;
	case 86:
		numberOfRounds = 14;
		break;
	case 88:
		numberOfRounds = 14;
	default:
		break;
	}
}

void AES::reverse(uint8_t* row, int rowSize) const
{
	// разворачиваем левую часть
	uint8_t tmp;
	for (int i = 0; i < rowSize / 2; ++i)
	{
		std::swap(row[i], row[rowSize - 1 - i]);
	}
}

void AES::init()
{
	determineNumberOfRounds();
	generateSBox();
	generateInvSBox();
}

void AES::log(const uint8_t* state, const std::string& text) const
{
	int columnsNum = stateSize / 4 / 8;
	std::cout << "---------: " << text << "----------" << std::endl;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < columnsNum; ++j)
		{
			std::cout << (int)state[i * columnsNum + j] << " ";
		}
		std::cout << std::endl;
	}
	/*int byteNum = stateSize / BYTE;
	for (int i = 0; i < byteNum; ++i)
	{
		std::cout << (int)state[i] << " ";
	}*/
	std::cout << "------------------------------------------------" << std::endl;
}

uint32_t AES::rotByte(uint32_t word) const
{
	// циклически сдвигаем вправо на байт
	return ((word << 24) | (word >> 8));
}

std::vector<uint32_t> AES::generateRoundKeys(const uint8_t* key) const
{
	/*
	* первые слова должны быть заполнены ключем, поэтому нужно посчитать, сколько занимает ключ слов
	* проеобразуем массив байт в массив слов (4 байта) и добавляем в вектор
	*/
	// word = 4 bytes (uint32)
	int wordsPerRound = stateSize / 32;
	int wordsPerKey = keySize / 32;
	int numberOfWords = (numberOfRounds) * wordsPerRound + wordsPerKey;
	const uint32_t* key_32 = (const uint32_t*)key;
	std::vector<uint32_t> res(numberOfWords);

	// добавляем ключ в первые слова
	for (int i = 0; i < wordsPerKey; ++i)
	{
		res.push_back(key_32[i]);
	}

	// заполняем оставшиеся слова для каждого раунда
	for (int i = wordsPerKey; i < numberOfWords; ++i)
	{
		if (!(i % 4))
		{
			res[i] = subByte(rotByte(res[i - 1]), false) ^ Rcon[i / 4];
		}
		else
		{
			res[i] = res[i - 4] ^ res[i - 1];
		}
	}
	return res;

}
