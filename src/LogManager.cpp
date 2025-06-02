// LogManager.cpp
#include "LogManager.h"

LogManager::LogManager(int logLevel) {
    // init plog
    this->logLevel = logLevel;
    plog::init(mapLogLevelToPlogSeverity(this->logLevel), &consoleAppender);
    PLOG_INFO << L"init LogManager";
}

int LogManager::getLogLevel() {
    return logLevel;
}

plog::Severity LogManager::mapLogLevelToPlogSeverity(int level) {
    switch (level) {
        case 6:
            return plog::verbose;
        case 5:
            return plog::debug;
        case 4:
            return plog::info;
        case 3:
            return plog::warning;
        case 2:
            return plog::error;
        case 1:
            return plog::fatal;
        case 0:
            return plog::none;
        default:
            return plog::none;
    }
}
