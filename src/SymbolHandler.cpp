// SymbolHandler.cpp
#include "SymbolHandler.h"
#include <iostream>
#include <shellapi.h>
#include <windows.h>

ConfigLoader *configLoader0 = nullptr;
NOTIFYICONDATA nid;

void AddTrayIcon(HWND hwnd) {
    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_USER + 1;          // Definiere eine eigene Nachricht
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION); // Standard-Icon
    strcpy(nid.szTip, "Caching Programm");

    Shell_NotifyIcon(NIM_ADD, &nid); // Füge das Icon zur Tray-Leiste hinzu
}

void RemoveTrayIcon() {
    Shell_NotifyIcon(NIM_DELETE, &nid); // Entferne das Tray Icon
}

void OnTrayIconClick(WPARAM wParam) {
    if (wParam == 1) { // Wenn auf das Tray Icon geklickt wird
        MessageBox(NULL, "Programm wird beendet...", "Beenden", MB_OK);
        configLoader0->clear();
        PostQuitMessage(0); // Beende die Nachrichtenschleife
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_USER + 1:
            if (lParam == WM_LBUTTONUP) {
                OnTrayIconClick(wParam);
            }
            return 0;
        case WM_DESTROY:
            RemoveTrayIcon();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

SymbolHandler::SymbolHandler(ConfigLoader *configLoader) {
    configLoader0 = configLoader;
    thread = std::thread(&SymbolHandler::startSymbolHandler, this);
}

void SymbolHandler::startSymbolHandler() {
    // Initialisiere das Fenster für das Tray Icon
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "TrayApp";

    if (!RegisterClass(&wc)) {
        return;
    }

    HWND hwnd2 = CreateWindow(wc.lpszClassName, "Tray App", 0, 0, 0, 0, 0, 0, 0, 0, hInstance, 0);
    if (hwnd2 == NULL) {
        return;
    }

    AddTrayIcon(hwnd2);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
