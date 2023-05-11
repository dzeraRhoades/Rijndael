#pragma once
#include<vector>
#include<string>

class EncryptionBlockAlgorithm
{
public:
	EncryptionBlockAlgorithm() = default;
	virtual ~EncryptionBlockAlgorithm() = default;
	virtual void encrypt(uint8_t* text, const uint8_t* key) const = 0;
	virtual void decrypt(uint8_t* text, const uint8_t* key) const = 0;
	virtual uint64_t getBlockSize() const = 0;
	virtual uint64_t getKeySize() const = 0;
};

class BlockEncryptionException : public std::exception
{
public:
	BlockEncryptionException(const char* error);
	const char* what() const noexcept override;
private:
	std::string errorMsg;

};

