// Benachrichtiger.h
#pragma once
#include <functional>
#include <vector>

class Benachrichtiger {
private:
    std::vector<std::function<void()>> listener;
    // AufgabenVerteiler dispatcher; // in cpp

public:
    virtual ~Benachrichtiger() = default;

    // Hier konnen sich andere Klassen registrieren
    void registriereListener(std::function<void()> callback);

protected:
    // Diese Methode rufst du im abgeleiteten Objekt auf, wenn sich etwas andert
    void benachrichtigeListenerAsync();
};
