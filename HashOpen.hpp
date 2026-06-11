#pragma once
#include "Slownik.hpp"

class HashOpen : public Slownik
{
    enum Stan // stan komorki: pusta, zajeta, usunieta
    {
        PUSTA,
        ZAJETA,
        USUNIETA
    };

    struct Komorka // pojedyncza komorka tablicy
    {
        int klucz;
        int wartosc;
        Stan stan;
        Komorka() : klucz(0), wartosc(0), stan(PUSTA) {}
    };

    Komorka *tablica;
    int pojemnosc;
    int rozmiar; // liczba komorek zjetych

    static const int PROG_PROCENT = 70; // rehash gdy zapelnienie > 70%

    int funkcjaMieszajaca(int klucz) const;
    void rehash();

public:
    HashOpen();
    ~HashOpen();

    void wstaw(int klucz, int wartosc) override;
    bool usun(int klucz) override;
    bool znajdz(int klucz, int &wynik) override;
    int size() override;
    void wyswietl() override;
    void wypelnij(const int *klucze, const int *wartosci, int n) override;
};