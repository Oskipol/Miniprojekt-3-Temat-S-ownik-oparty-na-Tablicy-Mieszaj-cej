#include "HashOpen.hpp"
#include <iostream>
using namespace std;

HashOpen::HashOpen() : pojemnosc(16), rozmiar(0)
{
    tablica = new Komorka[pojemnosc];
}

HashOpen::~HashOpen()
{
    delete[] tablica;
}

int HashOpen::funkcjaMieszajaca(int klucz) const
{
    long long h = (long long)klucz % pojemnosc;
    if (h < 0)
        h += pojemnosc;
    return (int)h;
}

// powieksza tablice dwukrotnie i przepisuje elementy zajete.
void HashOpen::rehash()
{
    int staraPoj = pojemnosc;
    Komorka *stara = tablica;

    pojemnosc *= 2;
    tablica = new Komorka[pojemnosc];
    rozmiar = 0;

    for (int i = 0; i < staraPoj; i++)
        if (stara[i].stan == ZAJETA)
            wstaw(stara[i].klucz, stara[i].wartosc);

    delete[] stara;
}

// liniowe probowanie do znalezienia wolnej/pasujacej komorki.
void HashOpen::wstaw(int klucz, int wartosc)
{
    if ((rozmiar + 1) * 100 > pojemnosc * PROG_PROCENT)
        rehash();

    int idx = funkcjaMieszajaca(klucz);
    int firstTombstone = -1;

    // skanuj caly lancuch probowania zatrzymaj sie na pustej lub obejdz tablice.
    for (int kroki = 0; kroki < pojemnosc; kroki++)
    {
        if (tablica[idx].stan == PUSTA)
            break;
        if (tablica[idx].stan == USUNIETA)
        {
            if (firstTombstone == -1)
                firstTombstone = idx;
        }
        else if (tablica[idx].klucz == klucz)
        { // klucz istnieje - aktualizuj
            tablica[idx].wartosc = wartosc;
            return;
        }
        idx = (idx + 1) % pojemnosc;
    }

    // nowy klucz - wstaw w pierwszym tombstone lub w komorce pustej
    if (firstTombstone != -1)
        idx = firstTombstone;
    tablica[idx].klucz = klucz;
    tablica[idx].wartosc = wartosc;
    tablica[idx].stan = ZAJETA;
    rozmiar++;
}

// oznacza komorke jako usunieta
bool HashOpen::usun(int klucz)
{
    int idx = funkcjaMieszajaca(klucz);
    int kroki = 0;

    while (tablica[idx].stan != PUSTA && kroki < pojemnosc)
    {
        if (tablica[idx].stan == ZAJETA && tablica[idx].klucz == klucz)
        {
            tablica[idx].stan = USUNIETA;
            rozmiar--;
            return true;
        }
        idx = (idx + 1) % pojemnosc;
        kroki++;
    }
    return false;
}

// szuka wartosci dla klucza.
bool HashOpen::znajdz(int klucz, int &wynik)
{
    int idx = funkcjaMieszajaca(klucz);
    int kroki = 0;

    while (tablica[idx].stan != PUSTA && kroki < pojemnosc)
    {
        if (tablica[idx].stan == ZAJETA && tablica[idx].klucz == klucz)
        {
            wynik = tablica[idx].wartosc;
            return true;
        }
        idx = (idx + 1) % pojemnosc;
        kroki++;
    }
    return false;
}

int HashOpen::size()
{
    return rozmiar;
}

void HashOpen::wyswietl()
{
    if (rozmiar == 0)
    {
        cout << "(pusty)\n";
        return;
    }
    int wypisane = 0;
    for (int i = 0; i < pojemnosc && wypisane < 10; i++)
    {
        if (tablica[i].stan == ZAJETA)
        {
            cout << "[" << tablica[i].klucz << "=>" << tablica[i].wartosc << "] ";
            wypisane++;
        }
    }
    if (rozmiar > 10)
        cout << "...";
    cout << " [rozmiar=" << rozmiar << " pojemnosc=" << pojemnosc << "]\n";
}

void HashOpen::wypelnij(const int *klucze, const int *wartosci, int n)
{
    delete[] tablica;
    pojemnosc = 16;
    rozmiar = 0;
    tablica = new Komorka[pojemnosc];
    for (int i = 0; i < n; i++)
        wstaw(klucze[i], wartosci[i]);
}