#include "Client.h"
#include <cstring>
#include <new>
#include <utility>

bool Client::validareCNP(unsigned long long cnp) {
    // Format CNP: SAALLZZJJNNNC - 13 cifre
    // Rol: Valideaza un CNP, mai multe detalii la: https://ro.wikipedia.org/wiki/Cod_numeric_personal_(Rom%C3%A2nia)
    unsigned long long copieCNP = cnp / 10;

    if (cnp < 1000000000000ULL || cnp >= 10000000000000ULL) { // Contine DOAR 13 Cifre
        return 0;
    }

    unsigned short int CifraControl = cnp % 10; // Salvam C intr-o variabila pentru a fi ulterior verificat
    cnp /= 10;

    if (cnp % 1000 == 0) return 0;
    cnp /= 1000; // Taiem NNN (NNN apartine intervalului [1, 999])

    unsigned short int JJ = cnp % 100; // Salvam JJ = Cod Judet
    if (JJ != 51 && JJ != 52 && JJ != 70 && (JJ < 1 || JJ > 48)) {
        return 0;
    }
    cnp /= 100;

    if (cnp % 100 < 1 || cnp % 100 > 31) return 0; // Validam Luna
    cnp /= 100;

    unsigned short int LL = cnp % 100; // Salvam LL = Luna
    if (LL < 1 || LL > 12) return 0;
    cnp /= 100;

    unsigned short int AA = cnp % 100; // Salvam AA = An
    cnp /= 100;

    unsigned short int S = cnp % 100; // Salvam S

    // Caz particular: Sectorul 7 si 8 desfiintate in August 1979
    if ((JJ == 47 || JJ == 48) && LL >= 8 && AA >= 79) return 0;

    if (S != 1 && S != 2 && S != 5 && S != 6) return 0; // Validam S
    if ((S == 5 || S == 6) && AA > 26) return 0; // Cineva nascut dupa 2000 nu poate avea ult. 2 cif. din an > 26

    unsigned long long validC = 0; // Validam C
    unsigned long long CONST = 279146358279ULL;
    while (copieCNP) {
        validC += (copieCNP % 10) * (CONST % 10);
        CONST /= 10;
        copieCNP /= 10;
    }
    validC %= 11;
    if (validC < 10 && CifraControl != validC) return 0;
    if (validC == 10 && CifraControl != validC - 9) return 0;

    return 1; // Daca am ajuns aici, CNP-ul este VALID
}

Client::Client(const char* numeClient, unsigned long long cnp) {
    if (numeClient != nullptr) { // Determinam daca numele clientului este valid
        try {
            nume = new char[strlen(numeClient) + 1]; // Incercam sa alocam memorie
            strcpy(nume, numeClient);
        }
        catch (const std::bad_alloc& e) {
            nume = nullptr; // Daca alocarea esueaza, nume devine nullptr si afisam un mesaj specific
            std::cout << e.what() << std::endl;
        }
    }
    else {
        nume = nullptr; // Daca numele nu e valid, nume devine nullptr
    }
    CNP = (validareCNP(cnp) ? cnp : 0); // Verificam validitatea CNP-ului
    if (CNP == 0)
        areCNPvalid = false;
    else
        areCNPvalid = true;
}

Client::~Client() {
    if (nume != nullptr) { // Eliberam memoria alocata
        delete[] nume;
    }
}

Client::Client(const Client& c) {
    if (c.nume != nullptr) { // Determinam daca numele clientului este valid
        try {
            nume = new char[strlen(c.nume) + 1]; // Incercam sa alocam memorie
            strcpy(nume, c.nume);
        }
        catch (const std::bad_alloc& e) {
            nume = nullptr; // Daca alocarea esueaza, nume devine nullptr si afisam un mesaj specific
            std::cout << e.what() << std::endl;
        }
    }
    else {
        nume = nullptr; // Daca numele nu e valid, nume devine nullptr
    }
    CNP = c.CNP; // Atribuim CNP-ul
    areCNPvalid = c.areCNPvalid;
}

Client& Client::operator=(Client c) {
    // Am folosit Copy and Swap Idiom
    // Transmitem Clinetul prin valoare si facem swap in functie
    // Astfel, evitam erorile de copiere partiala
    swap(*this, c); // Functie de swap intre 2 clienti
    return *this;
}

bool Client::esteCNPvalid() const { return areCNPvalid; }
unsigned long long Client::getCNP() const { return CNP; }
const char* Client::getNume() const { return nume; }

void Client::schimbaNume(const char* numeClient) {
    char *numeAuxiliar = nullptr;
    if (numeClient != nullptr) { // Determinam daca numele clientului este valid
        try {
            numeAuxiliar = new char[strlen(numeClient) + 1]; // Incercam sa alocam memorie
            strcpy(numeAuxiliar, numeClient);
        }
        catch (const std::bad_alloc& e) {
            std::cout << e.what() << std::endl; // Daca nu a functionat, afisam  un mesaj specific
        }
        delete[] nume;
        nume = numeAuxiliar;
    }
    else {
        return;
    }
}

void swap(Client& unu, Client& doi) {
    std::swap(unu.nume, doi.nume); // Facem swap pe fiecare membru
    std::swap(unu.CNP, doi.CNP);
    std::swap(unu.areCNPvalid, doi.areCNPvalid);
}

std::ostream& operator<<(std::ostream& out, const Client& c) {
    out << "Nume Client: " << (c.nume ? c.nume : "Fara nume") << "\n"
        << "CNP: " << c.CNP;
    return out;
}
