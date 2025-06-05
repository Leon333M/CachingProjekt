// RingSpeicher.h
#pragma once
#include <deque>
#include <unordered_set>
#include <vector>

template <typename Typ>

class RingSpeicher {
private:
    std::vector<std::unordered_set<Typ>> daten;

public:
    RingSpeicher(int depth = 3, int initVectorSize = 64) {
        daten.resize(depth);
        for (std::unordered_set<Typ> &data : daten) {
            data.reserve(initVectorSize);
        }
    }

    bool istVorhanden(const std::unordered_set<Typ> &data, const Typ &objekt) const {
        return (data.find(objekt) != data.end());
    }

    int count(const Typ &objekt) const {
        int count = 0;
        for (std::unordered_set<Typ> &data : daten) {
            if (istVorhanden(data, objekt)) {
                count++;
            } else {
                break;
            }
        }
        return count;
    }

    void add(const Typ objekt) {
        int count = count(objekt);
        if (count < daten.size()) {
            daten[count].insert(objekt);
        }
    }

    void remove(const Typ &objekt) {
        for (std::unordered_set<Typ> &data : daten) {
            data.erase(objekt);
        }
    }

    int size() {
        int size = 0;
        for (std::unordered_set<Typ> &data : daten) {
            size += data.size();
        }
        return size;
    }

    void pop_front() {
        if (daten.size() > 0) {
            std::unordered_set<Typ> &data = daten[0];
            Typ objekt = data.begin();
            remove(objekt);
        }
    }
};
