#ifndef RSA_INCLUDED
#define RSA_INCLUDED

#include "Converter.h"

class RSA
{
    int p,q,phi,n,e,d;
public:
    RSA(int p, int q);
    int gcd(int a, int b);
    int inv_mod(int a, int n);
    int powMod(int a, int exp, int n);
    std::string encode(std::string data);
    std::string decode(std::string data);
    std::string encode(std::string data, int e, int n);
    std::string decode(std::string data, int e, int n);
    int getE();
    int getN();
};

#endif // RSA_INCLUDED
