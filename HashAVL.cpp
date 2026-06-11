#include "HashAVL.hpp"
#include <iostream>
using namespace std;

HashAVL::HashAVL() : pojemnosc(16), rozmiar(0)
{
    kubelki = new Wezel *[pojemnosc];
    for (int i = 0; i < pojemnosc; i++)
        kubelki[i] = nullptr;
}

HashAVL::~HashAVL()
{
    for (int i = 0; i < pojemnosc; i++)
        usunDrzewo(kubelki[i]);
    delete[] kubelki;
}

// rekurencyjnie usuwa wszystkie wezly drzewa
void HashAVL::usunDrzewo(Wezel *w)
{
    if (!w)
        return;
    usunDrzewo(w->lewy);
    usunDrzewo(w->prawy);
    delete w;
}

int HashAVL::funkcjaMieszajaca(int klucz) const
{
    long long h = (long long)klucz % pojemnosc;
    if (h < 0)
        h += pojemnosc;
    return (int)h;
}

int HashAVL::wysokosc(Wezel *w) const
{
    return w ? w->wysokosc : 0;
}

// balans=wysokosc lewego - wysokosc prawego poddrzewa
int HashAVL::balans(Wezel *w) const
{
    return w ? wysokosc(w->lewy) - wysokosc(w->prawy) : 0;
}

void HashAVL::aktualizujWysokosc(Wezel *w)
{
    int hl = wysokosc(w->lewy);
    int hp = wysokosc(w->prawy);
    w->wysokosc = (hl > hp ? hl : hp) + 1;
}

// rotacja w prawo - naprawia przechylenie w lewo
HashAVL::Wezel *HashAVL::rotacjaPrawo(Wezel *y)
{
    Wezel *x = y->lewy;
    Wezel *T = x->prawy;
    x->prawy = y;
    y->lewy = T;
    aktualizujWysokosc(y);
    aktualizujWysokosc(x);
    return x; // nowy korzen poddrzewa
}

// rotacja w lewo - naprawia przechylenie w prawo
HashAVL::Wezel *HashAVL::rotacjaLewo(Wezel *x)
{
    Wezel *y = x->prawy;
    Wezel *T = y->lewy;
    y->lewy = x;
    x->prawy = T;
    aktualizujWysokosc(x);
    aktualizujWysokosc(y);
    return y;
}

// wstaw do drzewa AVL z zachowaniem balansu. dodano=false jesli tylko aktualizacja.
HashAVL::Wezel *HashAVL::wstawAVL(Wezel *w, int klucz, int wartosc, bool &dodano)
{
    if (!w)
    {
        dodano = true;
        return new Wezel(klucz, wartosc);
    }

    if (klucz < w->klucz)
        w->lewy = wstawAVL(w->lewy, klucz, wartosc, dodano);
    else if (klucz > w->klucz)
        w->prawy = wstawAVL(w->prawy, klucz, wartosc, dodano);
    else
    {
        w->wartosc = wartosc;
        dodano = false;
        return w;
    } // klucz istnieje

    aktualizujWysokosc(w);
    int b = balans(w);

    // cztery przypadki niezbalansowania
    if (b > 1 && klucz < w->lewy->klucz)
        return rotacjaPrawo(w); // LL
    if (b < -1 && klucz > w->prawy->klucz)
        return rotacjaLewo(w); // RR
    if (b > 1 && klucz > w->lewy->klucz)
    { // LR
        w->lewy = rotacjaLewo(w->lewy);
        return rotacjaPrawo(w);
    }
    if (b < -1 && klucz < w->prawy->klucz)
    { // RL
        w->prawy = rotacjaPrawo(w->prawy);
        return rotacjaLewo(w);
    }
    return w;
}

// znajdz wezel o najmniejszym kluczu (najbardziej w lewo)
HashAVL::Wezel *HashAVL::minWezel(Wezel *w) const
{
    while (w->lewy)
        w = w->lewy;
    return w;
}

