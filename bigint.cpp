#include "bigint.h"
#include <cmath>
#include <cctype>
#include <iostream>
#include <string>
#include <cstdint>
using namespace std;

/// Construtor default.
/// Inicializa com um inteiro de 1 digito, valor 0 (zero).
/// NAO PODE SER MODIFICADO
BigInt::BigInt()
  : neg(false)
  , nDig(1)
  , d(new int8_t[1]{0})
{}

/// Destrutor
BigInt::~BigInt()
{
    delete[] d;
}
/// Construtor especifico PRIVADO que recebe o sinal e a quantidade de digitos
BigInt::BigInt(bool IsNeg, int Size) :
    neg(IsNeg),
    nDig(Size >= 1 ? Size : 1),
    d(new int8_t[nDig]{0})
{
}
/// Construtor por copia.
/// Delega ao construtor especifico privado.
/// NAO PODE SER MODIFICADO.
BigInt::BigInt(const BigInt& B)
  : BigInt(B.isNeg(), B.size())
{
  // Copia os digitos
  for (int i=0; i<size(); ++i) d[i] = B.d[i];
}

/// Atribuicao por copia
BigInt& BigInt::operator=(const BigInt& B)
{
    if (this != &B)
    {
        delete[] d;
        neg = B.neg;
        nDig = B.nDig;
        d = new int8_t[nDig];
        for (int i=0; i<nDig; ++i)
        d[i] = B.d[i];
    }

    return *this;
}
/// Construtor por movimento
BigInt::BigInt(BigInt&& B) noexcept
  : neg(B.neg)
  , nDig(B.nDig)
  , d(B.d)
{
    B.neg = false;
    B.nDig = 0;
    B.d = nullptr;
}
/// Atribuicao por movimento
BigInt& BigInt::operator=(BigInt&& B) noexcept
{
    if (this != &B)
    {
        delete[] d;
        neg = B.neg;
        nDig = B.nDig;
        d = B.d;
        B.neg = false;
        B.nDig = 0;
        B.d = nullptr;
    }

    return *this;
}

/// Construtor especifico a partir de inteiro longo.
/// Tambem conversor de long long int para BigInt.
/// NAO PODE SER MODIFICADO NAS PARTES JAH IMPLEMENTADAS.
/// PODE (E PRECISA) RECEBER ACRESCIMOS, APENAS
/// NAS PARTES INDICADAS POR /* ACRESCENTAR */
BigInt::BigInt(long long int N)
  : BigInt(N < 0,
           N == 0 ? 1 : 1 + int(log10(fabs(N))))
{
  // Calcula os digitos, usando divisao inteira por 10
  for (int i=0; i<size(); ++i)
  {
    d[i] = abs(N%10);
    N /= 10;
  }
}
/// Conversor de BigInt para long long int
long long int BigInt::toInt() const
{
    long long int valor = 0;

    for (int i=size()-1; i>=0; --i)
    {
        valor = 10*valor + d[i];

        if (valor < 0)
        {
            cerr << "Erro: bigint muito grande convertendo para long long int"<<endl;
            return 0;
        }
    }

    if (isNeg())
        valor = -valor;

    return valor;
}



/// ******************
/// * FIM DA PARTE 1 *
/// ******************

