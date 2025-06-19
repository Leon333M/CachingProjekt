// Benachrichtiger.h
#pragma once
#include <functional>
#include <vector>

class Benachrichtiger {
private:
    std::vector<std::function<void()>> listener;
    std::unordered_map<int, int> idZuIndex; // ID → Index
    int naechsteId = 0;
    // AufgabenVerteiler dispatcher; // in cpp

public:
    virtual ~Benachrichtiger() = default;

    // Hier konnen sich andere Klassen registrieren
    int registriereListener(std::function<void()> callback);
    void entferneListener(int id);

protected:
    // Diese Methode rufst du im abgeleiteten Objekt auf, wenn sich etwas andert
    void benachrichtigeListenerAsync();
};
