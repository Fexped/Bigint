#ifndef BIGINT_H
#define BIGINT_H

#include <cstdint>
#include <string>
#include <iostream>

class BigInt
{
private:
    bool neg;
    int nDig;
    int8_t* d;

    BigInt(bool IsNeg, int Size);
    void correct();
    void increment();
    void decrement();

public:
    BigInt();
    ~BigInt();

    BigInt(const BigInt& B);
    BigInt(BigInt&& B) noexcept;
    BigInt& operator=(const BigInt& B);
    BigInt& operator=(BigInt&& B) noexcept;

    BigInt(long long int N);
    explicit BigInt(const std::string& S);
    bool isNeg() const { return neg; }
    int size() const { return nDig; }
    bool isZero() const { return nDig == 1 && d[0] == 0; }
    int operator[](int i) const
    {if (i < 0 || i >= size())
            return 0;
    else{
        return d[i];
    }
    }

    long long int toInt() const;
    friend std::ostream& operator<<(std::ostream& O, const BigInt& B);
    friend std::istream& operator>>(std::istream& I, BigInt& B);
    friend bool operator==(const BigInt& A, const BigInt& B);
    friend bool operator<(const BigInt& A, const BigInt& B);
    friend bool operator!=(const BigInt& A, const BigInt& B) { return !(A == B); }
    friend bool operator>(const BigInt& A, const BigInt& B) { return B < A; }
    friend bool operator<=(const BigInt& A, const BigInt& B) { return !(B < A); }
    friend bool operator>=(const BigInt& A, const BigInt& B) { return !(A < B); }
    BigInt& operator++();
    BigInt& operator--();
    BigInt operator++(int);
    BigInt operator--(int);
    friend BigInt abs(const BigInt& A);
    friend BigInt operator-(const BigInt& A);
    friend BigInt operator+(const BigInt& A, const BigInt& B);
    friend BigInt operator-(const BigInt& A, const BigInt& B);
    friend BigInt operator*(const BigInt& A, const BigInt& B);
    friend BigInt operator!(const BigInt& A);
    friend const BigInt& operator+(const BigInt& A);
    //so coloquei agora pq eu nao vi no pdf pedindo o +unnario
    friend BigInt operator<<(const BigInt& A, int N);
    friend BigInt operator>>(const BigInt& A, int N);

    void division(const BigInt& D, BigInt& Q, BigInt& R) const;

    friend BigInt operator/(const BigInt& A, const BigInt& B);
    friend BigInt operator%(const BigInt& A, const BigInt& B);
};

#endif
