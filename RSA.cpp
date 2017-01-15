#include <time.h>
#include "RSA.h"

using namespace std;

RSA::RSA(int p, int q)
{
    this->p = p;
    this->q = q;

    phi = (p - 1) * (q - 1);
    n = p * q;

    for(e = 3; gcd(e,phi) != 1; e += 2);
    d = inv_mod(e,phi);
}

int RSA::gcd(int a, int b)
{
  int t;

  while(b != 0)
  {
    t = b;
    b = a % b;
    a = t;
  };
  return a;
}

int RSA::inv_mod(int a, int n)
{
  int a0,n0,p0,p1,q,r,t;

  p0 = 0; p1 = 1; a0 = a; n0 = n;
  q  = n0 / a0;
  r  = n0 % a0;
  while(r > 0)
  {
    t = p0 - q * p1;
    if(t >= 0)
      t = t % n;
    else
      t = n - ((-t) % n);
    p0 = p1; p1 = t;
    n0 = a0; a0 = r;
    q  = n0 / a0;
    r  = n0 % a0;
  }
  return p1;
}


int RSA::powMod(int a, int exp, int n)
{
  unsigned long int pow, ret, q;

  pow = a; ret = 1;
  for(q = exp; q > 0; q /= 2)
  {
    if(q % 2) ret = (ret * pow) % n;
    pow = (pow * pow) % n;
  }
  return ret;
}

string RSA::encode(string data)
{
    string ret = "";
    for(int i = 0; i < (int)data.length() / 2; i++)
    {
        char c1 = data.c_str()[2*i];
        char c2 = data.c_str()[2*i+1];
        int l = (c1 << 8) | c2;
        int encoded = powMod(l, e, n);
        ret += Converter::fill(Converter::toString(encoded), '0', 5);
    }
    if(data.length() % 2 != 0)
    {
        int l = data.c_str()[data.length()-1];
        int encoded = powMod(l, e, n);
        ret += Converter::fill(Converter::toString(encoded), '0', 5);
    }
    return ret;
}

string RSA::decode(string data)
{
    if(data.length() % 5 != 0)
        return "";

    string ret = "";
    for(int i = 0; i < (int)data.length() / 5; i++)
    {
        string str = data.substr(i * 5, 5);
        int l = Converter::toInt(str);
        if(l < 0)
            return "";
        int decoded = powMod(l, d, n);

        if(decoded < 256)
        {
            char c = decoded;
            ret += c;
        }
        else
        {
            char c1 = decoded >> 8;
            char c2 = decoded & 255;
            ret += c1;
            ret += c2;
        }
    }
    return ret;
}
