// AufgabenVerteiler.h
#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class AufgabenVerteiler {
private:
    std::thread arbeitsThread;                                    // Hintergrund-Thread für Aufgaben
    std::mutex mutex;                                             // Schutz für die Warteschlange
    std::condition_variable bedingung;                            // Zum Warten auf neue Aufgaben
    bool beenden = false;                                         // Signal zum Beenden des Threads
    std::unordered_map<uint64_t, std::function<void()>> aufgaben; // Warteschlange für Aufgaben

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

                    auto it = aufgaben.begin();
                    aktuelleAufgabe = std::move(it->second);
                    aufgaben.erase(it);
                }

                // Aufgabe ausfuhren
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

    static uint64_t generiereSlotHash() {
        thread_local uint64_t lokalerZaehler = 0;

        // Thread-ID zu 64-bit Hash machen
        std::hash<std::thread::id> hasher;
        uint64_t threadHash = static_cast<uint64_t>(hasher(std::this_thread::get_id()));

        // Slot-Hash: obere 32 Bit Thread-ID, untere 32 Bit lokaler Zahler
        uint64_t slotHash = (threadHash << 32) | (lokalerZaehler++);
        return slotHash;
    }

    // Neue Aufgabe hinzufugen
    void einreihen(std::function<void()> aufgabe) {
        uint64_t slotHash = generiereSlotHash();
        {
            // std::lock_guard<std::mutex> schloss(mutex); // Rausoptimiert, da ineffizient
            aufgaben[slotHash] = std::move(aufgabe);
        }
        bedingung.notify_one();
    }
};
