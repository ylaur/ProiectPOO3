#include "SistemBancar.h"
#include <iostream>

SistemBancar* SistemBancar::instanta = nullptr; // Instantierea initiala a sistemlui bancar

SistemBancar::SistemBancar() {}

SistemBancar* SistemBancar::getInstanta() {
    if (instanta == nullptr) {
        instanta = new SistemBancar();
    }
    return instanta;
}

SistemBancar::~SistemBancar() {
    for (Cont* cont : conturi) {
        delete cont;
    }
    conturi.clear();
}

void SistemBancar::adaugaCont(Cont* cont) {
    if (!cont->getDetinator().esteCNPvalid()) { // Daca CNP-ul nu este valid, nu adaugam contul
        jurnalSistem.adaugaInregistrare("Nu s-a putut efectua adaugarea contului (client cu date invalide)"); // Scriem informatia in jurnalul de sistem
        return;
    }
    conturi.push_back(cont); // Daca CNP-ul este valid, adaugam contul
    jurnalSistem.adaugaInregistrare("A fost adaugat un cont in sistem: " + cont->getIban()); // Scriem informatia ...
}

void SistemBancar::eliminaCont(Cont* cont) {
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

void SistemBancar::eliminaContDupaCNP(unsigned long long cnp, unsigned long long numarConturi) {
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

void SistemBancar::transferaFonduri(const std::string& dinIban, const std::string& catreIban, double suma) {
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

void SistemBancar::actualizeazaToateLimiteleCredit(double nouaLimita) {
    for (Cont* cont : conturi) {
        ContCredit* contCred = dynamic_cast<ContCredit*>(cont); // Facem dynamic_cast pentru a verifica daca contul este unul de credit
        if (contCred != nullptr) {
            contCred->actualizeazaLimitaCredit(nouaLimita); // Actualizam linia de credit
            jurnalSistem.adaugaInregistrare("Limita de credit actualizata pentru: " + contCred->getIban());
        }
    }
}

void SistemBancar::ruleazaDetectieFrauda() {
    for (Cont* cont : conturi) {
        if (cont->verificaAlertaFrauda()) {
            cont->blocheazaCont();
            jurnalSistem.adaugaInregistrare("Alerta: Cont blocat pentru frauda -> " + cont->getIban());
        }
    }
}

void SistemBancar::printeazaJurnal() const {
    std::cout << "\nJURNAL SISTEM:\n";
    jurnalSistem.afiseazaTot();
    std::cout << "\n\n";
}

Cont* SistemBancar::extrageContDupaIndex(unsigned long long index) {
    if (index < conturi.size()) return conturi[index];
    return nullptr;
}
