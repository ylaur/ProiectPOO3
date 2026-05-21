#include "Tranzactie.h"
#include <ctime>

Tranzactie::Tranzactie(double valoare, const std::string& destinatie) : suma(valoare), destinatieIBAN(destinatie) {
    time_t timp = time(nullptr); // Folosim functie time din biblioteca ctime pentru a determina timpul
    tm* timpLocal = localtime(&timp); // Folosim localtime pentru a determina timpul de pe calculatorul nostru

    int an = timpLocal->tm_year + 1900; // Facem conversiile (i.e anii incep de la 1900, deci trebuie prelucrat)
    int luna = timpLocal->tm_mon + 1;
    int zi = timpLocal->tm_mday;
    int oraCurenta = timpLocal->tm_hour;
    int minut = timpLocal->tm_min;
    int secunda = timpLocal->tm_sec;

    data = std::to_string(an) + "/";

    if (luna < 10) { // Daca luna < 10, adaugam un 0
        data += "0";
    }
    data += std::to_string(luna) + "/";

    if (zi < 10) { // Daca ziua < 10, adaugam un 0
        data += "0";
    }
    data += std::to_string(zi);

    if (oraCurenta < 10) { // Daca ora < 10, adaugam un 0
        ora = "0";
    } else {
        ora = "";
    }
    ora += std::to_string(oraCurenta) + ":";

    if (minut < 10) { // Daca minutul < 10, adaugam un 0
        ora += "0";
    }
    ora += std::to_string(minut) + ":";

    if (secunda < 10) { // Daca secunda < 10, adaugam un 0
        ora += "0";
    }
    ora += std::to_string(secunda);
}

std::string Tranzactie::getDestinatie() const { return destinatieIBAN; }
double Tranzactie::getSuma() const { return suma; }
std::string Tranzactie::getData() const { return data; }
std::string Tranzactie::getOra() const { return ora; }

std::ostream& operator<<(std::ostream& out, const Tranzactie& t) {
    out << "Catre: " << t.getDestinatie() // Afisam IBAN-ul destinatie
        << " | Suma: " << t.getSuma() // Afisam suma
        << " | Data: " << t.data << " " << t.ora; // Afisam data la care a avut loc tranzactia
    return out;
}
