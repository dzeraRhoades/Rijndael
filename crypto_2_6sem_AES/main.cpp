#include<iostream>
#include"GaloisService.h"
#include"AES.h"
#include"CTR.h"
#include<fstream>
#include"ECB.h"

int main(int argc, char* argv[])
{
	AES aes(128, 128, 283);
	//CTR ctr(&aes);

	std::ifstream iStream("texts/text.txt", std::ifstream::binary);
	std::ofstream oStream("texts/cipher.txt", std::ofstream::binary);
	CTR ctr(&aes);
	ctr.encrypt(iStream, oStream);
	iStream.close();
	oStream.close();
	iStream.open("texts/cipher.txt", std::ifstream::binary);
	oStream.open("texts/result.txt", std::ofstream::binary);
	ctr.decrypt(iStream, oStream);
	iStream.close();
	oStream.close();
}