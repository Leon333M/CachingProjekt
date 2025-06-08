// CacheInterface.h
#pragma once
#include "RingSpeicher.h"
#include <deque>
#include <plog/Log.h>
#include <shared_mutex>
#include <string>
#include <unordered_set>
#include <windows.h>

/**
 * @brief CacheInterface ist die Grundstruktur, die zum Implementieren des Caches benutzt wird.
 * Jeder Cache ist eine Ableitung des CacheInterface, damit die Klasse, die den Cache dann nutzt,
 * nur die Funktionen dieses Interfaces nutzen muss und somit modularer gestaltet werden kann.
 *
 */
class CacheInterface {
protected:
    UINT64 maxCacheSize = 8ULL * 1024 * 1024 * 1024; // 8 GB
    UINT64 currentCacheSize = 0;
    std::unordered_set<std::wstring> cashePfade;

private:
    CacheInterface *nextCache = nullptr;
    const int maxHandleHistorie = 8;
    RingSpeicher<HANDLE> handleHistorie;
    int minZugriffsHaufigkeit = 2;
    const int maxPfadHistorie = 64;
    RingSpeicher<std::wstring> pfadHistorie;

public:
    /**
     * @brief Konstruktor fuer CacheInterface. Initialisiert handleHistorie und pfadHistorie.
     *
     */
    CacheInterface();

    /**
     * @brief Zerstort das CacheInterface-Objekt
     *
     */
    virtual ~CacheInterface() = default;

    /**
     * @brief Setzt die maximale Cache-Grosse.
     *
     * @param maxCacheSizeInGb Maximale Cache-Grosse in Gigabyte.
     */
    void setMaxCacheSize(UINT64 maxCacheSizeInGb);

    /**
     * @brief Setzt die minimale Zugriffs-Haufigkeit.
     *
     * @param minZugriffsHaufigkeit Minimale Anzahl an Zugriffen, ab der eine Datei im Cache verbleibt.
     */
    void setMinZugriffsHaufigkeit(int minZugriffsHaufigkeit);

    /**
     * @brief Setzt den Pointer auf den cache.
     *
     * @param cache der Cache auf den der Pointers gesetzt wird.
     */
    virtual void setNextCache(CacheInterface *cache);

    /**
     * @brief read ist die Grundfunktion fur den Cache.
     * Hier wird gepruft, ob die Datei im Cache liegt. Wenn nicht,
     * wird entschieden, ob diese Datei dann gecacht wird.
     *
     * @param Alle Parameter sind die WinAPI-Parameter, die nur durchgereicht werden.
     * @return true Wenn erfolgreich vom Cache gelesen.
     * @return false Wenn der Pfad nicht im Cache ist und somit nicht eingelesen wurde.
     */
    virtual bool read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);

    /**
     * @brief write dient der Handhabung eines potenziellen WriteCaches und der Information, dass die Datei geaendert wurde.
     *
     * @param Alle Parameter sind die WinAPI-Parameter, die nur durchgereicht werden.
     * @return true wenn kein Schreiben im Originalpfad mehr noetig ist.
     * @return false wenn die Datei noch gespeichert werden muss.
     */
    virtual bool write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);

    /**
     * @brief remove dient der Entfernung einer Datei aus dem Cache. Nuetzlich bei einem write oder zum Aufraeumen.
     *
     * @param fullPath ist der gespeicherte Pfad im Cache. Im Normalfall also der Pfad zur Originaldatei.
     */
    virtual void remove(const std::wstring &fullPath);

    /**
     * @brief removeHandle ruft nur die remove auf, indem es sich den Pfad von handle holt.
     *
     * @param handle ist der Winfsp-Handle, von dem nur der Pfad zur Datei extrahiert wird.
     */
    void removeHandle(HANDLE handle);

    /**
     * @brief clear leert den Cache, indem es remove von allen gecachten Pfaden aufruft.
     *
     */
    virtual void clear();