/// Funcao privada que corrige o numero, caso haja inconsistencias
void BigInt::correct()
{
    int novo = size();

    while (novo > 1 && d[novo-1] == 0)
    {
        novo--;
    }

    if (novo != size())
    {
        BigInt B(neg, novo);

        for (int i=0; i<novo; ++i)
            B.d[i] = d[i];

        *this = B;
    }

    if (isZero())
    {
        neg = false;
    }
}
/// Construtor especifico a partir de string.
/// Nao eh conversor de string para BigInt.
/// Delega ao construtor default.
/// NAO PODE SER MODIFICADO NAS PARTES JAH IMPLEMENTADAS.
/// PODE (E PRECISA) RECEBER ACRESCIMOS, APENAS
/// NAS PARTES INDICADAS POR /* ACRESCENTAR */
BigInt::BigInt(const string& S)
  : BigInt() // Valor inicial zero
{
  // Se string vazia, emite erro e permanece com valor inicial zero
  if (S.empty())
  {
    cerr << "empty string cannot create a BigInt\n";
    return;
  }

  // Posicao onde comecam os digitos, inicialmente zero
  size_t ini=0;
  // Leva em conta o sinal
  bool IsNeg = false;

  if (S[0]=='+' || S[0]=='-')
  {
    // Se nao tem nenhum digito alem do sinal, emite erro e permanece com valor inicial
    if (S.size()==1)
    {
      cerr << "sign-only string cannot create a BigInt\n";
      return;
    }
    IsNeg = (S[0]=='-');
    ++ini;
  }

  // Faz ter sinal (IsNeg) e numero de digitos (tamanho da string - ini) corretos
    *this = BigInt(IsNeg, S.size()-ini);

  // Calculo dos digitos do BigInt
  for (int i=0; i<size(); ++i)
  {
    const char& c = S[S.size()-1-i]; // Referencia (apelido) para facilitar a notacao
    if (!isdigit(c))
    {
      *this = BigInt(); // = 0
      cerr << "string with invalid character cannot create a BigInt\n";
      return;
    }
    d[i] = static_cast<int8_t>(c-'0');
  }
  // Corrige eventuais numeros fora da especificacao
  correct();
}

/// ******************
/// * FIM DA PARTE 2 *
/// ******************

/// Insercao (impressao)
ostream& operator<<(ostream& O, const BigInt& B)
{
    if (B.isNeg())
        O << '-';

    for (int i=B.size()-1; i>=0; --i)
        O << B[i];

    return O;
}

/// Extracao (digitacao).
/// NAO PODE SER MODIFICADO NAS PARTES JAH IMPLEMENTADAS.
/// PODE (E PRECISA) RECEBER ACRESCIMOS NAS PARTES INDICADAS POR /* ACRESCENTAR */
std::istream& operator>>(istream& I, BigInt& B)
{
  // Valor inicial zero
  B = BigInt(); // = 0

  // Testa a stream de entrada e descarta eventuais separadores iniciais.
  // Em caso de erro, encerra a digitacao.
  istream::sentry s(I);
  if (!s) return I;

  // Inspeciona o primeiro caractere que serah lido
  int c = I.peek();

  // Testa se o primeiro caractere eh um sinal.
  // Se for, consome (elimina do buffer), processa e inspeciona o proximo caractere.
  if (c=='+' || c=='-')
  {
    // Consome da stream
    c = I.get(); // Valor de "c" permanece o mesmo...
    // Atribui o sinal ESSA PARTE DA ERRO PERGUNTAR AO PROFESSOR !!!!!!!!!!!(corigi vi no novo pdf)
    B.neg = (c=='-');
    // Obtem o proximo caractere
    c = I.peek();
  }

  // Numero de digitos que foram digitados
  int numDigitos = 0;

  // Testa se eh um caractere valido: digitos 0 a 9
  while (isdigit(c))
  {
    // Consome da stream
    c = I.get(); // Valor de "c" permanece o mesmo...
    ++numDigitos;

   if (numDigitos>1)
{
    BigInt temp(B.neg, B.size()+1);

    for (int i=0; i<B.size(); ++i)
    {
        temp.d[i+1] = B.d[i];
    }

    B = temp;
}
    // Acrescenta o novo digito como sendo o primeiro (o menos significativo)
    B.d[0] = static_cast<int8_t>(c-'0');

    // Inspeciona o proximo caractere que vai ser lido
    c = I.peek();
  }

  // Assinala erro na stream se nenhum digito foi lido
  if (numDigitos==0) I.setstate(ios::failbit);

  // Corrige eventuais erros na digitacao
  B.correct();

  // Encerra a digitacao
  return I;
}

/// ******************
/// * FIM DA PARTE 3 *
/// ******************

