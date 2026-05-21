#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "Exceptii.h"
#include "Registru.h"
#include "Tranzactie.h"
#include "Client.h"
#include "Conturi.h"
#include "SistemBancar.h"

template <typename T> // Functie template
void genereazaRaport(const T& entitate, const std::string& titluRaport) { // Afiseaza un raport al unei entitati (i.e tranzactii, string)
    std::cout << "RAPORT: " << titluRaport << "\n";
    std::cout << entitate << "\n\n";
}

int main() {
    srand((time(0)));

    // Initializam sistemul bancar
    SistemBancar* banca = SistemBancar::getInstanta();
    // Initalizam Factory-ul
    ContFactory factory;

    // Creem clienti
    std::vector<Client> clienti = {
        Client("Mihai Popescu", 1900515410010ULL), // Valid
        Client("Elena Ionescu", 2920820410021ULL), // Valid
        Client("Fraudulescu Ion", 5010203123456ULL), // Invalid
        Client("Andrei Vasile", 5051210011230ULL), // Valid
        Client("Maria Radu", 6080417124563ULL), // Valid
        Client("Gigel Nesuportat", 6020304234567ULL), // Invalid
        Client("Ion Georgescu", 1851125229994ULL), // Valid
        Client("Ana Dumitru", 2700101351111ULL), // Valid
        Client("Vasile Lupu", 1990909415558ULL), // Valid
        Client("Cristina Stan", 2880228083338ULL) // Valid
    };

    std::vector<TipCont> tipuriCont = { // Adaugam tipuri de conturi pentru fiecare client
        CONT_ECONOMIE, CONT_CREDIT, CONT_ECONOMIE, CONT_CREDIT, CONT_ECONOMIE,
        CONT_CREDIT, CONT_ECONOMIE, CONT_CREDIT, CONT_ECONOMIE, CONT_ECONOMIE
    };

    std::vector<double> solduri = { // Adaugam solduri pentu fiecare client
        5000.0, 1000.0, 2000.0, 1500.0, 8000.0,
        3000.0, 4500.0, 2500.0, 6000.0, 7000.0
    };

    // Creem conturile
    for (size_t i = 0; i < clienti.size(); ++i) {
        try {
            Cont* contNou = factory.creeazaCont(tipuriCont[i], clienti[i], solduri[i]);
            banca->adaugaCont(contNou);
            std::cout << "Cont deschis pentru: " << (clienti[i].getNume() ? clienti[i].getNume() : "Nume invalid") << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "Nu s-a deschis cont pentru " << (clienti[i].getNume() ? clienti[i].getNume() : "Nume invalid")
                      << ": " << e.what() << "\n";
        }
    }


    // Actualizam linia de credit pentru cardurile de credit
    banca->actualizeazaToateLimiteleCredit(5000.0);

    // Extragem primele doua conturi valide din sistemul bancar pentru a vedea rolul tranzactii
    Cont* contEco = banca->extrageContDupaIndex(0);
    Cont* contCred = banca->extrageContDupaIndex(1);

    if (contEco != nullptr && contCred != nullptr) {
        std::string ibanEco = contEco->getIban(); // Luam primul IBAN
        std::string ibanCred = contCred->getIban(); // Luam al doilea IBAN

        std::cout << "\n";
        try {
            banca->transferaFonduri(ibanEco, ibanCred, 250.0); // Efectuam un trasnfer de fonduri
            banca->transferaFonduri(ibanEco, ibanCred, 300.0);
            banca->transferaFonduri(ibanEco, ibanCred, 100.0);
            banca->transferaFonduri("RO99FALS123", ibanCred, 50.0);
        }
        catch (const std::exception& e) { // Prindem erorile
            std::cout << "Exceptie prinsa: " << e.what() << "\n";
        }

        banca->ruleazaDetectieFrauda();

        try {
            banca->transferaFonduri(ibanEco, ibanCred, 50.0);
            banca->transferaFonduri(ibanEco, ibanCred, 100.0);
            banca->transferaFonduri(ibanEco, ibanCred, 600.0);
        }
        catch (const std::exception& e) {
            std::cout << "Interventie sistem blocata: " << e.what() << "\n";
        }
        std::cout << "\n\n";

        banca->ruleazaDetectieFrauda(); // Rulam metoda pentru a detecta fraudele

        genereazaRaport(*contEco, "Situatia Cont Sursa");
        genereazaRaport(*contCred, "Situatie Cont Destinatie");
    }

    Tranzactie Test(999.0, "RO00TEST000000"); // Afisam o tranzactie
    genereazaRaport(Test, "Verificare tranzactie");

    banca->printeazaJurnal();

    delete banca;

    return 0;
}
