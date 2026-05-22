#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <ctime>
#include <cstring>
#include <cmath>
#include <new>

class FonduriInsuficienteException : public std::exception { // Clasa de exceptii
public:
    const char* what() const noexcept override { // Schimbam eroarea standard cu un mesaj mai simplu
        return "Eroare fonduri: Fonduri insuficiente pentru a finaliza tranzactia";
    }
};

class ContBlocatException : public std::exception { // Clasa de exceptii
public:
    const char* what() const noexcept override { // Schimbam eroarea standard cu un mesaj mai simplu
        return "Operatiune refuzata: Contul este blocat";
    }
};

class EroareAlocareMemorie : public std::bad_alloc { // Clasa de exceptii
public:
    const char* what() const noexcept override { // Schimbam eroarea standard cu un mesaj mai simplu
        return "Eroare alocare: Nu s-a putut aloca memoria necesara";
    }
};

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

class Tranzactie {
private:
    double suma; // Suma tranzactiei
    std::string destinatieIBAN; // IBAN catre care va fi trimisa suma
    std::string data; // Data la care s-a efectuat tranzictia
    std::string ora; // Ora ...
public:
    Tranzactie(double valoare, const std::string& destinatie) : suma(valoare), destinatieIBAN(destinatie) { // Constructor
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

    std::string getDestinatie() const { return destinatieIBAN; } // Getter pentru destinatieIBAN
    double getSuma() const { return suma; } // Getter pentru suma
    std::string getData() const { return data; } // Getter pentru data curenta
    std::string getOra() const { return ora; } // Getter pentru ora curenta

    friend std::ostream& operator<<(std::ostream& out, const Tranzactie& t) { // Overload pe operator<< pentru afisare a unei tranzactii
        out << "Catre: " << t.getDestinatie() // Afisam IBAN-ul destinatie
            << " | Suma: " << t.getSuma() // Afisam suma
            << " | Data: " << t.data << " " << t.ora; // Afisam data la care a avut loc tranzactia
        return out;
    }
};

enum CategorieStadiu { // Enumeratie TipStadiu
    ANGAJAT, // Persoana anagajata
    STUDENT, // Studenti
    INACTIV, // Persoane sub 18 ani SAU aflate la pensie
    ALT_STADIU // Persoane care nu intra in categoriile de mai sus
};

class Client { // Clasa Client
private:
    char* nume; // Numele clientului
    unsigned long long CNP; // CNP-ul clientului
    bool areCNPvalid; // Validitatea CNP-ului
    CategorieStadiu tipStadiu;

