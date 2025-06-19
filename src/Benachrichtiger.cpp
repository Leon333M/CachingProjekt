// Benachrichtiger.cpp
#include "Benachrichtiger.h"
#include "AufgabenVerteiler.h"
#include <mutex>
#include <shared_mutex>

static AufgabenVerteiler dispatcher;
std::shared_mutex sperre;

int Benachrichtiger::registriereListener(std::function<void()> callback) {
    std::unique_lock<std::shared_mutex> lock(sperre);
    listener.push_back(std::move(callback));
    int id = naechsteId++;
    idZuIndex[id] = listener.size() - 1;
    return id;
}

void Benachrichtiger::entferneListener(int id) {
    std::unique_lock<std::shared_mutex> lock(sperre);
    auto it = idZuIndex.find(id);
    if (it != idZuIndex.end()) {
        size_t index = it->second;
        if (index < listener.size()) {
            listener.erase(listener.begin() + index);
            idZuIndex.erase(it);

            // Alle nachfolgenden Indizes aktualisieren
            for (auto &[key, val] : idZuIndex) {
                if (val > index) {
                    val--;
                }
            }
        }
    }
}

void Benachrichtiger::benachrichtigeListenerAsync() {
    std::shared_lock<std::shared_mutex> lock(sperre);
    for (const auto &f : listener) {
        dispatcher.einreihen(f); // Fuhrt Listener asynchron aus
    }
}
