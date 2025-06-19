// ShutdownEventFilter.h
#pragma once
#include <QAbstractNativeEventFilter>
#include <QApplication>

class ShutdownEventFilter : public QAbstractNativeEventFilter {
public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override {
        MSG *msg = static_cast<MSG *>(message);
        // PLOG_DEBUG << "Qt nativeEventFilter: ";
        // PLOG_DEBUG << msg->message;
        if (msg->message == 800) {
            PLOG_DEBUG << "Windows fahrt gerade runter!";
            // Hier Cache runterfahren
            *result = TRUE; // Shutdown wird akzeptiert
            return true;
        }

        return false;
    }
};
