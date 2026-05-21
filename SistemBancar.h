#pragma once
#include <vector>
#include <string>
#include "Conturi.h"
#include "Registru.h"

class SistemBancar { // Clasa SitemBancar (design pattern, Singleton)
private:
    static SistemBancar* instanta; // Unica instanta a sistemului bancar
    std::vector<Cont*> conturi; // Vector ce contine toate conturile deschise
    Registru<std::string> jurnalSistem; // Jurnalul sistemului

    SistemBancar(); // Constructor privat
    SistemBancar(const SistemBancar&) = delete; // Constructor de copiere sters
    SistemBancar& operator=(const SistemBancar&) = delete; // Operator= sters

public:
    static SistemBancar* getInstanta(); // Functie ce returneaza unica instanta a sistemului bancar
    ~SistemBancar(); // Destructor pentru vector

    void adaugaCont(Cont* cont); // Metoda ce adauga un cont in sistemul bancar
    void eliminaCont(Cont* cont); // Metoda pentru eliminarea unui cont
    void eliminaContDupaCNP(unsigned long long cnp, unsigned long long numarConturi = 1); // Metoda ce elimina conturi dupa CNP

    void transferaFonduri(const std::string& dinIban, const std::string& catreIban, double suma); // Metoda ce transfera fonduri
    void actualizeazaToateLimiteleCredit(double nouaLimita); // Metoda ce actualizeaza limita de credit
    void ruleazaDetectieFrauda(); // Metoda ce detecteaza eventualele fraude
    void printeazaJurnal() const; // Metoda ce afiseaza jurnalul de sistem al bancii

    Cont* extrageContDupaIndex(unsigned long long index); // Metoda ce extrage un cont dupa indexul sau
};
