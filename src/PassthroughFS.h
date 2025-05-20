// PassthroughFS.h
#pragma once
#include <string>
#include <winfsp/winfsp.h>

class PassthroughFS {
private:
    std::string backend;
    std::wstring wBackend;
    std::string mountPoint;
    std::wstring wMountPoint;
    bool mounted;

public:
    PassthroughFS(const std::string backendPath);
    void mount(const std::string mountPointPath);
    void unmount();
    bool isMounted() const { return mounted; }
};
