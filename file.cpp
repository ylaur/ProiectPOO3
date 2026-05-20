#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <ctime>
#include <cstring>
#include <cmath>

class FonduriInsuficienteException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Fonduri insuficiente pentru a finaliza tranzactia";
    }
};

class ContBlocatException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Operatiune refuzata: Contul este blocat";
    }
};

template <typename T>
class Registru {
private:
    std::vector<T> inregistrari;
public:
    void adaugaInregistrare(const T& inregistrare) {
        inregistrari.push_back(inregistrare);
    }
    const std::vector<T>& getInregistrari() const {
        return inregistrari;
    }
    void afiseazaTot() const {
        for (const auto& inregistrare : inregistrari) {
            std::cout << inregistrare << "\n";
        }
    }
};

class Tranzactie {
private:
    double suma;
    std::string destinatieIBAN;
    std::string data;
    std::string ora;
public:
    Tranzactie(double valoare, const std::string& destinatie) : suma(valoare), destinatieIBAN(destinatie) {
        time_t timp = time(nullptr);
        tm* timpLocal = localtime(&timp);

        int an = timpLocal->tm_year + 1900;
        int luna = timpLocal->tm_mon + 1;
        int zi = timpLocal->tm_mday;
        int oraCurenta = timpLocal->tm_hour;
        int minut = timpLocal->tm_min;
        int secunda = timpLocal->tm_sec;

        data = std::to_string(an) + "/";

        if (luna < 10) {
            data += "0";
        }
        data += std::to_string(luna) + "/";

        if (zi < 10) {
            data += "0";
        }
        data += std::to_string(zi);

        if (oraCurenta < 10) {
            ora = "0";
        } else {
            ora = "";
        }
        ora += std::to_string(oraCurenta) + ":";

        if (minut < 10) {
            ora += "0";
        }
        ora += std::to_string(minut) + ":";

        if (secunda < 10) {
            ora += "0";
        }
        ora += std::to_string(secunda);
    }

    std::string getDestinatie() const { return destinatieIBAN; }
    double getSuma() const { return suma; }
    std::string getData() const { return data; }
    std::string getOra() const { return ora; }

    friend std::ostream& operator<<(std::ostream& out, const Tranzactie& t) {
        out << "Catre: " << t.getDestinatie()
            << " | Suma: " << t.getSuma()
            << " | Data: " << t.data << " " << t.ora;
        return out;
    }
};

class Client {
private:
    char* nume;
    unsigned long long CNP;

    bool validareCNP(unsigned long long cnp) {
        unsigned long long copieCNP = cnp / 10;
        if (cnp < 1000000000000ULL || cnp >= 10000000000000ULL) return false;
        unsigned short int CifraControl = cnp % 10;
        cnp /= 10;
        if (cnp % 1000 == 0) return false;
        cnp /= 1000;
        unsigned short int JJ = cnp % 100;
        if (JJ != 51 && JJ != 52 && JJ != 70 && (JJ < 1 || JJ > 48)) return false;
        cnp /= 100;
        if (cnp % 100 < 1 || cnp % 100 > 31) return false;
        cnp /= 100;
        unsigned short int LL = cnp % 100;
        if (LL < 1 || LL > 12) return false;
        cnp /= 100;
        unsigned short int AA = cnp % 100;
        cnp /= 100;
        unsigned short int S = cnp % 100;
        if ((JJ == 47 || JJ == 48) && LL >= 8 && AA >= 79) return false;
        if (S != 1 && S != 2 && S != 5 && S != 6) return false;
        if ((S == 5 || S == 6) && AA > 26) return false;
        unsigned long long validC = 0;
        unsigned long long CONST = 279146358279ULL;
        while (copieCNP) {
            validC += (copieCNP % 10) * (CONST % 10);
            CONST /= 10;
            copieCNP /= 10;
        }
        validC %= 11;
        if (validC < 10 && CifraControl != validC) return false;
        if (validC == 10 && CifraControl != validC - 9) return false;
        return true;
    }

public:
    Client(const char* numeClient, unsigned long long cnp) {
        if (numeClient != nullptr) {
            nume = new char[strlen(numeClient) + 1];
            strcpy(nume, numeClient);
        } else {
            nume = nullptr;
        }
        CNP = (validareCNP(cnp) ? cnp : 0);
    }

