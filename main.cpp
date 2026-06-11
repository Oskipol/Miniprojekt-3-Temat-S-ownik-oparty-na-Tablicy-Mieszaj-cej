#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include "Slownik.hpp"
#include "HashChaining.hpp"
#include "HashOpen.hpp"
#include "HashAVL.hpp"
using namespace std;

// mt19937 (pelny zakres int, lepsza jakosc niz rand)
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int losujWartosc()
{
    return (int)rng();
}
int losujKlucz(int maxKlucz)
{
    uniform_int_distribution<int> dist(0, maxKlucz);
    return dist(rng);
}
int losujIndeks(int n)
{
    uniform_int_distribution<int> dist(0, n - 1);
    return dist(rng);
}

// aktualny czas w nanosekundach

long long nanos()
{
    return chrono::duration_cast<chrono::nanoseconds>(
               chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}

// Mierzy insert i remove dla 3 wariantow tablicy mieszajacej.
// Przed kazdym pomiarem struktura jest resetowana do N elementow,

void benchmark(bool waskiZakres)
{
    const int ROZMIARY[] = {10000, 25000, 50000, 100000, 200000, 350000, 500000, 1000000};
    const int ILE = 8;
    const int OPS = 10000; // operacji na pomiar
    const int POWT = 8;    // powtorzenia dla usrednienia

    const char *nazwy[] = {"Chaining", "OpenAddr", "AVL"};

    cout << "\n"
         << (waskiZakres ? "WASKI ZAKRES KLUCZY (duzo kolizji)" : "SZEROKI ZAKRES KLUCZY (malo kolizji)") << "\n";
    cout << "N\t";
    for (int s = 0; s < 3; s++)
        cout << nazwy[s] << "_insert\t" << nazwy[s] << "_remove\t";
    cout << "\n";

    for (int s = 0; s < ILE; s++)
    {
        int N = ROZMIARY[s];

        // Zakres kluczy zalezny od trybu
        int maxKlucz = waskiZakres ? (N / 10) : (N * 5);

        long long czasy[3][2] = {}; // 3 struktury x 2 operacje

        for (int r = 0; r < POWT; r++)
        {

            // Wspolne dane dla wszystkich struktur
            int *klucze = new int[N];
            int *wartosci = new int[N];
            for (int i = 0; i < N; i++)
            {
                klucze[i] = losujKlucz(maxKlucz);
                wartosci[i] = losujWartosc();
            }

            // OPS nowych kluczy do wstawienia (do pomiaru insert)
            int *doWstaw = new int[OPS];
            for (int i = 0; i < OPS; i++)
                doWstaw[i] = losujKlucz(maxKlucz);

            // OPS kluczy do usuniecia — losowane ze zrodla zeby istnialy w strukturze
            int *doUsun = new int[OPS];
            for (int i = 0; i < OPS; i++)
                doUsun[i] = klucze[losujIndeks(N)];

            // Tworzymy struktury w petli przez wskaznik na klase bazowa
            for (int idx = 0; idx < 3; idx++)
            {
                Slownik *sl;
                if (idx == 0)
                    sl = new HashChaining();
                else if (idx == 1)
                    sl = new HashOpen();
                else
                    sl = new HashAVL();

                long long t0, t1;

                // insert — reset do N, potem wstaw OPS nowych par
                sl->wypelnij(klucze, wartosci, N);
                t0 = nanos();
                for (int j = 0; j < OPS; j++)
                    sl->wstaw(doWstaw[j], losujWartosc());
                t1 = nanos();
                czasy[idx][0] += t1 - t0;

                // remove — reset do N, potem usun OPS kluczy
                sl->wypelnij(klucze, wartosci, N);
                t0 = nanos();
                for (int j = 0; j < OPS; j++)
                    sl->usun(doUsun[j]);
                t1 = nanos();
                czasy[idx][1] += t1 - t0;

                delete sl;
            }

            delete[] klucze;
            delete[] wartosci;
            delete[] doWstaw;
            delete[] doUsun;
        }

        // Usredniony czas jednej operacji w nanosekundach
        cout << N << "\t";
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 2; j++)
                cout << czasy[i][j] / POWT / OPS << "\t";
        cout << "\n";
    }
}

void podmenu(Slownik *s, const char *nazwa)
{
    const char *opcje[] = {
        "1. Utworz losowo",
        "3. Insert (dodaj pare klucz-wartosc)",
        "4. Remove (usun po kluczu)",
        "5. Znajdz (po kluczu)",
        "6. Rozmiar",
        "7. Wyswietl",
        "0. Powrot"};

    const int MAX_KLUCZ = 500000;

    int wybor;
    do
    {
        cout << "\n"
             << nazwa << "\n";
        for (const char *o : opcje)
            cout << o << "\n";
        cout << "Wybor: ";
        cin >> wybor;

        if (wybor == 1)
        {
            int n;
            cout << "Ile elementow? ";
            cin >> n;
            int *k = new int[n];
            int *w = new int[n];
            for (int i = 0; i < n; i++)
            {
                k[i] = losujKlucz(MAX_KLUCZ);
                w[i] = losujWartosc();
            }
            s->wypelnij(k, w, n); // wypelnij usuwa stare dane przed wczytaniem nowych
            delete[] k;
            delete[] w;
            cout << "Utworzono " << n << " elementow.\n";
        }

        else if (wybor == 2)
        {
            int k, w;
            cout << "Klucz: ";
            cin >> k;
            cout << "Wartosc: ";
            cin >> w;
            s->wstaw(k, w);
            cout << "Dodano.\n";
        }

        else if (wybor == 3)
        {
            int k;
            cout << "Klucz do usuniecia: ";
            cin >> k;
            if (s->usun(k))
                cout << "Usunieto.\n";
            else
                cout << "Nie znaleziono klucza.\n";
        }

        else if (wybor == 4)
        {
            int k;
            cout << "Klucz do znalezienia: ";
            cin >> k;
            int wynik;
            if (s->znajdz(k, wynik))
                cout << "Wartosc: " << wynik << "\n";
            else
                cout << "Nie znaleziono klucza.\n";
        }

        else if (wybor == 5)
        {
            cout << "Rozmiar: " << s->size() << "\n";
        }

        else if (wybor == 6)
        {
            s->wyswietl();
        }

    } while (wybor != 0);
}

int main()
{
    const char *opcje[] = {
        "1. Tablica mieszajaca - lancuchowanie (chaining)",
        "2. Tablica mieszajaca - adresowanie otwarte",
        "3. Tablica mieszajaca - kubelki AVL",
        "4. Benchmark (malo kolizji)",
        "5. Benchmark (duzo kolizji)",
        "0. Wyjdz"};

    int wybor;
    do
    {
        cout << "\n=== SLOWNIK - TABLICA MIESZAJACA ===\n";
        for (const char *o : opcje)
            cout << o << "\n";
        cout << "Wybor: ";
        cin >> wybor;

        if (wybor == 1)
        {
            HashChaining h;
            podmenu(&h, "Lancuchowanie");
        }
        else if (wybor == 2)
        {
            HashOpen h;
            podmenu(&h, "Adresowanie otwarte");
        }
        else if (wybor == 3)
        {
            HashAVL h;
            podmenu(&h, "Kubelki AVL");
        }
        else if (wybor == 4)
        {
            benchmark(false);
        }
        else if (wybor == 5)
        {
            benchmark(true);
        }

    } while (wybor != 0);

    return 0;
}