/// Teste de igualdade
bool operator==(const BigInt& A, const BigInt& B)
{
    if (A.neg != B.neg)
        return false;

    if (A.nDig != B.nDig)
        return false;

    for (int i=0; i<A.nDig; ++i)
    {
        if (A.d[i] != B.d[i])
            return false;
    }

    return true;
}

/// Menor que

bool operator<(const BigInt& A, const BigInt& B)
{
    if (A.isNeg() != B.isNeg())
        return A.isNeg();

    if (A.size() != B.size())
    {
        if (!A.isNeg())
            return A.size() < B.size();
        else
            return A.size() > B.size();
    }

    for (int i=A.size()-1; i>=0; --i)
    {
        if (A.d[i] != B.d[i])
        {
            if (!A.isNeg())
                return A.d[i] < B.d[i];
            else
                return A.d[i] > B.d[i];
        }
    }

    return false;

}

// não entendi se tem que colocar aq,vou colocar no header

/// ******************
/// * FIM DA PARTE 4 *
/// ******************

/// Funcao privada que incrementa os digitos (o modulo) do numero
void BigInt::increment()
{
    int k = 0;

    while (k < size())
    {
        if (d[k] < 9)
        {
            d[k]++;
            return;
        }
        d[k] = 0;
        k++;
    }
    *this = BigInt(neg, size()+1);
    d[size()-1] = 1;
}
/// Funcao privada que decrementa os digitos (o modulo) do numero
void BigInt::decrement()
{
    if (isZero())
    {
        *this = BigInt(-1);
        return;
    }
    int k = 0;
    while (k < size())
    {
        if (d[k] > 0)
        {
            d[k]--;
            break;
        }

        d[k] = 9;
        k++;
    }
    correct();
}
/// Operador de incremento pre-fixado
/// NAO PODE SER MODIFICADO
BigInt& BigInt::operator++()
{
  if (!isNeg()) increment();
  else decrement();
  return *this;
}

/// Operador de decremento pre-fixado
/// NAO PODE SER MODIFICADO
BigInt& BigInt::operator--()
{
  if (isNeg()) increment();
  else decrement();
  return *this;
}

/// Operador de incremento pos-fixado
BigInt BigInt::operator++(int)
{
    BigInt temp(*this);
    ++(*this);
    return temp;
}

/// Operador de decremento pos-fixado
BigInt BigInt::operator--(int)
{
    BigInt temp(*this);
    --(*this);
    return temp;
}
/// ******************
/// * FIM DA PARTE 5 *
/// ******************

/// Modulo (abs)
BigInt abs(const BigInt& A)
{
    BigInt C(A);
    C.neg = false;
    return C;
}

/// Negativo (unario)
BigInt operator-(const BigInt& A)
{
    BigInt C(A);
    if (!C.isZero())
        C.neg = !C.neg;

    return C;
}
///soma unaria
const BigInt& operator+(const BigInt& A)
{
    return A;
}

/// Soma
BigInt operator+(const BigInt& A, const BigInt& B)
{
    if (A.isZero())
        return B;

    if (B.isZero())
        return A;

    if (A.isNeg() != B.isNeg())
        return A - (-B);

    int maior = A.size();
    if (B.size() > maior)
        maior = B.size();

    BigInt C(A.isNeg(), maior+1);

    int pg = 0;

    for (int i=0; i<C.size(); ++i)
    {
        C.d[i] = A[i] + B[i] + pg;

        if (C.d[i] > 9){
            C.d[i] = C.d[i] - 10;
            pg = 1;
        }
        else{
            pg = 0;
        }
    }
    C.correct();

    return C;
}

/// Subtracao
BigInt operator-(const BigInt& A, const BigInt& B)
{
    if (A.isZero())
        return -B;

    if (B.isZero())
        return A;

    if (A.isNeg() != B.isNeg())
        return A + (-B);

    if (abs(A) < abs(B))
        return -(B - A);

    BigInt C(A.isNeg(), A.size());

    int emp = 0;

    for (int i=0; i<C.size(); ++i){
        C.d[i] = A[i] - B[i] - emp;

        if (C.d[i] < 0){
            C.d[i] = C.d[i] + 10;
            emp = 1;
        }

        else{
            emp = 0;
        }
    }

    C.correct();

    return C;
}

