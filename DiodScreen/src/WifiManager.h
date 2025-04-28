#ifndef WiFiManager_h
#define WiFiManager_h

#include <Arduino.h>
#include <ESP8266WiFi.h>

class WiFiManager {
    private :
    public:
       
        WiFiManager();

        String apSSID = "KellyClock22";
        String apPassword = "kc12345678";

        wl_status_t connect(String sid, String password);
        String getWiFiSelectorHtml();
        IPAddress getIP();
        bool isAPmode();
        void runAsAccesspoint();

        void updateStatus();

        wl_status_t lastConnectStatus;
        std::function<void()> onConnectWaitTick = NULL; // 100 ms timer callback during connection process
};


#endif
