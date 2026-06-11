#pragma once
#include "Slownik.hpp"

class HashChaining : public Slownik
{
    struct Wezel
    {
        int klucz;
        int wartosc;
        Wezel *nast;
        Wezel(int k, int w) : klucz(k), wartosc(w), nast(nullptr) {}
    };

    Wezel **kubelki; // tablica wskaznikow na listy
    int pojemnosc;   // liczba kubelkow
    int rozmiar;     // liczba przechowywanych par

    static const int PROG_PROCENT = 75; // rehash gdy zapelnienie > 75%

    int funkcjaMieszajaca(int klucz) const; // zamienia klucz na indeks kubelka
    void rehash();                          // powieksza tablice i przepisuje elementy

public:
    HashChaining();
    ~HashChaining();

    void wstaw(int klucz, int wartosc) override;
    bool usun(int klucz) override;
    bool znajdz(int klucz, int &wynik) override;
    int size() override;
    void wyswietl() override;
    void wypelnij(const int *klucze, const int *wartosci, int n) override;
};