/// ******************
/// * FIM DA PARTE 6 *
/// ******************

/// Multiplicacao
BigInt operator*(const BigInt& A, const BigInt& B)
{
    if (A.isZero() || B.isZero())
        return BigInt();

    bool sinal;

    if (A.isNeg() != B.isNeg())
        sinal = true;
    else
        sinal = false;

    BigInt C(sinal, A.size() + B.size());

    for (int i=0; i<A.size(); ++i)
    {
        if (A.d[i] != 0)
        {
            for (int j=0; j<B.size(); ++j)
            {
                if (B.d[j] != 0)
                {
                    int k = i + j;

                    C.d[k] = C.d[k] + A.d[i] * B.d[j];

                    while (C.d[k] > 9)
                    {
                        int pg = C.d[k] / 10;
                        C.d[k] = C.d[k] % 10;

                        k++;
                        C.d[k] = C.d[k] + pg;
                    }
                }
            }
        }
    }

    C.correct();

    return C;
}
/// Fatorial
BigInt operator!(const BigInt& A)
{
    if (A.isNeg())
    {
        cerr << "Erro: fatorial nao pode ser negativo" << endl;
        return BigInt();
    }

    BigInt C(1);

    for (BigInt N(2); N <= A; ++N)
    {
        C = C * N;
    }

    return C;
}

/// ******************
/// * FIM DA PARTE 7 *
/// ******************

/// Deslocamento aa esquerda
BigInt operator<<(const BigInt& A, int N)
{
    if (N <= 0 || A.isZero())
        return A;

    BigInt C(A.isNeg(), A.size() + N);

    for (int i=N; i<C.size(); ++i)
    {
        C.d[i] = A.d[i-N];
    }

    return C;
}
/// Deslocamento aa direita
BigInt operator>>(const BigInt& A, int N)
{
    if (N <= 0 || A.isZero())
        return A;

    if (N >= A.size())
        return BigInt();

    BigInt C(A.isNeg(), A.size() - N);

    for (int i=0; i<C.size(); ++i)
    {
        C.d[i] = A.d[i+N];
    }

    return C;
}

/// Divisao de *this por D.
/// Armazena o resultado (quociente) em Q e o resto da divisao em R.
void BigInt::division(const BigInt& D, BigInt& Q, BigInt& R) const
{
    if (isZero() || D.isZero()){
        if (D.isZero())
            cerr << "dividindo zero" << endl;

        Q = BigInt();
        R = BigInt();
        return;
    }

    BigInt absD = abs(D);

    if (abs(*this) < absD){
        Q = BigInt();
        R = *this;
        return;
    }

    bool sinal;

    if (isNeg() != D.isNeg())
        sinal = true;
    else{
        sinal = false;
    }
    int tamanho = size() - D.size() + 1;

    Q = BigInt(sinal, tamanho);

    R = abs((*this) >> (size() - D.size()));

    for (int i=Q.size()-1; i>=0; --i){
        int div = 0;

        while (R >= absD){
            R = R - absD;
            div++;
        }

        Q.d[i] = div;

        if (i > 0){
            R = R << 1;
            R.d[0] = d[i-1];
        }
    }
    Q.correct();

    if (!R.isZero())
        R.neg = isNeg();
}
/// Quociente da divisao inteira
BigInt operator/(const BigInt& A, const BigInt& B)
{
    BigInt Q;
    BigInt R;
    A.division(B, Q, R);

    return Q;
}

/// Resto da divisao inteira
BigInt operator%(const BigInt& A, const BigInt& B)
{
    BigInt Q;
    BigInt R;
    A.division(B, Q, R);
    return R;
}

/// ******************
/// * FIM DA PARTE 8 *
/// ******************
