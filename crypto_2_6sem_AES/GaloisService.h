#pragma once
#include<cstdint>
#include<cmath>

/*
defines the galois field 2^8 methods
*/
class GaloisService
{
public:
    static uint64_t mult(uint8_t a, uint8_t b, uint32_t mod);
    static uint32_t sum(uint32_t a, uint32_t b);
    static uint8_t invert(uint8_t num, uint32_t mode);
    static uint32_t mod(uint32_t a, uint32_t b);
    static uint32_t modAndMultipple(uint32_t a, uint32_t b, uint32_t& multiple);
    static uint32_t Evklid(uint32_t a, uint32_t b);
    static uint32_t extendedEvklid(uint32_t a, uint32_t b, uint32_t& x, uint32_t& y);
    static bool isIrreducible(uint32_t pol);
    static bool xorAllBits(uint32_t byte);
    static uint8_t& cicleMove(uint8_t& row, uint8_t shift);
    static uint32_t& cicleMove(uint32_t& row, uint8_t shift);
    // cicle move row shift bytes left. row length is rowLength bytes
    static void cicleMoveLeft(uint8_t* row, uint8_t shift, uint8_t rowLength);
    static void cicleMoveRight(uint8_t* row, uint8_t shift, uint8_t rowLength);
private:
    static bool xorAllBits(uint32_t byte, int numberOfBits);
};


