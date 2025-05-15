#pragma once
#include <string>

class PassthroughFS {
public:
    PassthroughFS(const std::string& backendPath);
    void mount(const std::string& mountPoint);
    void unmount();

private:
    std::string backend;
};