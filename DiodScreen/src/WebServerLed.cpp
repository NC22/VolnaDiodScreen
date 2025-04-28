#include <WebServerLed.h>

WebServerLed::WebServerLed(Env * env, int port) : WebServerBase(env, port) {
    
    #if defined(ESP32)
        ramFriendlyMode = false;
    #else 
        ramFriendlyMode = true;
    #endif

    server->on("/api/mode", std::bind(&WebServerLed::switchMode, this)); 
    server->on("/api/get", std::bind(&WebServerLed::apiGet, this));     

    server->on("/api/notification", std::bind(&WebServerLed::showNotification, this));
    server->on("/api/delete/notification", std::bind(&WebServerLed::apiDeleteNotification, this)); 
    server->on("/api/sleeptimer", std::bind(&WebServerLed::apiTempSleep, this)); 
    server->on("/api/modeorder", std::bind(&WebServerLed::apiTempModelist, this)); 

    server->on("/api/direct/input", std::bind(&WebServerLed::apiDirectInput, this)); 
    //server.on("/api/getinfo", std::bind(&WebServer::apiGetInfo, this));
}

void WebServerLed::router() {


    if (defaultRoute(false)) {    
        return;
    } 


    if (server->uri().indexOf("/out/language.js") != -1) {
        apiGetLanguage();
    }
}

String WebServerLed::getInfo() {

    String json = WebServerBase::getInfo(); // "{";

    json += ",";
    json += "\"sensors\":" + env->getSensorsJSON();
    json += ",";
    json += "\"notifications\":" + env->getNotificationsJSON();

    return json;
}

void WebServerLed::apiGetLanguage() {

    if (ramFriendlyMode) {
    
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_locale_js);

        server->send(200, "text/javascript; charset=utf-8", "");
        
        outputROMData(webdata_locale_js, webdataSize_locale_js);

        server->sendContent("");
    } else {


        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_locale_js);
        server->send(200, "text/javascript; charset=utf-8", FPSTR(webdata_locale_js)); 
    }
}

void WebServerLed::apiGet() {

    String result = "";
    for (int i = 0; i < server->args(); i++)  {
        
        String argResult = "";

        if (server->argName(i) == "currentMode") {
            
            String modeName = "";
            if (env->currentMode == modeDate) {
                modeName = "date";
            } else if (env->currentMode == modeTime) {
                modeName = "time";
            } else if (env->currentMode == modeTemp) {
                modeName = "temp";
            } else if (env->currentMode == modeText) {
                modeName = "text";
            } else if (env->currentMode == modeHum) {
                modeName = "hum";
            } else if (env->currentMode == modeIP) {
                modeName = "ip";
            } else if (env->currentMode == modeSleep) {
                modeName = "sleep";
            } else if (env->currentMode == modeSleepUser) {
                modeName = "sleepUser";
            } else modeName = "other";

            argResult += "\"currentMode\":\"" + modeName + "\"";

        } else if (server->argName(i) == "modeTimerCurrent") {

            argResult += "\"modeTimerCurrent\":\"" + String(env->modeTimerCurrent) + "\"";
        } 

        if (argResult.length() > 0) {
            if (result.length() > 0) result += ",";
            result += argResult;
        }
    } 

    server->send(200, "application/json", "{" + result + "}"); 
}

void WebServerLed::apiTempModelist() {

    int state = 0;
    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i) == "modelist") {
            
            env->getConfig()->set(cModeList, server->arg(i));
            state++;

        } else if (server->argName(i) == "enabled") {
            
            env->getConfig()->set(cModeListEnabled, server->arg(i));
            state++;
        } 
    } 

    String result = state == 2 ? "ok" : "fail";
    env->validateConfig();
    server->send(200, "application/json", "{\"result\":\"" + result + "\"}"); 
}

void WebServerLed::apiTempSleep() {   

    int states = 0;
    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i) == "from") {
            
            if (env->getScreenClock()->setSleepState(server->arg(i), true)) states++;

        } else if (server->argName(i) == "to") {
           
            if (env->getScreenClock()->setSleepState(server->arg(i), false)) states++;

        } 
    } 

    env->getScreenClock()->enableSleepControll(states == 2 ? true : false);

    String result = states == 2 ? "ok" : "fail";
    server->send(200, "application/json", "{\"result\":\""  + result + "\"}"); 
}

