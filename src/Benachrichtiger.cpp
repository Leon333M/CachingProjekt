// Benachrichtiger.cpp
#include "Benachrichtiger.h"
#include "AufgabenVerteiler.h"
#include <mutex>

static AufgabenVerteiler dispatcher;

void Benachrichtiger::registriereListener(std::function<void()> callback) {
    listener.push_back(std::move(callback));
}

void Benachrichtiger::benachrichtigeListenerAsync() {
    for (const auto &f : listener) {
        dispatcher.einreihen(f); // Fuhrt Listener asynchron aus
    }
}
