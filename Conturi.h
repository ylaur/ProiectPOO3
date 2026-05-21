#pragma once
#include <string>
#include <iostream>
#include "Client.h"
#include "Registru.h"
#include "Tranzactie.h"

class Cont { // Clasa de baza Cont
protected:
    std::string iban; // IBAN-ul contului
    Client detinator; // Clientul ce detine contul
    double sold; // Soldul contului
    bool esteBlocat; // Data membra folosita pentru a salva stadiul curent al contului
    Registru<Tranzactie> istoricTranzactii; // Istoricul de tranzactii

    std::string genereazaIban(); // Metoda ce genereaza un IBAN valid matematic

public:
    Cont(const Client& detinatorCont, double soldInitial); // Constructor
    virtual ~Cont(); // Destructor virtual

    virtual void depune(double suma) = 0; // Metoda pura pentru depuneri
    virtual void retrage(double suma) = 0; // Metoda pura pentru retrageri
    virtual Cont* cloneaza() const = 0; // Metoda pura pentru clonarea obiectului
    virtual double estimeazaSoldViitor(int ani) const = 0; // Metoda ce estimeaza soldul

    const Client& getDetinator() const; // Getter pentru detinator
    bool esteContBlocat() const; // Getter pentru stadiul contului
    double getSold() const; // Getter pentru sold
    std::string getIban() const; // Getter pentru IBAN

    void blocheazaCont(); // Metoda ce blocheaza contul
    void deblocheazaCont(); // Metoda ce deblocheaza contul

    void adaugaTranzactieIstoric(const Tranzactie& t); // Metoda ce adauga o tranzactie la istoric
    void actualizeazaTranzactieIstoric(const Registru<Tranzactie>& t); // Metoda ce inlocuieste istoricul de tranzactii

    bool verificaAlertaFrauda(); // Metoda ce verifica tranzactiile frauduloase

    friend std::ostream& operator<<(std::ostream& out, const Cont& c); // Overloading pe operator<<
};

class ContEconomii : public Cont { // Clasa mostenita din clasa de baza Cont
private:
    double rataDobanda;
public:
    ContEconomii(const Client& detinator, double sold, double rata); // Constructor

    void depune(double suma) override; // Metoda depunere specifica clasei
    void retrage(double suma) override; // Metoda retragere specifica clasei
    void schimbaRataDobanda(double rata); // Metoda ce schimba rata dobanzii
    Cont* cloneaza() const override; // Metoda ce cloneaza obiectul
    double estimeazaSoldViitor(int ani) const override; // Metoda ce determina soldul
};

class ContCredit : public Cont { // Clasa mostenita din clasa de baza Cont
private:
    double limitaCredit;
public:
    ContCredit(const Client& detinator, double sold, double limita); // Constructor

    void depune(double suma) override; // Metoda depunere specifica clasei
    void retrage(double suma) override; // Metoda retragere specifica clasei
    Cont* cloneaza() const override; // Metoda ce cloneaza obiectul
    double estimeazaSoldViitor(int ani) const override; // Metoda ce estimeaza suma
    void actualizeazaLimitaCredit(double limitaNoua); // Metoda ce actualizeaza linia de credit
};

enum TipCont { // Enumeratie folosita la crearea conturilor
    CONT_ECONOMIE,
    CONT_CREDIT
};

class ContFactory { // Clasa ContFactory (design pattern, Factory)
public:
    Cont* creeazaCont(TipCont tip, const Client& detinator, double sold);
};