    bool validareCNP(unsigned long long cnp) { // Metoda privata ce determina daca un CNP este valid
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

public:
    Client(const char* numeClient, unsigned long long cnp, CategorieStadiu ts) { // Constructor
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

        switch (ts) { // Determinam categoria in care se afla persoana
            case ANGAJAT:
                tipStadiu = ANGAJAT;
                break;
            case STUDENT:
                tipStadiu = STUDENT;
                break;
            case INACTIV:
                tipStadiu = INACTIV;
                break;
            default:
                tipStadiu = ALT_STADIU;
        }
    }

    ~Client() { // Destructor
        if (nume != nullptr) { // Eliberam memoria alocata
            delete[] nume;
        }
    }

    Client(const Client& c) { // Costructor de copiere
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

        tipStadiu = c.tipStadiu;
    }

    Client& operator=(Client c) { // Overloading pe operator=
        // Am folosit Copy and Swap Idiom
        // Transmitem Clinetul prin valoare si facem swap in functie
        // Astfel, evitam erorile de copiere partiala
        swap(*this, c); // Functie de swap intre 2 clienti
        return *this;
    }

    bool esteCNPvalid() const { return areCNPvalid; } // Metoda ce returneaza validitatea CNP-ului
    unsigned long long getCNP() const { return CNP; } // Metoda ce returneaza CNP-ul
    CategorieStadiu getTipStadiu() const { return tipStadiu; } // Metoda ce returneaza stadiul
    const char* getNume() const { return nume; } // Metoda ce returneaza numele

    void schimbaNume(const char* numeClient) {
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

    friend void swap(Client& unu, Client& doi) { // Metoda ce interschimba 2 clienti
        std::swap(unu.nume, doi.nume); // Facem swap pe fiecare membru
        std::swap(unu.CNP, doi.CNP);
        std::swap(unu.areCNPvalid, doi.areCNPvalid);
        std::swap(unu.tipStadiu, doi.tipStadiu);
    }

    friend std::ostream& operator<<(std::ostream& out, const Client& c) { // Overloading pe operator<<
        out << "Nume Client: " << (c.nume ? c.nume : "Fara nume") << "\n"
            << "CNP: " << c.CNP;
        return out;
    }
};

class PercepeComision { // Clasa de baza PercepeComision (design pattern, Strategy)
protected:
    double comision;
public:
    PercepeComision(double c) : comision(c) {}
    virtual double calculeazaComision(double suma) = 0; // Metoda virtuala pura
    double getComision() const { return comision; }
    virtual ~PercepeComision() = default; // Desctructor virtual
};

class ComisionStandard : public PercepeComision { // Clasa derivata ComisionStandard
public:
    ComisionStandard(double comision = 0.01) : PercepeComision(comision) {}
    double calculeazaComision(double suma) { // Metoda ce calculeaza comisionul aplicat pe o suma (1%)
        return suma * comision; // Subtituire comision
    }
};

class ComisionAngajat : public PercepeComision { // Clasa derivata ComisionStandard
public:
    ComisionAngajat(double comision = 0.007) : PercepeComision(comision) {}
    double calculeazaComision(double suma) { // Metoda ce calculeaza comisionul aplicat pe o suma (0.7%)
        return suma * comision; // Subtituire comision
    }
};

class ComisionStudent : public PercepeComision { // Clasa derivata ComisionStandard
public:
    ComisionStudent(double comision = 0) : PercepeComision(comision) {}
    double calculeazaComision(double suma) { // Metoda ce calculeaza comisionul aplicat pe o suma (0%)
        return comision; // In acest caz, nu se aplica comision
    }
};

class ComisionPersoanaInactivaEconomic : public PercepeComision { // Clasa derivata ComisionStandard
public:
    ComisionPersoanaInactivaEconomic(double comision = 0.003) : PercepeComision(comision) {}
    double calculeazaComision(double suma) { // Metoda ce calculeaza comisionul aplicat pe o suma (0.3%)
        return suma * comision; // Comision destinat persoanelor inactive economic (i.e persoane < 18 ani, persoana aflate la pensie)
    }
};

class Cont { // Clasa de baza Cont
protected:
    std::string iban; // IBAN-ul contului
    Client detinator; // Clientul ce detine contul
    double sold; // Soldul contului
    PercepeComision *comision;
    bool esteBlocat; // Data membra folosita pentru a salva stadiul curent al contului
    Registru<Tranzactie> istoricTranzactii; // Istoricul de tranzactii

    std::string genereazaIban() { // Metoda ce genereaza un IBAN valid matematic
        // Generam un cod de banca fictiv format din 4 litere (ex: BTRL, BCRX)
        std::string codBanca = "";
        for (int i = 0; i < 4; ++i) {
            char litera = 'A' + (rand() % 26);
            codBanca += litera;
        }

        // Generam numarul de cont propriu-zis, format din 16 cifre aleatoare
        std::string numarCont = "";
        for (int i = 0; i < 16; ++i) {
            numarCont += std::to_string(rand() % 10);
        }

        // Pregatim datele pentru a calcula Cifra de Control (Checksum)
        std::string codTara = "RO";
        std::string checksumFals = "00"; // In algoritmul IBAN, initial se folosesc "00"

        // Pentru calcul, codul de tara si checksum-ul se muta la finalul sirului
        std::string sirMod97 = codBanca + numarCont + codTara + checksumFals;

        // Convertim caracterele in format pur numeric
        // Literele primesc valori de la 10 la 35 (A=10, B=11 ... Z=35)
        std::string sirNumeric = "";
        for (size_t i = 0; i < sirMod97.length(); ++i) {
            char caracterCurent = sirMod97[i];
            if (caracterCurent >= 'A' && caracterCurent <= 'Z') {
                int valoareNumerica = caracterCurent - 'A' + 10;
                sirNumeric += std::to_string(valoareNumerica);
            } else if (caracterCurent >= '0' && caracterCurent <= '9') {
                sirNumeric += caracterCurent; // Cifrele raman la fel
            }
        }

        // Aplicam algoritmul Modulo 97 (impartirea cu rest la 97)
        // Deoarece numarul rezultat are 26 de cifre si depaseste limita oricarui tip de date int/long,
        // calculam restul parcurgand string-ul cifra cu cifra
        int rest = 0;
        for (size_t i = 0; i < sirNumeric.length(); ++i) {
            int cifra = sirNumeric[i] - '0';
            rest = (rest * 10 + cifra) % 97;
        }

        // Calculam Cifra de Control finala conform formulei IBANului
        int checksumCurent = 98 - rest;

        // Ne asiguram ca checksum-ul are mereu 2 caractere (daca e < 10, ii adaugam un '0' in fata)
        std::string cifraDeControl;
        if (checksumCurent < 10) {
            cifraDeControl = "0" + std::to_string(checksumCurent);
        } else {
            cifraDeControl = std::to_string(checksumCurent);
        }

        // Facem concatenare pentru a afisa IBAN-ul in formatul corect
        std::string ibanComplet = codTara + cifraDeControl + codBanca + numarCont;

        return ibanComplet;
    }

public:
    Cont(const Client& detinatorCont, double soldInitial) // Constructor
        : detinator(detinatorCont), sold(soldInitial), esteBlocat(false) {
        iban = genereazaIban();
        switch (detinator.getTipStadiu()) {
            case ANGAJAT:
                comision = new ComisionAngajat();
                break;
            case STUDENT:
                comision = new ComisionStudent();
                break;
            case INACTIV:
                comision = new ComisionPersoanaInactivaEconomic();
                break;
            default:
                comision = new ComisionStandard();
        }
    }

    virtual ~Cont() { // Destructor virtual
        delete comision;
    }

    virtual void depune(double suma) = 0; // Metoda pura pentru depuneri
    virtual void retrage(double suma) = 0; // Metoda pura pentru retrageri
    virtual Cont* cloneaza() const = 0; // Metoda pura pentru clonarea obiectului (fiecare obiect trebuie sa stie cum sa se copieze)
    virtual double estimeazaSoldViitor(int ani) const = 0; // Metoda ce estimeaza soldul din contul respectiv

    const Client& getDetinator() const { return detinator; } // Getter pentru detinator
    bool esteContBlocat() const { return esteBlocat; } // Getter pentru stadiul contului
    double getSold() const { return sold; } // Getter pentru sold

    std::string getIban() const { return iban; } // Getter pentru IBAN

    void blocheazaCont() { esteBlocat = true; } // Metoda ce blocheaza contul
    void deblocheazaCont() { esteBlocat = false; } // Metoda ce deblocheaza contul

    void adaugaTranzactieIstoric(const Tranzactie& t) { // Metoda ce adauga o tranzactie la istoric
        istoricTranzactii.adaugaInregistrare(t); // Folosim metode specifice din clasa Registru<Tranzactie>
    }

    void actualizeazaTranzactieIstoric(const Registru<Tranzactie>& t) { // Metoda ce sterge istoricul de tranzactii si il inlocuieste cu altul
        istoricTranzactii.stergeInregistrari(); // Stergem tranzactiile
        for (const auto& tranzactie : t.getInregistrari()) // Parcurgem vectorul nou de tranzactii
            istoricTranzactii.adaugaInregistrare(tranzactie);
    }

    bool verificaAlertaFrauda() { // Metoda ce verifica daca unui cont i s-au facut tranzactii frauduloase
        const auto& tranzactii = istoricTranzactii.getInregistrari(); // Determinam tranzactiile

        if (tranzactii.size() >= 5) { // Luam ultimile cele mai recente tranzactii
            auto it = tranzactii.rbegin();

            std::string ultimaDestinatie = it->getDestinatie(); // Determinam contul unde s-au trimis suma
            std::string ultimaData = it->getData(); // Determinam data
            std::string ultimaOra = it->getOra();

            char anUltim[5] = {ultimaData[0], ultimaData[1], ultimaData[2], ultimaData[3], '\0'};
            char lunaUltima[3] = {ultimaData[5], ultimaData[6], '\0'};
            char ziUltima[3] = {ultimaData[8], ultimaData[9], '\0'};
            char oraUltima[3] = {ultimaOra[0], ultimaOra[1], '\0'};
            char minutUltim[3] = {ultimaOra[3], ultimaOra[4], '\0'};

            int contorTranzactiiSimilare = 0; // Contor pentru a numara cate astfel de tranzactii au avut loc

            for (int i = 0; i < 5; ++i) {
                std::string destinatieCurenta = it->getDestinatie();
                std::string dataCurenta = it->getData();
                std::string oraCurenta = it->getOra();

                char anCurent[5] = {dataCurenta[0], dataCurenta[1], dataCurenta[2], dataCurenta[3], '\0'};
                char lunaCurenta[3] = {dataCurenta[5], dataCurenta[6], '\0'};
                char ziCurenta[3] = {dataCurenta[8], dataCurenta[9], '\0'};
                char oraCurentaArr[3] = {oraCurenta[0], oraCurenta[1], '\0'};
                char minutCurent[3] = {oraCurenta[3], oraCurenta[4], '\0'};

                bool acelasiMinut = true; // Facem verificarile necesare (aceasi data, aceasi zi, acelasi minut)

                for (int j = 0; j < 4; j++) {
                    if (anUltim[j] != anCurent[j]) {
                        acelasiMinut = false;
                    }
                }

                for (int j = 0; j < 2; j++) {
                    if (lunaUltima[j] != lunaCurenta[j]) {
                        acelasiMinut = false;
                    }
                    if (ziUltima[j] != ziCurenta[j]) {
                        acelasiMinut = false;
                    }
                    if (oraUltima[j] != oraCurentaArr[j]) {
                        acelasiMinut = false;
                    }
                    if (minutUltim[j] != minutCurent[j]) {
                        acelasiMinut = false;
                    }
                }

                if (destinatieCurenta == ultimaDestinatie && acelasiMinut == true) { // Daca se constata ca suma a mers la acelasi cont,
                    contorTranzactiiSimilare++;                                      // incrementam contorul
                }

                ++it;
            }

            if (contorTranzactiiSimilare == 5) { // Daca in ultimul minut s-au efectuat 5 tranzactii diferite catre acelasi cont
                return true;                     // returnam true, ceea ce inseamna ca au avut loc tranzactii frauduloase
            }
        }
        return false;
    }

    friend std::ostream& operator<<(std::ostream& out, const Cont& c) { // Overloading pe operator<<
        out << "Date Cont:\n"
            << "IBAN: " << c.iban << "\n"
            << "Sold: " << c.sold << " RON\n"
            << "Status Blocare: " << (c.esteBlocat ? "DA" : "NU") << "\n"
            << c.detinator;
        return out;
    }
};

class ContEconomii : public Cont { // Clasa mostenita din clasa de baza Cont
private:
    double rataDobanda;
public:
    ContEconomii(const Client& detinator, double sold, double rata) // Constructor
        : Cont(detinator, sold), rataDobanda(rata) {}

    void depune(double suma) override { // Metoda depunere specifica clasei
        if (esteBlocat) throw ContBlocatException(); // Daca contul este blocat in urma unei fraude, aruncam o exceptie
        if (suma > 0) {
            suma += comision->calculeazaComision(suma);
            sold += suma; // Adaugam suma in soldul contului
        }
    }

    void retrage(double suma) override { // Metoda retragere specifica clasei
        if (esteBlocat) throw ContBlocatException(); // Daca contul este blocat in urma unei fraude, aruncam o exceptie
        if (sold < suma) throw FonduriInsuficienteException(); // Daca soldul este negativ, nu putem extrage
        if (suma < 0) return; // Daca suma depusa este negativa, nu se intampla retragerea
        suma += comision->getComision();
        sold -= suma;
    }

    void schimbaRataDobanda(double rata) { // Metoda ce schimba rata dobanzii
        if (rata > 0) {
            rataDobanda = rata;
        }
    }

    Cont* cloneaza() const override { // Metoda ce cloneaza obiectul (fiecare obiect trebuie sa stie cum sa se copieze)
        return new ContEconomii(*this); // Returnam obiectul
    }

    double estimeazaSoldViitor(int ani) const override { // Metoda ce determina soldul peste un numar de ani
        return sold * std::pow((1.0 + rataDobanda), ani); // A = (1 + r)^n (formula pentru dobanda compusa)
    }
};

class ContCredit : public Cont { // Clasa mostenita din clasa de baza Cont
private:
    double limitaCredit;
public:
    ContCredit(const Client& detinator, double sold, double limita) // Constructor
        : Cont(detinator, sold), limitaCredit(limita) {}

    void depune(double suma) override { // Metoda depunere specifica clasei
        if (esteBlocat) throw ContBlocatException(); // Daca contul este blocat in urma unei fraude, aruncam o exceptie
        if (suma > 0) {
            suma += comision->calculeazaComision(suma);
            sold += suma; // Adaugam suma in soldul contului
        }
    }

    void retrage(double suma) override {  // Metoda retragere specifica clasei
        if (suma < 0) return;
        suma += comision->getComision();
        if (esteBlocat) throw ContBlocatException(); // Daca contul este blocat in urma unei fraude, aruncam o exceptie
        if (sold + limitaCredit < suma) throw FonduriInsuficienteException();
        // Daca suma este mai mare decat suma curenta din sold si depaseste de asemenea si limita de credit, aruncam o exceptie
        sold -= suma;
    }

    Cont* cloneaza() const override { // Metoda ce cloneaza obiectul (fiecare obiect trebuie sa stie cum sa se copieze)
        return new ContCredit(*this); // Returnam obiectul
    }

    double estimeazaSoldViitor(int ani) const override { // Metoda ce estimeaza suma din sold peste un numar de ani
        double dobandaPenalizare = 0.15; // Conturile de credit nu sunt conturi de economii, suma ramane aceasi
        // Eventaul, daca creditul este negativ, se aplica sactiuni
        if (sold < 0) {
            return sold * std::pow((1.0 + dobandaPenalizare), ani); // A = (1 + r)^n (formula pentru dobanda compusa)
        }
        return sold;
    }

    void actualizeazaLimitaCredit(double limitaNoua) { // Metoda ce actualizeaza linia de credit
        if (limitaNoua > 0)
            limitaCredit = limitaNoua;
    }
};

enum TipCont { // Enumeratie folosita la crerea conturilor
    CONT_ECONOMIE,
    CONT_CREDIT
};

class ContFactory { // Clasa ContFactory  (design pattern, Factory)
public:
    Cont* creeazaCont(TipCont tip, const Client& detinator, double sold) {
        // Folosim metoda creeazaCont pentru a genera un tip anume de cont
        if (!detinator.esteCNPvalid()) { // Un cont nou poate fi creat cu un CNP invalid
            throw std::invalid_argument("Tip de client nesuportat (CNP invalid)");
        }
        if (tip == CONT_ECONOMIE) {
            return new ContEconomii(detinator, sold, 0.04);
        }
        else if (tip == CONT_CREDIT) {
            return new ContCredit(detinator, sold, 3000.0);
        }
        throw std::invalid_argument("Tip de cont nesuportat"); // Aruncam o exceptie daca tipul de cont nu este cunoscut
    }
};

class SistemBancar { // Clasa SitemBancar (design pattern, Singleton)
private:
    static SistemBancar* instanta; // Unica instanta a sistemului bancar
    std::vector<Cont*> conturi; // Vector ce contine toate conturile deschise
    Registru<std::string> jurnalSistem; // Jurnalul sistemului

    SistemBancar() {} // Constructor privat
    SistemBancar(const SistemBancar&) = delete; // Constructor de copiere sters
    SistemBancar& operator=(const SistemBancar&) = delete; // Operator= sters

public:
    static SistemBancar* getInstanta() { // Functie ce returneaza unica instanta a sistemului bancar
        if (instanta == nullptr) {
            instanta = new SistemBancar();
        }
        return instanta;
    }

    ~SistemBancar() { // Destructor pentru vector
        for (Cont* cont : conturi) {
            delete cont;
        }
        conturi.clear();
    }

    void adaugaCont(Cont* cont) { // Metoda ce adauga un cont in sistemul bancar
        if (!cont->getDetinator().esteCNPvalid()) { // Daca CNP-ul nu este valid, nu adaugam contul
            jurnalSistem.adaugaInregistrare("Nu s-a putut efectua adaugarea contului (client cu date invalide)"); // Scriem informatia in jurnalul de sistem
            return;
        }
        conturi.push_back(cont); // Daca CNP-ul este valid, adaugam contul
        jurnalSistem.adaugaInregistrare("A fost adaugat un cont in sistem: " + cont->getIban()); // Scriem informatia ...
    }

    void eliminaCont(Cont* cont) { // Metoda pentru eliminarea unui cont
        for (auto it = conturi.begin(); it != conturi.end(); it++) {
            const Cont* contAux = *it;
            if (contAux == cont && cont->getSold() == 0) {
                jurnalSistem.adaugaInregistrare("Contul cu IBAN-ul " + cont->getIban() + " a fost sters");
                delete cont;
                it = conturi.erase(it);
                return;
            }
        }
        jurnalSistem.adaugaInregistrare("Nu s-a efectuat stergerea unui cont (nu se afla in baza de date)");
    }

    void eliminaContDupaCNP(unsigned long long cnp, unsigned long long numarConturi = 1) { // Metoda ce elimina conturi dupa CNP
        unsigned long long copie = numarConturi;
        for (auto it = conturi.begin(); it != conturi.end() && numarConturi > 0; ) {
            const Cont* cont = *it;
            if (cont->getDetinator().getCNP() == cnp && cont->getSold() == 0) { // Daca am gasit un cont cu acealasi CNP si sold 0 il stergem
                delete cont;
                it = conturi.erase(it);
                numarConturi--;
            }
            else {
                it++;
            }
        }
        if (numarConturi == 0) { // Actualizam jurnalul
            jurnalSistem.adaugaInregistrare("Toate conturile cu CNP-ul " + std::to_string(cnp) + " au fost sterse");
        }
        else if (copie - numarConturi > 0) {
            jurnalSistem.adaugaInregistrare("Din cauza soldului necorespunzator, nu toate conturile au putut fi sterse");
        }
        else if (numarConturi == copie) {
            jurnalSistem.adaugaInregistrare("Nu s-au gasit conturi asociate cu CNP-ul respectiv");
        }
    }

    void transferaFonduri(const std::string& dinIban, const std::string& catreIban, double suma) { // Metoda ce transfera fonduri de la un cont la altul
        Cont* contSursa = nullptr;
        Cont* contDestinatie = nullptr;

        for (Cont* c : conturi) { // Verificam daca gasim conturi cu IBAN-urile respective
            if (c->getIban() == dinIban) contSursa = c;
            if (c->getIban() == catreIban) contDestinatie = c;
        }

        if (contSursa == nullptr && contDestinatie == nullptr) { // Actualizam jurnaulul daca nu a reusit transferul
            jurnalSistem.adaugaInregistrare("Transfer esuat: Ambele IBAN-uri sunt invalide (" + dinIban + " si " + catreIban + ")");
            return;
        }

        if (contSursa == nullptr) { // Actualizam jurnaulul daca nu a reusit transferul
            jurnalSistem.adaugaInregistrare("Transfer esuat: IBAN sursa invalid (" + dinIban + ")");
            return;
        }

        if (contDestinatie == nullptr) { // Actualizam jurnaulul daca nu a reusit transferul
            jurnalSistem.adaugaInregistrare("Transfer esuat: IBAN destinatie invalid (" + catreIban + ")");
            return;
        }

        // Retragem suma din contul sursa
        contSursa->retrage(suma);

        try {
            // Adaugam suma in contul destinatie
            contDestinatie->depune(suma);
            contSursa->adaugaTranzactieIstoric(Tranzactie(suma, catreIban));
            jurnalSistem.adaugaInregistrare("Transfer finalizat: " + std::to_string(suma) + " RON");
        }
        catch (...) { // Prindem daca o eroare are loc
            contSursa->depune(suma);
            jurnalSistem.adaugaInregistrare("Eroare depunere. Am restabilit soldul pentru: " + dinIban);
            throw; // Aruncam mai departe eroarea
        }
    }

    void actualizeazaToateLimiteleCredit(double nouaLimita) { // Metoda ce actualizeaza limita de credit pentru conutrile de credit
        for (Cont* cont : conturi) {
            ContCredit* contCred = dynamic_cast<ContCredit*>(cont); // Facem dynamic_cast pentru a verifica daca contul este unul de credit
            if (contCred != nullptr) {
                contCred->actualizeazaLimitaCredit(nouaLimita); // Actualizam linia de credit
                jurnalSistem.adaugaInregistrare("Limita de credit actualizata pentru: " + contCred->getIban());
            }
        }
    }

    void ruleazaDetectieFrauda() { // Metoda ce detecteaza eventualele fraude
        for (Cont* cont : conturi) {
            if (cont->verificaAlertaFrauda()) {
                cont->blocheazaCont();
                jurnalSistem.adaugaInregistrare("Alerta: Cont blocat pentru frauda -> " + cont->getIban());
            }
        }
    }

    void printeazaJurnal() const { // Metoda ce afiseaza jurnalul de sistem al bancii
        std::cout << "\nJURNAL SISTEM:\n";
        jurnalSistem.afiseazaTot();
        std::cout << "\n\n";
    }

    Cont* extrageContDupaIndex(unsigned long long index) { // Metoda ce extrage un cont dupa indexul sau
        if (index < conturi.size()) return conturi[index];
        return nullptr;
    }

    template<typename T, typename U>
    std::vector<T*> filtreazaConturi(U criteriu) { // Metoda ce returneaza conturi care indeplinesc o anumita conditie
        std::vector<T*> conturiRezultat;
        for (Cont* cont : conturi) { // Parcurgem conturile
            T* contAuxiliar = dynamic_cast<T*>(cont);
            try {
                if (contAuxiliar != nullptr && criteriu(contAuxiliar)) { // Verificam daca contul indeplineste criteriile de filtrare
                    conturiRezultat.push_back(contAuxiliar);
                }
            }
            catch (...) { // In cazul in care U nu este o functie sau expresie lambda, prindem eroarea
                std::cout << "Eroare fatala\n";
                jurnalSistem.adaugaInregistrare("Eroare fatala in metoda \"filtrareConturi\"");
                return std::vector<T*>();
            }
        }
        return conturiRezultat;
    }
};

SistemBancar* SistemBancar::instanta = nullptr; // Instantierea initiala a sistemlui bancar

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
        Client("Mihai Popescu", 1900515410010ULL, ANGAJAT), // Valid
        Client("Elena Ionescu", 2920820410021ULL, INACTIV), // Valid
        Client("Fraudulescu Ion", 5010203123456ULL, STUDENT), // Invalid
        Client("Andrei Vasile", 5051210011230ULL, INACTIV), // Valid
        Client("Maria Radu", 6080417124563ULL, STUDENT), // Valid
        Client("Gigel Nesuportat", 6020304234567ULL, ALT_STADIU), // Invalid
        Client("Ion Georgescu", 1851125229994ULL, ANGAJAT), // Valid
        Client("Ana Dumitru", 2700101351111ULL, INACTIV), // Valid
        Client("Vasile Lupu", 1990909415558ULL, ALT_STADIU), // Valid
        Client("Cristina Stan", 2880228083338ULL, STUDENT) // Valid
    };

