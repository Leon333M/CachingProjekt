// RingSpeicher.h
#pragma once
#include <unordered_set>
#include <vector>

template <typename Typ>

/**
 * @brief Dient zur Haltung von Daten, ohne Sperren zu brauchen.
 *
 */
class RingSpeicher {
private:
    std::vector<std::unordered_set<Typ>> daten;
    int vectorSize;

public:
    RingSpeicher(int depth = 3, int initVectorSize = 64) {
        setVectorSize(initVectorSize);
        daten.resize(depth);
        for (std::unordered_set<Typ> &data : daten) {
            data.reserve(initVectorSize);
        }
        vectorSize = initVectorSize;
    }

    bool istVorhanden(const std::unordered_set<Typ> &data, const Typ &objekt) const {
        return (data.find(objekt) != data.end());
    }

    int count(const Typ &objekt) const {
        int count = 0;
        for (const std::unordered_set<Typ> &data : daten) {
            if (istVorhanden(data, objekt)) {
                count++;
            } else {
                break;
            }
        }
        return count;
    }

    void add(const Typ objekt) {
        int count = this->count(objekt);
        if (count < daten.size()) {
            daten[count].insert(objekt);
        }
    }

    void remove(const Typ &objekt) {
        for (std::unordered_set<Typ> &data : daten) {
            data.erase(objekt);
        }
    }

    int size() const {
        int size = 0;
        for (const std::unordered_set<Typ> &data : daten) {
            size += data.size();
        }
        return size;
    }

    void pop_front() {
        if (daten.size() > 0) {
            std::unordered_set<Typ> &data = daten[0];
            Typ objekt = *data.begin();
            remove(objekt);
        }
    }

    void setDepth(int depth) {
        int oldDepth = daten.size();
        daten.resize(depth);
        if (oldDepth < depth) {
            setVectorSize(vectorSize);
        }
    }

    void setVectorSize(int size) {
        vectorSize = size;
        for (int i = 0; i < daten.size(); i++) {
            std::unordered_set<Typ> &data = daten.at(i);
            int maxUsedSize = (vectorSize / (i + 1)) + 1; // +1 == aufrunden
            data.reserve(maxUsedSize);
        }
    }
};