    ~Client() {
        if (nume != nullptr) {
            delete[] nume;
        }
    }

    Client(const Client& c) {
        if (c.nume != nullptr) {
            nume = new char[strlen(c.nume) + 1];
            strcpy(nume, c.nume);
        } else {
            nume = nullptr;
        }
        CNP = c.CNP;
    }

    Client& operator=(Client c) {
        swap(*this, c);
        return *this;
    }

    friend void swap(Client& unu, Client& doi) {
        std::swap(unu.nume, doi.nume);
        std::swap(unu.CNP, doi.CNP);
    }

    friend std::ostream& operator<<(std::ostream& out, const Client& c) {
        out << "Nume Client: " << (c.nume ? c.nume : "Fara nume") << "\n"
            << "CNP: " << c.CNP;
        return out;
    }
};

class Cont {
protected:
    std::string iban;
    Client detinator;
    double sold;
    bool esteBlocat;
    Registru<Tranzactie> istoricTranzactii;

    std::string genereazaIban() {
        std::string codBanca = "";
        for (int i = 0; i < 4; ++i) {
            char litera = 'A' + (rand() % 26);
            codBanca += litera;
        }

        std::string numarCont = "";
        for (int i = 0; i < 16; ++i) {
            numarCont += std::to_string(rand() % 10);
        }

        std::string codTara = "RO";
        std::string checksumFals = "00";

        std::string sirMod97 = codBanca + numarCont + codTara + checksumFals;

        std::string sirNumeric = "";
        for (size_t i = 0; i < sirMod97.length(); ++i) {
            char caracterCurent = sirMod97[i];
            if (caracterCurent >= 'A' && caracterCurent <= 'Z') {
                int valoareNumerica = caracterCurent - 'A' + 10;
                sirNumeric += std::to_string(valoareNumerica);
            } else if (caracterCurent >= '0' && caracterCurent <= '9') {
                sirNumeric += caracterCurent;
            }
        }

        int rest = 0;
        for (size_t i = 0; i < sirNumeric.length(); ++i) {
            int cifra = sirNumeric[i] - '0';
            rest = (rest * 10 + cifra) % 97;
        }

        int checksumCurent = 98 - rest;

        std::string cifraDeControl;
        if (checksumCurent < 10) {
            cifraDeControl = "0" + std::to_string(checksumCurent);
        } else {
            cifraDeControl = std::to_string(checksumCurent);
        }

        std::string ibanComplet = codTara + cifraDeControl + codBanca + numarCont;

        return ibanComplet;
    }

public:
    Cont(const Client& detinatorCont, double soldInitial)
        : detinator(detinatorCont), sold(soldInitial), esteBlocat(false) {
        iban = genereazaIban();
    }

    virtual ~Cont() {}

    virtual void depune(double suma) = 0;
    virtual void retrage(double suma) = 0;
    virtual Cont* cloneaza() const = 0;
    virtual double estimeazaSoldViitor(int ani) const = 0;
    const Client& getDetinator() { return detinator; }

    std::string getIban() const { return iban; }
    void blocheazaCont() { esteBlocat = true; }

    void adaugaTranzactieIstoric(const Tranzactie& t) {
        istoricTranzactii.adaugaInregistrare(t);
    }

    bool verificaAlertaFrauda() {
        const auto& tranzactii = istoricTranzactii.getInregistrari();

        if (tranzactii.size() >= 5) {
            auto it = tranzactii.rbegin();

            std::string ultimaDestinatie = it->getDestinatie();
            std::string ultimaData = it->getData();
            std::string ultimaOra = it->getOra();

            char anUltim[5] = {ultimaData[0], ultimaData[1], ultimaData[2], ultimaData[3], '\0'};
            char lunaUltima[3] = {ultimaData[5], ultimaData[6], '\0'};
            char ziUltima[3] = {ultimaData[8], ultimaData[9], '\0'};
            char oraUltima[3] = {ultimaOra[0], ultimaOra[1], '\0'};
            char minutUltim[3] = {ultimaOra[3], ultimaOra[4], '\0'};

            int contorTranzactiiSimilare = 0;

            for (int i = 0; i < 5; ++i) {
                std::string destinatieCurenta = it->getDestinatie();
                std::string dataCurenta = it->getData();
                std::string oraCurenta = it->getOra();

                char anCurent[5] = {dataCurenta[0], dataCurenta[1], dataCurenta[2], dataCurenta[3], '\0'};
                char lunaCurenta[3] = {dataCurenta[5], dataCurenta[6], '\0'};
                char ziCurenta[3] = {dataCurenta[8], dataCurenta[9], '\0'};
                char oraCurentaArr[3] = {oraCurenta[0], oraCurenta[1], '\0'};
                char minutCurent[3] = {oraCurenta[3], oraCurenta[4], '\0'};

                bool acelasiMinut = true;

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

                if (destinatieCurenta == ultimaDestinatie && acelasiMinut == true) {
                    contorTranzactiiSimilare++;
                }

                ++it;
            }

            if (contorTranzactiiSimilare == 5) {
                return true;
            }
        }
        return false;
    }

