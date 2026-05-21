#pragma once
#include <string>
#include <iostream>

class Tranzactie {
private:
    double suma; // Suma tranzactiei
    std::string destinatieIBAN; // IBAN catre care va fi trimisa suma
    std::string data; // Data la care s-a efectuat tranzictia
    std::string ora; // Ora ...
public:
    Tranzactie(double valoare, const std::string& destinatie); // Constructor

    std::string getDestinatie() const; // Getter pentru destinatieIBAN
    double getSuma() const; // Getter pentru suma
    std::string getData() const; // Getter pentru data curenta
    std::string getOra() const; // Getter pentru ora curenta

    friend std::ostream& operator<<(std::ostream& out, const Tranzactie& t); // Overload pe operator<< pentru afisare a unei tranzactii
};
