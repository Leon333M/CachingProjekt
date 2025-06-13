// AufgabenVerteiler.h
#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class AufgabenVerteiler {
private:
    std::thread arbeitsThread;                  // Hintergrund-Thread für Aufgaben
    std::mutex mutex;                           // Schutz für die Warteschlange
    std::condition_variable bedingung;          // Zum Warten auf neue Aufgaben
    std::queue<std::function<void()>> aufgaben; // Warteschlange für Aufgaben
    bool beenden = false;                       // Signal zum Beenden des Threads

public:
    AufgabenVerteiler() {
        arbeitsThread = std::thread([this]() {
            while (true) {
                std::function<void()> aktuelleAufgabe;

                {
                    std::unique_lock<std::mutex> schloss(mutex);
                    bedingung.wait(schloss, [this]() {
                        return beenden || !aufgaben.empty();
                    });

                    if (beenden && aufgaben.empty())
                        return;

                    aktuelleAufgabe = std::move(aufgaben.front());
                    aufgaben.pop();
                }

                // Aufgabe ausführen (außerhalb des Locks!)
                aktuelleAufgabe();
            }
        });
    }

    ~AufgabenVerteiler() {
        {
            std::lock_guard<std::mutex> schloss(mutex);
            beenden = true;
        }
        bedingung.notify_all();
        if (arbeitsThread.joinable()) {
            arbeitsThread.join();
        }
    }

    // Neue Aufgabe hinzufügen
    void einreihen(std::function<void()> aufgabe) {
        {
            std::lock_guard<std::mutex> schloss(mutex);
            aufgaben.push(std::move(aufgabe));
        }
        bedingung.notify_one();
    }
};
