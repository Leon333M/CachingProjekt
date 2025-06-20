// ShutdownEventFilter.h
#pragma once
#include <QAbstractNativeEventFilter>
#include <QApplication>

class ShutdownEventFilter : public QAbstractNativeEventFilter {
private:
    GuiManager *guiManager = nullptr;

public:
    ShutdownEventFilter(GuiManager *gm) : guiManager(gm) {}
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override {
        MSG *msg = static_cast<MSG *>(message);
        // PLOG_DEBUG << "Qt nativeEventFilter: ";
        // PLOG_DEBUG << msg->message;
        if (msg->message == 800) {
            PLOG_DEBUG << "Windows fahrt gerade runter!";
            // Hier Cache runterfahren
            if (guiManager != nullptr) {
                guiManager->shutdown();
                PLOG_DEBUG << "guiManager erfolgreich heruntergefahren";
            }
            *result = TRUE; // Shutdown wird akzeptiert
            return true;
        }

        return false;
    }
};
