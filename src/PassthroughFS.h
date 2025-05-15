#pragma once
#include <string>

class PassthroughFS {
private:
    std::string backend;

public:
    PassthroughFS(const std::string& backendPath);
    void mount(const std::string& mountPoint);
    void unmount();
    
};