#include "HashChaining.hpp"
#include <iostream>
using namespace std;

HashChaining::HashChaining() : pojemnosc(16), rozmiar(0)
{
    kubelki = new Wezel *[pojemnosc];
    for (int i = 0; i < pojemnosc; i++)
        kubelki[i] = nullptr;
}

HashChaining::~HashChaining()
{
    for (int i = 0; i < pojemnosc; i++)
    {
        Wezel *cur = kubelki[i];
        while (cur)
        {
            Wezel *t = cur->nast;
            delete cur;
            cur = t;
        }
    }
    delete[] kubelki;
}

// reszta z dzielenia przez pojemnosc.
// abs zabezpiecza przed ujemnym indeksem dla ujemnych kluczy.
int HashChaining::funkcjaMieszajaca(int klucz) const
{
    long long h = (long long)klucz % pojemnosc;
    if (h < 0)
        h += pojemnosc;
    return (int)h;
}

// powieksza tablice dwukrotnie i przepisuje wszystkie elementy.
// Indeksy musza byc przeliczone na nowo bo zmienila sie pojemnosc.
void HashChaining::rehash()
{
    int staraPoj = pojemnosc;
    Wezel **stare = kubelki;

    pojemnosc *= 2;
    kubelki = new Wezel *[pojemnosc];
    for (int i = 0; i < pojemnosc; i++)
        kubelki[i] = nullptr;

    // przenies wszystkie wezly do nowej tablicy
    for (int i = 0; i < staraPoj; i++)
    {
        Wezel *cur = stare[i];
        while (cur)
        {
            Wezel *nast = cur->nast;
            int idx = funkcjaMieszajaca(cur->klucz);
            cur->nast = kubelki[idx]; // wstaw na poczatek nowego kubelka
            kubelki[idx] = cur;
            cur = nast;
        }
    }
    delete[] stare;
}

// wstaw pare, jesli klucz juz istnieje zaktualizuj wartosc.
void HashChaining::wstaw(int klucz, int wartosc)
{
    int idx = funkcjaMieszajaca(klucz);

    // sprawdz czy klucz juz istnieje w kubelku
    Wezel *cur = kubelki[idx];
    while (cur)
    {
        if (cur->klucz == klucz)
        {
            cur->wartosc = wartosc;
            return;
        }
        cur = cur->nast;
    }

    // dopisz na poczatek kubelka
    Wezel *nowy = new Wezel(klucz, wartosc);
    nowy->nast = kubelki[idx];
    kubelki[idx] = nowy;
    rozmiar++;

    // sprawdz wspolczynnik zapelnienia
    if (rozmiar * 100 > pojemnosc * PROG_PROCENT)
        rehash();
}

// usun pare o danym kluczu, zwraca true jesli usunieto.
bool HashChaining::usun(int klucz)
{
    int idx = funkcjaMieszajaca(klucz);
    Wezel *cur = kubelki[idx];
    Wezel *prev = nullptr;

    while (cur)
    {
        if (cur->klucz == klucz)
        {
            if (prev)
                prev->nast = cur->nast;
            else
                kubelki[idx] = cur->nast;
            delete cur;
            rozmiar--;
            return true;
        }
        prev = cur;
        cur = cur->nast;
    }
    return false; // nie znaleziono
}

// szuka wartosci dla klucza, zwraca true i ustawia wynik jesli znaleziono.
bool HashChaining::znajdz(int klucz, int &wynik)
{
    int idx = funkcjaMieszajaca(klucz);
    Wezel *cur = kubelki[idx];
    while (cur)
    {
        if (cur->klucz == klucz)
        {
            wynik = cur->wartosc;
            return true;
        }
        cur = cur->nast;
    }
    return false;
}

int HashChaining::size()
{
    return rozmiar;
}

void HashChaining::wyswietl()
{
    if (rozmiar == 0)
    {
        cout << "(pusty)\n";
        return;
    }
    int wypisane = 0;
    for (int i = 0; i < pojemnosc && wypisane < 10; i++)
    {
        Wezel *cur = kubelki[i];
        while (cur && wypisane < 10)
        {
            cout << "[" << cur->klucz << "=>" << cur->wartosc << "] ";
            cur = cur->nast;
            wypisane++;
        }
    }
    if (rozmiar > 10)
        cout << "...";
    cout << " [rozmiar=" << rozmiar << " pojemnosc=" << pojemnosc << "]\n";
}

void HashChaining::wypelnij(const int *klucze, const int *wartosci, int n)
{
    // wyczysc stare dane
    for (int i = 0; i < pojemnosc; i++)
    {
        Wezel *cur = kubelki[i];
        while (cur)
        {
            Wezel *t = cur->nast;
            delete cur;
            cur = t;
        }
    }
    delete[] kubelki;
    pojemnosc = 16;
    rozmiar = 0;
    kubelki = new Wezel *[pojemnosc];
    for (int i = 0; i < pojemnosc; i++)
        kubelki[i] = nullptr;

    for (int i = 0; i < n; i++)
        wstaw(klucze[i], wartosci[i]);
}