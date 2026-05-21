#include "Exceptii.h"

const char* FonduriInsuficienteException::what() const noexcept {
    return "Eroare fonduri: Fonduri insuficiente pentru a finaliza tranzactia";
}

const char* ContBlocatException::what() const noexcept {
    return "Operatiune refuzata: Contul este blocat";
}

const char* EroareAlocareMemorie::what() const noexcept {
    return "Eroare alocare: Nu s-a putut aloca memoria necesara";
}
