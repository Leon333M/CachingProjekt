// CacheInterface.h
#pragma once
#include <deque>
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
    std::deque<std::wstring> pfadHistorie; // std::wstring::reserve()
    int minZugriffsHaufigkeit = 2;
    const int maxPfadHistorie = 64;
    std::deque<HANDLE> handleHistorie;
    const int maxHandleHistorie = 8;

public:
    /**
     * @brief Zerstort das CacheInterface-Objekt
     *
     */
    virtual ~CacheInterface() = default;

    /**
     * @brief Setzt die maximale Cache-Größe.
     *
     * @param maxCacheSizeInGb Maximale Cache-Größe in Gigabyte.
     */
    void setMaxCacheSize(UINT64 maxCacheSizeInGb);

    /**
     * @brief Setzt die minimale Zugriffs-Häufigkeit.
     *
     * @param minZugriffsHaufigkeit Minimale Anzahl an Zugriffen, ab der eine Datei im Cache verbleibt.
     */
    void setMinZugriffsHaufigkeit(int minZugriffsHaufigkeit);

    /**
     * @brief Read ist die Grundfunktion für den Cache.
     * Hier wird gepruft, ob die Datei im Cache liegt. Wenn nicht,
     * wird entschieden, ob diese Datei dann gecacht wird.
     *
     * @param Alle Parameter sind die WinAPI-Parameter, die nur durchgereicht werden.
     * @return true Wenn erfolgreich vom Cache gelesen.
     * @return false Wenn der Pfad nicht im Cache ist und somit nicht eingelesen wurde.
     */
    virtual bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) = 0;

    /**
     * @brief Write dient der Handhabung eines potenziellen WriteCaches und der Information, dass die Datei geaendert wurde.
     *
     * @param Alle Parameter sind die WinAPI-Parameter, die nur durchgereicht werden.
     * @return true wenn kein Schreiben im Originalpfad mehr noetig ist.
     * @return false wenn die Datei noch gespeichert werden muss.
     */
    virtual bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) = 0;

    /**
     * @brief Remove dient der Entfernung einer Datei aus dem Cache. Nuetzlich bei einem Write oder zum Aufraeumen.
     *
     * @param fullPath ist der gespeicherte Pfad im Cache. Im Normalfall also der Pfad zur Originaldatei.
     */
    virtual void Remove(const std::wstring &fullPath) = 0;

    /**
     * @brief RemoveHandle ruft nur die Remove auf, indem es sich den Pfad von handle holt.
     *
     * @param handle ist der Winfsp-Handle, von dem nur der Pfad zur Datei extrahiert wird.
     */
    void RemoveHandle(HANDLE handle);

    /**
     * @brief Clear leert den Cache, indem es Remove von allen gecachten Pfaden aufruft.
     *
     */
    virtual void Clear();

protected:
    /**
     * @brief ShouldCachePath dient zur Strukturierung des Caches, in dem es abfragt, ob die Datei gecacht werden soll.
     *
     * @param fullPath ist der Pfad zur Originaldatei, der zur eindeutigen Identifikation der Datei genutzt wird.
     * @return true, wenn die Datei gecacht werden soll.
     * @return false, wenn die Datei nicht gecacht werden soll.
     */
    virtual bool ShouldCachePath(const std::wstring &fullPath) = 0;

    /**
     * @brief ShouldHandleCache dient dazu, um zu verhindern, dass bei Einlesen einer Datei nicht mehrmals ShouldCachePath aufgerufen wird.
     *
     * @param handle Standard-Winfsp-Handle, der gespeichert wird, wenn nicht vorhanden.
     * @return true, wenn der Handle und somit der Read nicht schon einmal weitergeleitet wurde (also true zurückgegeben wurde).
     * @return false, wenn der Handle bereits vorhanden ist, was bedeutet, dass der Pfad zur Datei schon an den Cache weitergeleitet wurde.
     */
    bool ShouldHadelCache(const HANDLE handle);

    /**
     * @brief countPathInHistory zaehlt, wie oft fullPath in pfadHistorie auftaucht.
     *
     * @param fullPath Der std::wstring, der gezahlt wird.
     * @return int Die Anzahl, wie oft fullPath vorhanden ist.
     */
    int countPathInHistory(const std::wstring &fullPath);
};
