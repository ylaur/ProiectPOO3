#pragma once
#include <iostream>

class Client { // Clasa Client
private:
    char* nume; // Numele clientului
    unsigned long long CNP; // CNP-ul clientului
    bool areCNPvalid; // Validitatea CNP-ului

    bool validareCNP(unsigned long long cnp); // Metoda privata ce determina daca un CNP este valid

public:
    Client(const char* numeClient, unsigned long long cnp); // Constructor
    ~Client(); // Destructor
    Client(const Client& c); // Costructor de copiere
    Client& operator=(Client c); // Overloading pe operator=

    bool esteCNPvalid() const; // Metoda ce returneaza validitatea CNP-ului
    unsigned long long getCNP() const; // Metoda ce returneaza CNP-ul
    const char* getNume() const; // Metoda ce returneaza numele

    void schimbaNume(const char* numeClient); // Metoda ce schimba numele

    friend void swap(Client& unu, Client& doi); // Metoda ce interschimba 2 clienti
    friend std::ostream& operator<<(std::ostream& out, const Client& c); // Overloading pe operator<<
};
