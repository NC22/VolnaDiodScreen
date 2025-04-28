#include "WifiManager.h"


WiFiManager::WiFiManager() {
}

IPAddress WiFiManager::getIP() {
    return isAPmode() ? WiFi.softAPIP() : WiFi.localIP();
}

void WiFiManager::updateStatus() {
    lastConnectStatus = WiFi.status();
}

wl_status_t WiFiManager::connect(String sid, String password) {

    WiFi.softAPdisconnect();
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);   

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    WiFi.begin(sid.c_str(), password.c_str());
    unsigned int waitTime = 0;

    unsigned long askTimerStart = millis();
    while (true) {

      if (millis() - askTimerStart >= 1000) {
        
        waitTime++;
        askTimerStart = millis();
        updateStatus();

        if ( lastConnectStatus == WL_CONNECTED) {
            break;
        } else if (lastConnectStatus == WL_CONNECT_FAILED) {
            Serial.print("FAIL TO CONNECT TO LAST CONFIGURED ACCESS POINT");
            break;
        } else if (lastConnectStatus == WL_WRONG_PASSWORD) {
            Serial.print("WRONG PASSWORD");
            break;            
        } else if (lastConnectStatus == WL_NO_SSID_AVAIL) {
            Serial.println("No any networks with specified network name | SSID");
            break;
        }
      }

      if (waitTime > 10) {
          Serial.print("Connect fail, exit by timeout");
          return WL_CONNECT_FAILED;
      }

      delay(100); 
      if (onConnectWaitTick != NULL) onConnectWaitTick();
    }

    return lastConnectStatus;
}

bool WiFiManager::isAPmode() {
  return WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA ? true : false;
}

void WiFiManager::runAsAccesspoint() {

    WiFi.softAPdisconnect();
    WiFi.disconnect();
    // WiFi.mode(WIFI_AP);

    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    
    WiFi.softAP(apSSID.c_str(), apPassword.c_str());
    WiFi.softAPConfig(local_ip, gateway, subnet);

    delay(100);

    Serial.println("Run as Access Point [ready]");
}

String WiFiManager::getWiFiSelectorHtml() {
  
  String ssid;
  
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;

  bool hidden;
  int scanResult;

  String selectorHtml; 

  Serial.println(F("Starting WiFi scan..."));

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/false);

  char buffer[256];
  sprintf(buffer, "Networks : %d", scanResult);
  
  selectorHtml += buffer;
  selectorHtml += "<select name=\"network\">";

  if (scanResult == 0) {
    selectorHtml += "<option>No networks</option>";
  } else if (scanResult > 0) {
 
    for (int8_t i = 0; i < scanResult; i++) {
      
      selectorHtml += buffer;

      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
      selectorHtml += "<option value=\"" + ssid + "\">" + ssid + "</option>";
      
       yield();
    }

    selectorHtml += "</select>";

  } else {

    sprintf(buffer, "WiFi scan error : %d", scanResult);    
    selectorHtml = buffer;
  
  }

  return selectorHtml;
}