void WebServerLed::showNotification() {

  int argCollected = 0;  
  int stringMaxSize = 255;

  String key = "";
  String text = "";
  int scrolled = -1; // auto

  int duration = -1000; int defaultDuration = 3;
  int showTimes = -1000; int defaultShowTimes = 3;

  for (int i = 0; i < server->args(); i++)  {

    if (server->argName(i) == "text") {

        argCollected++;
        
        String tmpText = server->arg(i); 
        int tmpTextLength = tmpText.length();
        
        if (tmpTextLength > stringMaxSize) { 
            // utf-8 unsafe cut
            for (int i = 0; i < stringMaxSize; i++) {
              text += tmpText[i];              
            }
            
        } else {
            text = tmpText;
        }
        
        Serial.println("text : " + text);
        
        // std::strncpy(config.ssid, server.arg(i).c_str(), sizeof(server.arg(i)));
        
    } else if (server->argName(i) == "duration") {
        
        if(sscanf(server->arg(i).c_str(), "%d", &duration) != 1) {
            duration = defaultDuration;  
        }

    } else if (server->argName(i) == "showTimes") {
        
        if(sscanf(server->arg(i).c_str(), "%d", &showTimes) != 1) {
            showTimes = defaultShowTimes;  
        }

    } else if (server->argName(i) == "key" && server->arg(i).length() <= 64) {
        
        key = server->arg(i);

    } else if (server->argName(i) == "scroll") {
        
        if (server->arg(i) == "1") {
            scrolled = 1;
        } else if (server->arg(i) == "-1") {
            scrolled = -1;
        } else scrolled = 0;
        
    } 
  } 
  
  if (text.length() > 0) {

      if (showTimes <= -1000) {
        showTimes = defaultShowTimes;
      }

      if (duration <= -1000) {
        duration = defaultDuration;
      }
      
      env->addNotification(text, duration, showTimes, scrolled, key);
  }
  
    server->send(200, "application/json", "{\"result\":\"ok\"}"); 
}

void WebServerLed::apiDeleteNotification() {

    int index = -1;
    String key = "";

    String status = "fail";

    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i) == "index") {
            
            if(sscanf(server->arg(i).c_str(), "%d", &index) != 1) {
                index = -1;  
            }
        } else if (server->argName(i) == "key" && server->arg(i).length() <= 64) {
            
            key = server->arg(i);
        }
    }

    if (key.length() > 0) {
        index = env->getNotificationIndexByKey(key);
    }

    if (index >= 0) {
        status = env->deleteNotification(index) ? "ok" : "fail";
    }  

    server->send(200, "application/json", "{\"status\":\"" + status + "\",\"notifications\":" + env->getNotificationsJSON() + "}");    
}

void WebServerLed::apiDirectInput() {

    String modeName = "";  
    tMode newMode = modeDirectWrite;

    env->setMode(newMode);
    env->modeTimerCurrent = 86400;
   
    server->send(200, "application/json", "{\"result\":\"ok\"}"); 
}

void WebServerLed::switchMode() {

    String modeName = "";  
    tMode newMode = modeNun;
    int newModeTime = 4;

    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i) == "mode") {
            
            String modeName = server->arg(i); 
            
            Serial.println("mode : " + server->arg(i));
            
            newMode = env->getModeByTextKey(modeName);

        } else if (server->argName(i) == "time") {
           
            newModeTime = 4;
            if(sscanf(server->arg(i).c_str(), "%d", &newModeTime) != 1 || newModeTime > 86400 || newModeTime <= 0) {
                newModeTime = 4;
            }

            Serial.println("time : " + newModeTime);
        }   
    } 

    if (newMode != modeNun) {
        env->setMode(newMode);
        env->modeTimerCurrent = newModeTime;
    } 
   
    server->send(200, "application/json", "{\"result\":\"ok\"}"); 
}