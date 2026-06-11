#pragma once
#include "Slownik.hpp"

class HashAVL : public Slownik
{
    struct Wezel
    {
        int klucz;
        int wartosc;
        int wysokosc;
        Wezel *lewy;
        Wezel *prawy;
        Wezel(int k, int w) : klucz(k), wartosc(w), wysokosc(1), lewy(nullptr), prawy(nullptr) {}
    };

    Wezel **kubelki;
    int pojemnosc;
    int rozmiar;

    static const int PROG_PROCENT = 75;

    int funkcjaMieszajaca(int klucz) const;
    void rehash();

    // Pomocnicze operacje na drzewie AVL
    int wysokosc(Wezel *w) const;
    int balans(Wezel *w) const;
    void aktualizujWysokosc(Wezel *w);
    Wezel *rotacjaPrawo(Wezel *y);
    Wezel *rotacjaLewo(Wezel *x);
    Wezel *wstawAVL(Wezel *w, int klucz, int wartosc, bool &dodano);
    Wezel *usunAVL(Wezel *w, int klucz, bool &usunieto);
    Wezel *minWezel(Wezel *w) const;
    Wezel *znajdzAVL(Wezel *w, int klucz) const;
    void usunDrzewo(Wezel *w);
    void przepiszDrzewo(Wezel *w);                // pomocnicze do rehash
    void wyswietlDrzewo(Wezel *w, int &wypisane); // pomocnicze do wyswietl

public:
    HashAVL();
    ~HashAVL();

    void wstaw(int klucz, int wartosc) override;
    bool usun(int klucz) override;
    bool znajdz(int klucz, int &wynik) override;
    int size() override;
    void wyswietl() override;
    void wypelnij(const int *klucze, const int *wartosci, int n) override;
};