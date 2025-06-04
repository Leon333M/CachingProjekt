// RingSpeicher.h
#pragma once

template <typename Typ>

class RingSpeicher {
private:
    std::unique_ptr<Typ[]> daten;
    int aktullePosition = 0;
    int size;

public:
    RingSpeicher(int maxGrosse) : size(maxGrosse), daten(std::make_unique<Typ[]>(size)) {}

    int count(Typ objekt) {
        std::count(daten, objekt);
    }

    void add(Typ objekt, int position) {
        if (position > aktullePosition) {
            position = 0;
        }
        daten[position] = objekt;
    }

    void add(Typ objekt) {
        add(objekt, aktullePosition);
        aktullePosition++;
    }
};