// usun z drzewa AVL z zachowaniem balansu
HashAVL::Wezel *HashAVL::usunAVL(Wezel *w, int klucz, bool &usunieto)
{
    if (!w)
    {
        usunieto = false;
        return nullptr;
    }

    if (klucz < w->klucz)
        w->lewy = usunAVL(w->lewy, klucz, usunieto);
    else if (klucz > w->klucz)
        w->prawy = usunAVL(w->prawy, klucz, usunieto);
    else
    {
        // znaleziono wezel do usuniecia
        usunieto = true;
        if (!w->lewy || !w->prawy)
        {
            // Zero lub jedno dziecko
            Wezel *dziecko = w->lewy ? w->lewy : w->prawy;
            delete w;
            return dziecko;
        }
        else
        {
            // dwoje dzieci - zastap nastepnikiem (min z prawego poddrzewa)
            Wezel *nast = minWezel(w->prawy);
            w->klucz = nast->klucz;
            w->wartosc = nast->wartosc;
            bool pom;
            w->prawy = usunAVL(w->prawy, nast->klucz, pom);
        }
    }

    aktualizujWysokosc(w);
    int b = balans(w);

    // rebalans po usunieciu
    if (b > 1 && balans(w->lewy) >= 0)
        return rotacjaPrawo(w);
    if (b > 1 && balans(w->lewy) < 0)
    {
        w->lewy = rotacjaLewo(w->lewy);
        return rotacjaPrawo(w);
    }
    if (b < -1 && balans(w->prawy) <= 0)
        return rotacjaLewo(w);
    if (b < -1 && balans(w->prawy) > 0)
    {
        w->prawy = rotacjaPrawo(w->prawy);
        return rotacjaLewo(w);
    }

    return w;
}

HashAVL::Wezel *HashAVL::znajdzAVL(Wezel *w, int klucz) const
{
    while (w)
    {
        if (klucz < w->klucz)
            w = w->lewy;
        else if (klucz > w->klucz)
            w = w->prawy;
        else
            return w;
    }
    return nullptr;
}

// powieksza tablice i przepisuje wszystkie elementy
void HashAVL::rehash()
{
    int staraPoj = pojemnosc;
    Wezel **stare = kubelki;

    pojemnosc *= 2;
    kubelki = new Wezel *[pojemnosc];
    for (int i = 0; i < pojemnosc; i++)
        kubelki[i] = nullptr;
    rozmiar = 0;

    // przepisz kazde drzewo do nowej tablicy
    for (int i = 0; i < staraPoj; i++)
    {
        przepiszDrzewo(stare[i]);
        usunDrzewo(stare[i]);
    }
    delete[] stare;
}

// rekurencyjnie wstawia wszystkie wezly drzewa do nowej tablicy
void HashAVL::przepiszDrzewo(Wezel *w)
{
    if (!w)
        return;
    wstaw(w->klucz, w->wartosc);
    przepiszDrzewo(w->lewy);
    przepiszDrzewo(w->prawy);
}

void HashAVL::wstaw(int klucz, int wartosc)
{
    int idx = funkcjaMieszajaca(klucz);
    bool dodano = false;
    kubelki[idx] = wstawAVL(kubelki[idx], klucz, wartosc, dodano);
    if (dodano)
    {
        rozmiar++;
        if (rozmiar * 100 > pojemnosc * PROG_PROCENT)
            rehash();
    }
}

bool HashAVL::usun(int klucz)
{
    int idx = funkcjaMieszajaca(klucz);
    bool usunieto = false;
    kubelki[idx] = usunAVL(kubelki[idx], klucz, usunieto);
    if (usunieto)
        rozmiar--;
    return usunieto;
}

bool HashAVL::znajdz(int klucz, int &wynik)
{
    int idx = funkcjaMieszajaca(klucz);
    Wezel *w = znajdzAVL(kubelki[idx], klucz);
    if (w)
    {
        wynik = w->wartosc;
        return true;
    }
    return false;
}

int HashAVL::size()
{
    return rozmiar;
}

void HashAVL::wyswietlDrzewo(Wezel *w, int &wypisane)
{
    if (!w || wypisane >= 10)
        return;
    wyswietlDrzewo(w->lewy, wypisane);
    if (wypisane < 10)
    {
        cout << "[" << w->klucz << "=>" << w->wartosc << "] ";
        wypisane++;
    }
    wyswietlDrzewo(w->prawy, wypisane);
}

void HashAVL::wyswietl()
{
    if (rozmiar == 0)
    {
        cout << "(pusty)\n";
        return;
    }
    int wypisane = 0;
    for (int i = 0; i < pojemnosc && wypisane < 10; i++)
        wyswietlDrzewo(kubelki[i], wypisane);
    if (rozmiar > 10)
        cout << "...";
    cout << " [rozmiar=" << rozmiar << " pojemnosc=" << pojemnosc << "]\n";
}

void HashAVL::wypelnij(const int *klucze, const int *wartosci, int n)
{
    for (int i = 0; i < pojemnosc; i++)
        usunDrzewo(kubelki[i]);
    delete[] kubelki;
    pojemnosc = 16;
    rozmiar = 0;
    kubelki = new Wezel *[pojemnosc];
    for (int i = 0; i < pojemnosc; i++)
        kubelki[i] = nullptr;
    for (int i = 0; i < n; i++)
        wstaw(klucze[i], wartosci[i]);
}