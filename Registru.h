#pragma once
#include <vector>
#include <iostream>

template <typename T> // Clasa Sablon
class Registru {
private:
    std::vector<T> inregistrari; // Vector care memoreaza toate inregistrarile
public:
    void adaugaInregistrare(const T& inregistrare) { // Metoda ce adauga o inregistrare in vector
        inregistrari.push_back(inregistrare); // Folosim metoda push_back specifica cointainerului vector pentru a adauga la final
    }
    const std::vector<T>& getInregistrari() const { // Metoda ce retunreaza inregistrarile
        return inregistrari;
    }

    void afiseazaTot() const { // Metoda ce afiseaza toate inregistrarile
        for (const auto& inregistrare : inregistrari) { // Parcurgem cu un loop si afisam fiecare inregistrare
            std::cout << inregistrare << "\n";
        }
    }

    void stergeInregistrari() { // Metoda care Stergem toate inregistrarile
        inregistrari.clear(); // Folosim metoda clear din cointainerul vector pentru a sterge toate inregistrarile
    }

    void stergeUltimaInregistrare() { // Metoda ce sterge ultima inregistrare
        if (!inregistrari.empty()) { // Verificam daca vectorul nu are elemente
            inregistrari.pop_back();
        }
    }

    void stergeInregistrare(unsigned int pozitie) { // Metoda ce sterge o inregistrare de la un index
        if (inregistrari.empty()) { // Verificam daca avem ce sterge
            return;
        }
        if (pozitie >= inregistrari.size()) { // Daca indexul furnizat este mai mare decat nr. de inregistrari, o stergemm pe ultima
            pozitie = inregistrari.size() - 1;
        }
        auto itPozitie = inregistrari.begin() + pozitie;
        inregistrari.erase(itPozitie); // Metoda erase accepta doar iteratori
    }

    void inserareInregistrare(unsigned int pozitie, const T& inregistrare) { // Metoda ce insereaza o inregistrare la un index anume
        if (pozitie > inregistrari.size()) { // Daca indexul e prea mare, inseram la final
            pozitie = inregistrari.size();
        }
        inregistrari.insert(inregistrari.begin() + pozitie, inregistrare);
    }
};