    friend std::ostream& operator<<(std::ostream& out, const Cont& c) {
        out << "Date Cont:\n"
            << "IBAN: " << c.iban << "\n"
            << "Sold: " << c.sold << " RON\n"
            << "Status Blocare: " << (c.esteBlocat ? "DA" : "NU") << "\n"
            << c.detinator;
        return out;
    }
};

class ContEconomii : public Cont {
private:
    double rataDobanda;
public:
    ContEconomii(const Client& detinator, double sold, double rata)
        : Cont(detinator, sold), rataDobanda(rata) {}

    void depune(double suma) override {
        if (esteBlocat) throw ContBlocatException();
        if (suma > 0)
            sold += suma;
    }

    void retrage(double suma) override {
        if (esteBlocat) throw ContBlocatException();
        if (sold < suma) throw FonduriInsuficienteException();
        if (suma < 0) return;
        sold -= suma;
    }

    Cont* cloneaza() const override {
        return new ContEconomii(*this);
    }

    double estimeazaSoldViitor(int ani) const override {
        return sold * std::pow((1.0 + rataDobanda), ani);
    }
};

class ContCredit : public Cont {
private:
    double limitaCredit;
public:
    ContCredit(const Client& detinator, double sold, double limita)
        : Cont(detinator, sold), limitaCredit(limita) {}

    void depune(double suma) override {
        if (esteBlocat) throw ContBlocatException();
        sold += suma;
    }

    void retrage(double suma) override {
        if (esteBlocat) throw ContBlocatException();
        if (sold + limitaCredit < suma) throw FonduriInsuficienteException();
        sold -= suma;
    }

    Cont* cloneaza() const override {
        return new ContCredit(*this);
    }

    double estimeazaSoldViitor(int ani) const override {
        double dobandaPenalizare = 0.15;
        if (sold < 0) {
            return sold * std::pow((1.0 + dobandaPenalizare), ani);
        }
        return sold;
    }

    void actualizeazaLimitaCredit(double limitaNoua) {
        if (limitaNoua > 0)
            limitaCredit = limitaNoua;
    }
};

enum TipCont {
    CONT_ECONOMIE,
    CONT_CREDIT
};

class ContFactory {
public:
    Cont* creeazaCont(TipCont tip, const Client& detinator, double sold) {
        if (tip == CONT_ECONOMIE) {
            return new ContEconomii(detinator, sold, 0.04);
        }
        else if (tip == CONT_CREDIT) {
            return new ContCredit(detinator, sold, 3000.0);
        }
        throw std::invalid_argument("Tip de cont nesuportat");
    }
};

class SistemBancar {
private:
    static SistemBancar* instanta;
    std::vector<Cont*> conturi;
    Registru<std::string> jurnalSistem;

    SistemBancar() {}
    SistemBancar(const SistemBancar&) = delete;
    SistemBancar& operator=(const SistemBancar&) = delete;

public:
    static SistemBancar* getInstanta() {
        if (instanta == nullptr) {
            instanta = new SistemBancar();
        }
        return instanta;
    }

    ~SistemBancar() {
        for (Cont* cont : conturi) {
            delete cont;
        }
        conturi.clear();
    }

    void adaugaCont(Cont* cont) {
        conturi.push_back(cont);
        jurnalSistem.adaugaInregistrare("A fost adaugat un cont in sistem: " + cont->getIban());
    }

