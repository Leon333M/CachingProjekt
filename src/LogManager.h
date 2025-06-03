// LogManager.h
#pragma once
#include <optional>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>

class LogManager {
private:
    // 6 PLOG_VERBOSE   (ganz feine Detailinfos – optional, oft ignoriert)
    // 5 PLOG_DEBUG     (zum Debuggen gedacht)
    // 4 PLOG_INFO      (normale Informationen)
    // 3 PLOG_WARNING   (etwas Unerwartetes, aber noch ok)
    // 2 PLOG_ERROR     (ein Fehler, der etwas verhindert hat)
    // 1 PLOG_FATAL     (kritischer Fehler, nach dem das Programm evtl. absturzt)
    int logLevel;
    std::string logDatei;
    plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    std::optional<plog::RollingFileAppender<plog::TxtFormatter>> fileAppender;

public:
    LogManager(int logLevel = 1, std::string logDatei = "");
    int getLogLevel();

    /**
     * @brief Ubersetzt ein int-Log-Level in plog::Severity.
     *
     * Log-Level:
     *  6 PLOG_VERBOSE   (ganz feine Detailinfos – optional, oft ignoriert)
     *  5 PLOG_DEBUG     (zum Debuggen gedacht)
     *  4 PLOG_INFO      (normale Informationen)
     *  3 PLOG_WARNING   (etwas Unerwartetes, aber noch ok)
     *  2 PLOG_ERROR     (ein Fehler, der etwas verhindert hat)
     *  1 PLOG_FATAL     (kritischer Fehler, nach dem das Programm evtl. abstürzt)
     *
     * @param level Ganzzahliger Log-Level (0–5)
     * @return plog::Severity Entsprechendes PLOG-Level
     */
    plog::Severity mapLogLevelToPlogSeverity(int level);
};
