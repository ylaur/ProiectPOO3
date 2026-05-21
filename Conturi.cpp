#include "Conturi.h"
#include "Exceptii.h"
#include <cmath>
#include <stdexcept>

// ====== IMPLEMENTARE CONT ======

Cont::Cont(const Client& detinatorCont, double soldInitial)
    : detinator(detinatorCont), sold(soldInitial), esteBlocat(false) {
    iban = genereazaIban();
}

Cont::~Cont() {}

std::string Cont::genereazaIban() {
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

const Client& Cont::getDetinator() const { return detinator; }
bool Cont::esteContBlocat() const { return esteBlocat; }
double Cont::getSold() const { return sold; }
std::string Cont::getIban() const { return iban; }

void Cont::blocheazaCont() { esteBlocat = true; }
void Cont::deblocheazaCont() { esteBlocat = false; }

void Cont::adaugaTranzactieIstoric(const Tranzactie& t) {
    istoricTranzactii.adaugaInregistrare(t); // Folosim metode specifice din clasa Registru<Tranzactie>
}

void Cont::actualizeazaTranzactieIstoric(const Registru<Tranzactie>& t) {
    istoricTranzactii.stergeInregistrari(); // Stergem tranzactiile
    for (const auto& tranzactie : t.getInregistrari()) // Parcurgem vectorul nou de tranzactii
        istoricTranzactii.adaugaInregistrare(tranzactie);
}

bool Cont::verificaAlertaFrauda() {
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

std::ostream& operator<<(std::ostream& out, const Cont& c) {
    out << "Date Cont:\n"
        << "IBAN: " << c.iban << "\n"
        << "Sold: " << c.sold << " RON\n"
        << "Status Blocare: " << (c.esteBlocat ? "DA" : "NU") << "\n"
        << c.detinator;
    return out;
}

// ====== IMPLEMENTARE CONT ECONOMII ======

ContEconomii::ContEconomii(const Client& detinator, double sold, double rata)
    : Cont(detinator, sold), rataDobanda(rata) {}

void ContEconomii::depune(double suma) {
    if (esteBlocat) throw ContBlocatException(); // Daca contul este blocat in urma unei fraude, aruncam o exceptie
    if (suma > 0)
        sold += suma; // Adaugam suma in sold
}

void ContEconomii::retrage(double suma) {
    if (esteBlocat) throw ContBlocatException(); // Daca contul este blocat in urma unei fraude, aruncam o exceptie
    if (sold < suma) throw FonduriInsuficienteException(); // Daca soldul este negativ, nu putem extrage
    if (suma < 0) return; // Daca suma depusa este negativa, nu se intampla retragerea
    sold -= suma;
}

void ContEconomii::schimbaRataDobanda(double rata) {
    if (rata > 0) {
        rataDobanda = rata;
    }
}

Cont* ContEconomii::cloneaza() const {
    return new ContEconomii(*this); // Returnam obiectul
}

double ContEconomii::estimeazaSoldViitor(int ani) const {
    return sold * std::pow((1.0 + rataDobanda), ani); // A = (1 + r)^n (formula pentru dobanda compusa)
}

// ====== IMPLEMENTARE CONT CREDIT ======

ContCredit::ContCredit(const Client& detinator, double sold, double limita)
    : Cont(detinator, sold), limitaCredit(limita) {}

void ContCredit::depune(double suma) {
    if (esteBlocat) throw ContBlocatException(); // Daca contul este blocat in urma unei fraude, aruncam o exceptie
    sold += suma;
}

void ContCredit::retrage(double suma) {
    if (esteBlocat) throw ContBlocatException(); // Daca contul este blocat in urma unei fraude, aruncam o exceptie
    if (sold + limitaCredit < suma) throw FonduriInsuficienteException();
    // Daca suma este mai mare decat suma curenta din sold si depaseste de asemenea si limita de credit, aruncam o exceptie
    sold -= suma;
}

Cont* ContCredit::cloneaza() const {
    return new ContCredit(*this); // Returnam obiectul
}

double ContCredit::estimeazaSoldViitor(int ani) const {
    double dobandaPenalizare = 0.15; // Conturile de credit nu sunt conturi de economii, suma ramane aceasi
    // Eventaul, daca creditul este negativ, se aplica sactiuni
    if (sold < 0) {
        return sold * std::pow((1.0 + dobandaPenalizare), ani); // A = (1 + r)^n (formula pentru dobanda compusa)
    }
    return sold;
}

void ContCredit::actualizeazaLimitaCredit(double limitaNoua) {
    if (limitaNoua > 0)
        limitaCredit = limitaNoua;
}

// ====== IMPLEMENTARE FACTORY ======

Cont* ContFactory::creeazaCont(TipCont tip, const Client& detinator, double sold) {
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