    void transferaFonduri(const std::string& dinIban, const std::string& catreIban, double suma) {
        Cont* contSursa = nullptr;
        Cont* contDestinatie = nullptr;

        for (Cont* c : conturi) {
            if (c->getIban() == dinIban) contSursa = c;
            if (c->getIban() == catreIban) contDestinatie = c;
        }

        if (contSursa == nullptr && contDestinatie == nullptr) {
            jurnalSistem.adaugaInregistrare("Transfer esuat: Ambele IBAN-uri sunt invalide (" + dinIban + " si " + catreIban + ")");
            return;
        }

        if (contSursa == nullptr) {
            jurnalSistem.adaugaInregistrare("Transfer esuat: IBAN sursa invalid (" + dinIban + ")");
            return;
        }

        if (contDestinatie == nullptr) {
            jurnalSistem.adaugaInregistrare("Transfer esuat: IBAN destinatie invalid (" + catreIban + ")");
            return;
        }

        contSursa->retrage(suma);

        try {
            contDestinatie->depune(suma);
            contSursa->adaugaTranzactieIstoric(Tranzactie(suma, catreIban));
            jurnalSistem.adaugaInregistrare("Transfer finalizat: " + std::to_string(suma) + " RON");
        } catch (...) {
            contSursa->depune(suma);
            jurnalSistem.adaugaInregistrare("Eroare depunere. Am restabilit soldul pentru: " + dinIban);
            throw;
        }
    }

    void actualizeazaToateLimiteleCredit(double nouaLimita) {
        for (Cont* cont : conturi) {
            ContCredit* contCred = dynamic_cast<ContCredit*>(cont);
            if (contCred != nullptr) {
                contCred->actualizeazaLimitaCredit(nouaLimita);
                jurnalSistem.adaugaInregistrare("Limita de credit actualizata pentru: " + contCred->getIban());
            }
        }
    }

    void ruleazaDetectieFrauda() {
        for (Cont* cont : conturi) {
            if (cont->verificaAlertaFrauda()) {
                cont->blocheazaCont();
                jurnalSistem.adaugaInregistrare("Alerta: Cont blocat pentru frauda -> " + cont->getIban());
            }
        }
    }

    void printeazaJurnal() const {
        std::cout << "\nJURNAL SISTEM:\n";
        jurnalSistem.afiseazaTot();
        std::cout << "\n\n";
    }

    Cont* extrageContDupaIndex(size_t index) {
        if (index < conturi.size()) return conturi[index];
        return nullptr;
    }
};

SistemBancar* SistemBancar::instanta = nullptr;

template <typename T>
void genereazaRaport(const T& entitate, const std::string& titluRaport) {
    std::cout << "RAPORT: " << titluRaport << "\n";
    std::cout << entitate << "\n\n";
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    SistemBancar* banca = SistemBancar::getInstanta();
    ContFactory factory;

    Client client1("Mihai Popescu", 5010203123456ULL);
    Client client2("Elena Ionescu", 6020304234567ULL);

    Cont* contEco = factory.creeazaCont(CONT_ECONOMIE, client1, 5000.0);
    Cont* contCred = factory.creeazaCont(CONT_CREDIT, client2, 1000.0);

    banca->adaugaCont(contEco);
    banca->adaugaCont(contCred);

    banca->actualizeazaToateLimiteleCredit(5000.0);

    std::string ibanEco = contEco->getIban();
    std::string ibanCred = contCred->getIban();

    try {
        banca->transferaFonduri(ibanEco, ibanCred, 250.0);
        banca->transferaFonduri(ibanEco, ibanCred, 300.0);
        banca->transferaFonduri(ibanEco, ibanCred, 100.0);
        banca->transferaFonduri("RO99FALS123", ibanCred, 50.0);
    }
    catch (const std::exception& e) {
        std::cout << "Exceptie: " << e.what() << "\n";
    }

    banca->ruleazaDetectieFrauda();

    try {
        banca->transferaFonduri(ibanEco, ibanCred, 50.0);
    }
    catch (const std::exception& e) {
        std::cout << "\nInterventie blocata: " << e.what() << "\n";
    }

    genereazaRaport(*contEco, "Situatia Cont Sursa");
    genereazaRaport(*contCred, "Situatie Cont Destinatie");

    Tranzactie Test(999.0, "RO00TEST000000");
    genereazaRaport(Test, "Verificare Chitanta Tranzictie");

    banca->printeazaJurnal();

    delete banca;

    return 0;
}
