#pragma once

// klasa bazowa slownika opartego na tablicy mieszajacej.
// klucze i wartosci sa liczbami calkowitymi.
class Slownik
{
public:
    virtual void wstaw(int klucz, int wartosc) = 0;
    virtual bool usun(int klucz) = 0;
    virtual bool znajdz(int klucz, int &wynik) = 0;
    virtual int size() = 0;
    virtual void wyswietl() = 0;
    virtual void wypelnij(const int *klucze, const int *wartosci, int n) = 0;
    virtual ~Slownik() {}
};