protected:
    /**
     * @brief Entfernt Daten aus dem Cache bis zur angegebenen Groesse.
     * Diese Methode versucht, Speicherplatz im Cache freizugeben,
     * indem sie Daten entfernt, bis mindestens die angegebene Groesse
     * freigegeben wurde.
     *
     * @param size Die gewuenschte Freigabegroesse in Bytes.
     * @return Die tatsaechlich freigegebene Groesse in Bytes.
     */
    UINT64 clear(const UINT64 &size);

    /**
     * @brief addFileSize dient dazu, die Dateigroesse für die interne Verwaltung zu speichern.
     *
     * @param fileSize ist die Groesse der Datei in Bytes.
     * @return true, wenn die Dateigroesse erfolgreich hinzugefuegt wurde.
     * @return false, wenn ein Fehler aufgetreten ist.
     */
    bool addFileSize(const UINT64 &fileSize);

    /**
     * @brief addFile registriert eine Datei im Cache anhand ihres Pfads und zugehoerigen Handles.
     *
     * @param fullPath ist der Pfad zur Originaldatei, die gecacht werden soll.
     * @param handle ist der WinFsp-Handle zur geoeffneten Datei.
     * @return true, wenn die Datei erfolgreich registriert wurde.
     * @return false, wenn ein Fehler aufgetreten ist.
     */
    bool addFile(const std::wstring &fullPath, HANDLE handle);

    /**
     * @brief readCache versucht, eine Datei direkt aus dem Cache zu lesen.
     *
     * @param fullPath ist der Pfad zur Originaldatei, die gelesen werden soll.
     * @param handle ist der WinFsp-Handle zur Datei.
     * @param buffer ist der Zielpuffer, in den die Daten gelesen werden.
     * @param length ist die Anzahl der zu lesenden Bytes.
     * @param bytesTransferred gibt die tatsachlich gelesene Byteanzahl zurueck.
     * @param overlapped wird von WinFsp fuer asynchrone Lesevorgaenge bereitgestellt und ist immer gueltig.
     * @return true, wenn die Datei erfolgreich aus dem Cache gelesen wurde.
     * @return false, wenn ein Fehler aufgetreten ist oder kein Cache-Eintrag vorliegt.
     */
    virtual bool readCache(const std::wstring &fullPath, HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) = 0;

    /**
     * @brief storeInCache legt eine Datei im Cache ab.
     *
     * @param fullPath ist der Pfad zur Originaldatei, die gespeichert werden soll.
     * @param handle ist der WinFsp-Handle zur Datei.
     * @return true, wenn die Datei erfolgreich im Cache abgelegt wurde.
     * @return false, wenn ein Fehler aufgetreten ist.
     */
    virtual bool storeInCache(const std::wstring &fullPath, HANDLE handle) = 0;

    /**
     * @brief Entfernt eine Datei aus dem Cache und aktualisiert den belegten Speicher.
     * Die Methode loescht die angegebene Datei aus dem Cache-Speicher. Falls vorhanden,
     * wird zusaetzlich die Dateigroesse vom currentCacheSize abgezogen.
     *
     * @param fullPath Der Pfad zur Originaldatei, wie er im Cache gespeichert wurde.
     * @return true Falls die Datei erfolgreich entfernt wurde.
     * @return false Falls die Datei nicht im Cache vorhanden oder nicht loeschbar war.
     */
    virtual bool removeCache(const std::wstring &fullPath) = 0;

    /**
     * @brief Berechnet die Groesse des Eintrags fuer einen gegebenen Pfad.
     *
     * Diese Methode ermittelt, wie viel Speicherplatz der Cache-Eintrag
     * fuer den angegebenen Pfad belegt.
     *
     * @param Path Der Dateipfad, dessen Cache-Groesse berechnet werden soll.
     * @return Die Groesse des Eintrags in Bytes.
     */
    UINT64 sizeFromPath(const std::wstring &Path);

    /**
     * @brief shouldCachePath dient zur Strukturierung des Caches, in dem es abfragt, ob die Datei gecacht werden soll.
     *
     * @param fullPath ist der Pfad zur Originaldatei, der zur eindeutigen Identifikation der Datei genutzt wird.
     * @return true, wenn die Datei gecacht werden soll.
     * @return false, wenn die Datei nicht gecacht werden soll.
     */
    bool shouldCachePath(const std::wstring &fullPath);

    /**
     * @brief ShouldHandleCache dient dazu, um zu verhindern, dass bei Einlesen einer Datei nicht mehrmals shouldCachePath aufgerufen wird.
     *
     * @param handle Standard-Winfsp-Handle, der gespeichert wird, wenn nicht vorhanden.
     * @return true, wenn der Handle und somit der read nicht schon einmal weitergeleitet wurde (also true zuruckgegeben wurde).
     * @return false, wenn der Handle bereits vorhanden ist, was bedeutet, dass der Pfad zur Datei schon an den Cache weitergeleitet wurde.
     */
    bool shouldHadelCache(const HANDLE handle);

    /**
     * @brief countPathInHistory zaehlt, wie oft fullPath in pfadHistorie auftaucht.
     *
     * @param fullPath Der std::wstring, der gezahlt wird.
     * @return int Die Anzahl, wie oft fullPath vorhanden ist.
     */
    int countPathInHistory(const std::wstring &fullPath);

    /**
     * @brief Entfernt den angegebenen Pfad aus der Historie.
     *
     * @param fullPath Der Pfad, der aus der Historie entfernt werden soll.
     */
    void removePathFromHistory(const std::wstring &fullPath);

    /**
     * @brief Fuegt einen Pfad der Historie hinzu.
     *
     * @param fullPath Der Pfad, der zur Historie hinzugefuegt werden soll.
     */
    void addPathToHistory(const std::wstring &fullPath);

    /**
     * @brief readNextCache ruft read von nachsten Cache auf wenn gesetzt.
     *
     * @param Alle Parameter sind die WinAPI-Parameter, die nur durchgereicht werden.
     * @return true Wenn erfolgreich vom Cache gelesen.
     * @return false Wenn der Pfad nicht im Cache ist und somit nicht eingelesen wurde.
     */
    bool readNextCache(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
};
