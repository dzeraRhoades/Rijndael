#include "EncryptionAlgorithm.h"

BlockEncryptionException::BlockEncryptionException(const char* error)
{
	errorMsg = error;
}

const char* BlockEncryptionException::what() const noexcept
{
	return errorMsg.c_str();
}