    std::vector<TipCont> tipuriCont = { // Adaguam tipuri de conturi pentru fiecare client
        CONT_ECONOMIE, CONT_CREDIT, CONT_ECONOMIE, CONT_CREDIT, CONT_ECONOMIE,
        CONT_CREDIT, CONT_ECONOMIE, CONT_CREDIT, CONT_ECONOMIE, CONT_ECONOMIE
    };

    std::vector<double> solduri = { // Adaumga solduri pentu fiecare client
        5000.0, 1000.0, 2000.0, 1500.0, 8000.0,
        3000.0, 4500.0, 2500.0, 6000.0, 7000.0
    };

    // Creem conturile
    for (size_t i = 0; i < clienti.size(); ++i) {
        try {
            Cont* contNou = factory.creeazaCont(tipuriCont[i], clienti[i], solduri[i]);
            banca->adaugaCont(contNou);
            std::cout << "Cont deschis pentru: " << (clienti[i].getNume() ? clienti[i].getNume() : "Nume invalud") << "\n";
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
            banca->transferaFonduri(ibanEco, ibanCred, 60.0);
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

    auto vec = banca->filtreazaConturi<ContEconomii>([](ContEconomii* c) { return c->getSold() > 5000.0; }); // Apelam metoda filtreazaConturi
    for (const auto& elem : vec) {
        std::cout << *elem << "\n\n";
    }

    delete banca;

    return 0;
}
