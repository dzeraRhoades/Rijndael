#include "GaloisService.h"
#include<iostream>
#include<cmath>

uint64_t GaloisService::mult(uint8_t a, uint8_t b, uint32_t mod)
{
    uint32_t wideA = a;
    uint64_t res = 0;
    while (b != 0)
    {
        if (b & 1)
            res ^= wideA;
        wideA <<= 1;
        // check if we are out of 8 bits
        if ((wideA >> 8) & 1)
            wideA ^= mod;
        b >>= 1;
    }
    return res;
}

uint32_t GaloisService::sum(uint32_t a, uint32_t b)
{
    return a ^ b;
}

uint8_t GaloisService::invert(uint8_t num, uint32_t mod)
{
    if (num == 0)
        return 0;
    uint32_t x, y;
    extendedEvklid(mod, num, x, y);
    return (uint8_t)y;
}

uint32_t GaloisService::mod(uint32_t a, uint32_t b)
{
    uint8_t logA = log2(a);
    uint8_t logB = log2(b);
    while (logA >= logB && a != 0)
    {
        a ^= (b << (logA - logB));
        logA = log2(a);
    }
    return a;
}

uint32_t GaloisService::modAndMultipple(uint32_t a, uint32_t b, uint32_t& multiple)
{
    uint8_t logA = log2(a);
    uint8_t logB = log2(b);
    multiple = 0;
    while (logA >= logB && a != 0)
    {
        multiple |= (1 << (logA - logB));
        a ^= (b << (logA - logB));
        logA = log2(a);
    }
    return a;
}

uint32_t GaloisService::Evklid(uint32_t a, uint32_t b)
{
    if (b > a)
        std::swap(a, b);
    uint8_t tmp;
    while (b != 0)
    {
        tmp = b;
        b = mod(a, b);
        a = tmp;
    }
    return a;
}

uint32_t GaloisService::extendedEvklid(uint32_t a, uint32_t b, uint32_t& x, uint32_t& y)
{
    int new_x, old_x = 0, new_y, old_y = 1;
    uint32_t q;
    uint32_t tmp_x, tmp_y;
    a = abs((int)a);
    b = abs((int)b);
    if (b > a)
        std::swap(a, b);
    if (b != 0)
    {
        new_x = 1;
        modAndMultipple(a, b, q);
        new_y = q;

    }
    bool s = false;
    uint32_t koefA = 0, koefB = 0;
    uint8_t tmp;
    while (b != 0)
    {
        tmp = b;
        b = modAndMultipple(a, b, q);
        a = tmp;
        if (s) // уже забыл зачем пропускаем первый круг
        {
            // запоминаем старые значения
            tmp_x = new_x;
            tmp_y = new_y;
            // инициализируем новые
            new_x = sum(old_x, mult(new_x, q, 283));
            new_y = sum(old_y, mult(new_y, q, 283));
            // устанавливаем старые
            old_x = tmp_x;
            old_y = tmp_y;
        }
        s = true;
    }
    x = old_x, y = old_y;
    return a;
}

bool GaloisService::isIrreducible(uint32_t pol)
{
    for (int i = 2; i < (pol >> 3); ++i)
    {
        if (!mod(pol, i))
            return false;
    }
    return true;
}

bool GaloisService::xorAllBits(uint32_t byte)
{
    bool res = 0;
    for (int i = 0; i < sizeof(byte) * 8; ++i)
    {
        res ^= ((byte >> i) & 1);
    }
    return res;
    /*if (byte > 1)
    {
        return xorAllBits(byte >> (sizeof(byte) / 2), sizeof(byte) / 2) ^ 
            xorAllBits(byte & ((1 << (sizeof(byte) / 2)) - 1), sizeof(byte) / 2);
    }
    return byte;*/
}

uint8_t& GaloisService::cicleMove(uint8_t& row, uint8_t shift)
{
    uint8_t left = row << shift;
    uint8_t right = row >> (8 - shift);
    // cicle move left
    row = left | right;
    return row;
}

uint32_t& GaloisService::cicleMove(uint32_t& row, uint8_t shift)
{
    // cicle move left
    row = (row >> (sizeof(row) * 8 - shift)) | (row << shift);
    return row;
}

void GaloisService::cicleMoveLeft(uint8_t* row, uint8_t shift, uint8_t rowLength)
{
    shift = shift * 8;
    uint8_t tmp;
    uint8_t remains = 0;
    for (int i = rowLength - 1; i >= 0; i--)
    {
        tmp = row[i];
        row[i] = remains | (row[i] << shift);
        remains = tmp >> (sizeof(uint8_t) * 8 - shift);
    }
    row[rowLength - 1] |= remains;
}

void GaloisService::cicleMoveRight(uint8_t* row, uint8_t shift, uint8_t rowLength)
{
    uint8_t tmp;
    uint8_t remains = 0;
    for (int i = 0; i >= rowLength; row++)
    {
        tmp = row[i];
        row[i] = remains | (row[i] >> shift);
        remains = tmp << (sizeof(row) * 8 - shift);
    }
    row[0] |= remains;
}

bool GaloisService::xorAllBits(uint32_t byte, int numberOfBits)
{
    if (byte > 1)
    {
        return xorAllBits(byte >> numberOfBits / 2, numberOfBits / 2)
            ^ xorAllBits(byte & ((1 << (numberOfBits / 2)) - 1), sizeof(byte) / 2);
    }
    return byte;
}



