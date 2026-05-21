#pragma once
#include <exception>
#include <new>

class FonduriInsuficienteException : public std::exception { // Clasa de exceptii
public:
    const char* what() const noexcept override; // Schimbam eroarea standard cu un mesaj mai simplu
};

class ContBlocatException : public std::exception { // Clasa de exceptii
public:
    const char* what() const noexcept override; // Schimbam eroarea standard cu un mesaj mai simplu
};

class EroareAlocareMemorie : public std::bad_alloc { // Clasa de exceptii
public:
    const char* what() const noexcept override; // Schimbam eroarea standard cu un mesaj mai simplu
};
