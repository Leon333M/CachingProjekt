// ConfigLoader.h
#include "VirtuelleFestplatte.h"
#include <memory>

class ConfigLoader {
private:
    std::unordered_map<std::string, std::shared_ptr<CacheInterface>> cacheMap;
    std::vector<VirtuelleFestplatte> vhdds;
    std::vector<std::string> configData;

public:
    bool loadFromFile(const std::wstring &filename);
    void start();
    void clear();
    std::wstring stringToWString(const std::string &str);
    std::string wstringToString(const std::wstring &wstr);

private:
    void starteVhdd(VirtuelleFestplatte &vhdd);
    bool loadFile(const std::wstring &filename);
    void verarbeiteteDaten();
    void erstelleSsdCache(std::string zeile);
    void erstelleRamCache(std::string zeile);
    void erstelleCache(std::string zeile);
    void erstelleVhdd(std::string zeile);
};
