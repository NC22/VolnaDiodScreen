#include <Env.h>

Env::Env(LedControl * ascreen, WiFiManager * awifi, Clock * aclock) {
    
    screen = ascreen;

    wifi = awifi; 
    wifi->onConnectWaitTick = std::bind(&Env::tickWiFiBlocking, this);

    clock = aclock;
    // cfg = Config();


/*
    if (!cfg.set(cWifiNetwork, "testetst")) Serial.println("testUnexu err");
    if (!cfg.set(cTempOffset, "-1.556")) Serial.println("tempOffset err");
    if (!cfg.set(cNtpHosts, "1tyyyy")) Serial.println("ntpHosts err");
    if (!cfg.set(cScreenContrast, "oooommll0gerte")) Serial.println("screenContrast err");
    if (cfg.set(cMqttPassword, "oooommll0gerte")) Serial.println("mqttPassword OKK");
    if (cfg.set(cWifiPassword, "testetst")) Serial.println("wifiPassword OKK");

    unsigned int t = 0x20;
    String testPass = " tte";
           testPass += t;  testPass += t; testPass += t; testPass += t;
           
    if (!cfg.set(cMqttPassword, testPass)) Serial.println("mqttPassword FAIL - found hide characters");

    cfg.set(cMqttHost, "1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy1tyyyy77"); 
    cfg.set(cNtpHosts, "");
    int cfgSize = sizeof(cfgOptions) / sizeof(cfgOptions[0]);    
    for(int i=0; i < cfgSize; i++) {

        Serial.println(cfg.getKeyName(cfgOptions[i].key) + " | " + cfg.getString(cfgOptions[i].key).c_str());
    }
*/
}

void Env::begin() {
    // load cfg & validate config
    // cfg.test();

    cfg.version = cdConfigVersion;
    
    if (testInputButtonPin > -1) {
        if (testInputButtonPin == 16) {
            pinMode(testInputButtonPin , INPUT_PULLDOWN_16);
        } else {
            pinMode(testInputButtonPin , INPUT_PULLUP);
        }

        log("Test button enabled PIN : " + String(testInputButtonPin));
    } 

    initSensors();

    int loadedVersion = cfg.loadEEPROM();
    //if (loadedVersion == 1001) resetConfig(fasle);

    validateConfig(loadedVersion);    
}

bool Env::commitConfig() {

    cfg.commitEEPROM();
    return true;
}

bool Env::deleteNotification(unsigned int index) {

    if (index < notifications.size()) {

        log("deleted notification " + String(index));
        notifications.erase(notifications.begin() + index);

        if (currentMode == modeText && nCursor == index) {
            log("deleted notification currently shown -> goto next");
            nextMode();
        }

        return true;
    } else {
        return false;
    }
}

int Env::getNotificationIndexByKey(String key) {
   
    unsigned int length = notifications.size();
    if (length && key.length() > 0) {
        for (unsigned int i = 0; i < length; i++) {
            if (notifications[i].key.length() > 0 && notifications[i].key.equals(key)) {
                return i;
                break;
            }
        }
    }

    return -1;
}

int Env::addNotification(String text, int duration, int times, int scroll, String key) {

    if (text.length() <= 0) return -1;

    bool system = false;
    unsigned int length = notifications.size();
    int updateIndex = getNotificationIndexByKey(key);

    if (duration <= -1) duration = 3;
    if (duration > 86400) duration = 86400;

    if (times <= -2) system = true;
    if (times <= -1) times = 3;
    if (times > 900) times = 900;
    if (scroll < -1 || scroll > 1) scroll = -1;
    
    if (length >= 5 && updateIndex == -1) { // todo check is index changes

       notifications.erase(notifications.begin() + 0);
    } 

    envNote newNote;
            newNote.text = text;
            newNote.duration = duration;
            newNote.showTimes = times;
            newNote.showCurrent = 0;
            newNote.key = key;
            newNote.scroll = scroll;

    if (updateIndex != -1) {

        notifications[updateIndex].text = text;
        notifications[updateIndex].showCurrent = 0;
        nCursor = updateIndex;

    } else {
        notifications.push_back(newNote);
        nCursor = notifications.size()-1;
    }

    if (system) setMode(modeText);
    return nCursor;
}

void Env::restart(String reason, bool skipRTCSanitize) {

    Serial.println("[RESTART] Reason : " + reason);
        
    delay(500);
    ESP.restart();
}

bool Env::resetConfig(bool resetWifi) {

    String wifiNetwork = cfg.getString(cWifiNetwork);
    String wifiPass = cfg.getString(cWifiPassword);

    cfg.reset();

    if (!resetWifi) {
        cfg.set(cWifiNetwork, wifiNetwork);
        cfg.set(cWifiPassword, wifiPass);
    }

    validateConfig();
    commitConfig();

    return true;
}

void Env::validateConfig(unsigned int version, std::vector<cfgOptionKeys> * updatedKeys) {
    
    if (version <= 0) {
        version = cdConfigVersion;
    }

    if (cfg.cfgValues[cWifiNetwork].length() <= 0) {
        cfg.cfgValues[cWifiNetwork] = FPSTR(cdWifiNetwork);
        cfg.cfgValues[cWifiPassword] = FPSTR(cdWifiPassword);
    }

    if (cfg.cfgValues[cMqttPrefix].length() <= 0) {
        cfg.cfgValues[cMqttPrefix] = "domoticz/out";
    }

    if (cfg.cfgValues[cMqttPrefixIn].length() <= 0) {
        cfg.cfgValues[cMqttPrefixIn] = "domoticz/in";
    }

    if (cfg.cfgValues[cMqttDevicesIds].length() <= 0) {
        cfg.cfgValues[cMqttDevicesIds] = "31";
    }

    if (cfg.cfgValues[cMqttPort].length() <= 0) {
        cfg.cfgValues[cMqttPort] = "1883";
    }

    mqttLoadDeviceIds();    

    if (cfg.cfgValues[cTimezone].length() <= 0) {
        cfg.cfgValues[cTimezone] = FPSTR(cdTimezone);
    }  

    if (cfg.cfgValues[cNtpHosts].length() <= 0) {
       cfg.cfgValues[cNtpHosts] = FPSTR(cdNtpHosts);
    } 

    int tmp = cfg.getInt(cScreenContrast);
    if (tmp <= 0 || tmp > 16) cfg.cfgValues[cScreenContrast] = "2";

    tmp = cfg.getInt(cModeDuration);  
    if (tmp <= 0) {
        cfg.cfgValues[cModeDuration] = defaultInterval;
    }

    defaultInterval = cfg.getInt(cModeDuration);

    if (cfg.cfgValues[cScreenRotate] != "1" && cfg.cfgValues[cScreenRotate] != "0") {
        cfg.cfgValues[cScreenRotate] = cdRotate ? "1" : "0";
    }

    if (cfg.cfgValues[cTempOffset].length() > 0) {

        tempOffset = cfg.getFloat(cTempOffset);
        if (cfg.sanitizeError) tempOffset = 0.0;

    } else {
        tempOffset = 0.0;
    }

    initModeList();

    if (updatedKeys != NULL) {
       
       Serial.println("[updatedKeys]  Size : " + String(updatedKeys->size()));

       for (unsigned int i = 0; i < updatedKeys->size(); i++)  {

            if (updatedKeys->at(i) == cScreenContrast) {
                getScreen()->setIntensity(getConfig()->getInt(cScreenContrast));
            } else if (updatedKeys->at(i) == cScreenRotate) {
                getScreen()->setRotate(getConfig()->getBool(cScreenRotate));
            }
            
       }
    }
}

tMode Env::getModeByTextKey(String & key) {
    
    tMode testMode = modeNun;
    if (key.length() <= 0) return testMode;

    if (key == "date") {
        testMode = modeDate;
    } else if (key == "time") {
        testMode = modeTime;
    } else if (key == "temp") {
        testMode = modeTemp;
    } else if (key == "text") {
        testMode = modeText;
    } else if (key == "hum") {
        testMode = modeHum;
    } else if (key == "ip") {
        testMode = modeIP;
    } else if (key == "sleep") {
        testMode = modeSleepUser;
    } 

    return testMode;
}

void Env::initDefaultModeList() {

    modeNum = 3;
    dModeList = "text,date,time";

    if (isSensorEnabled()) {
        dModeList += ",temp";
        modeNum++;

        #if defined(Sensor_CJMCU8128) || defined(Sensor_BME280)
            dModeList += ",hum";
            modeNum++;
        #endif
    }
}

void Env::initModeList(bool restoreDefault) {

    initDefaultModeList();
    unsigned int length = cfg.cfgValues[cModeList].length();    
    
    log("initModeList [string size : " + String(length) + "]"); 

    if (restoreDefault || length <= 0) {
        cfg.cfgValues[cModeList] = dModeList;
        length = cfg.cfgValues[cModeList].length();
    }

    String modeName = "";
    String modeListEnabled = "";
    unsigned int parsedModesNum = 0;
    modeListR.clear();

    for (unsigned int i = 0; i < length; i++) {
          
          if (cfg.cfgValues[cModeList][i] == ',' || i >= length-1) {

            if (i >= length-1) modeName += cfg.cfgValues[cModeList][i];
            
            Serial.println(modeName);
            tMode testMode = getModeByTextKey(modeName);

            if (testMode != modeNun) {

                if (cfg.cfgValues[cModeListEnabled].length() - 1 >= parsedModesNum && 
                    cfg.cfgValues[cModeListEnabled][parsedModesNum] == '0') {

                        log("Skip mode : " + modeName); 
                        modeListEnabled += "0";

                } else {
                    modeListEnabled += "1";
                    modeListR.push_back(testMode);
                } 

                parsedModesNum++;
            }
            
            modeName = "";
            
          } else {
          
            modeName += cfg.cfgValues[cModeList][i];
          }
    }

    if (!restoreDefault && parsedModesNum != modeNum) {
        log("Restore default list | Modes in cfg : " + String(modeListR.size()) + ", " + String(modeNum)); 
        initModeList(true);
        return;
    }    

    if (modeListR.size() <= 0) {
        log("Empty modeList - set clock as default"); 
        modeListR.push_back(modeTime);
    }

    cfg.cfgValues[cModeListEnabled] = modeListEnabled;
    log("mode list loaded from config [Size: " + String(modeListR.size()) + "]"); 
}

void Env::log(String notice) {

    Serial.println(notice); 
}


bool Env::isSensorEnabled() {
    return sensor;
}

void Env::initSensors() {

    Wire.begin(4, 5); 
    bool error = false;

    #if defined(Sensor_BMP180)

        if (!tempSensor.begin()) {
            error = true;
            Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        } 

    #elif defined(Sensor_BME280)

        if (!tempSensor.begin(0x76)) {
            error = true;
            Serial.println("Could not find a valid BME280 sensor, check wiring!");
        } 

    #elif defined(Sensor_CJMCU8128)
    
        if (!tempSensor.begin(0x76)) {
            error = true;
            Serial.println("Could not find BMP280 sensor, check wiring!");
        } 

        if (!SI702x.begin()) {
            Serial.println("Did not find Si702x sensor!");
        } 
        
        ccs811.setI2CAddress(0x5A);
        if (!ccs811.begin()) {
            Serial.println("Failed to start ccs811 sensor!");            
        } 

    #else
        Serial.println("No any temperature sensor defined");
        return;
    #endif

    if (!error) {
        sensor = true;
        Serial.println("Temperature sensor [OK]");
    }
}

String Env::getNotificationsJSON() {

    unsigned int length = notifications.size();

    if (length <= 0) return "[]";

    String json = "[";

    for (unsigned int i = 0; i < length; i++) {
        json += "{\"id\":\"" + String(i) + "\",\"text\":\"" + notifications[i].text + "\",\"d\":" + String(notifications[i].duration) + ",\"t\":" + String(notifications[i].showTimes) + ",\"tc\":" + String(notifications[i].showCurrent) + ",\"s\":" + String(notifications[i].scroll) + "}";
        if (i < length-1) json += ',';
    }

    json += "]";

    return json;
}

String Env::getSensorsJSON() {

    if (!isSensorEnabled()) return "{}";

    std::vector<String> paramList;

    #if defined(Sensor_BMP180)

        paramList.push_back("\"temperature\":" + String(readTemperature()));
        paramList.push_back("\"pressure\":" + String(readPressure()));

    #elif defined(Sensor_CJMCU8128)    
      
        paramList.push_back("\"temperature\":" + String(readTemperature()));
        paramList.push_back("\"pressure\":" + String(readPressure()));
        paramList.push_back("\"humidity\":" + String(readHumidity()));
        paramList.push_back("\"temperature__2\":" + String(SI702x.readTemperature()));
        paramList.push_back("\"co2\":" + String(lastCO2));
        paramList.push_back("\"tvoc\":" + String(lastTVOC));

    #elif defined(Sensor_BME280)    
      
        paramList.push_back("\"temperature\":" + String(readTemperature()));
        paramList.push_back("\"pressure\":" + String(readPressure()));
        paramList.push_back("\"humidity\":" + String(readHumidity()));

    #else
        return "{}";
    #endif

    String json = "{";

    unsigned int pSize = paramList.size();
    for (unsigned int i = 0; i < pSize; i++) {
        json += paramList[i];
        if (i < pSize-1) json += ',';
    }

    json += "}";

    return json;
}

void Env::tickWiFiBlocking() {
    screen->tick();
}

void Env::tickSensors() {

    // log("tick sensors");

    #if defined(Sensor_CJMCU8128)
      
      if (ccs811.dataAvailable()) {
        lastCO2 = ccs811.getCO2();
        lastTVOC = ccs811.getTVOC();
        ccs811.readAlgorithmResults();
        ccs811.setEnvironmentalData(tempSensor.readTemperature(), SI702x.readHumidity()); 
      }

    #endif
}

float Env::readTemperature()  {
    
    #if defined(Sensor_BMP180) || defined(Sensor_CJMCU8128) || defined(Sensor_BME280)
       return tempSensor.readTemperature() + tempOffset;
    #endif

    return -1000;
}

float Env::readPressure() {

    #if defined(Sensor_BMP180) || defined(Sensor_CJMCU8128) || defined(Sensor_BME280)
        return tempSensor.readPressure();
    #endif

    return -1000;
}

float Env::readHumidity() {
    
    #ifdef Sensor_BMP180
        
        return -1000;

    #elif Sensor_CJMCU8128

        return SI702x.readHumidity();
        
    #elif Sensor_BME280

        return tempSensor.readHumidity();

    #endif
}

void Env::tick() {

    unsigned long now = millis();
    if (now - secondTimerStart >= 1000) {
        
        secondTimerStart = now;  
        modeTimerCurrent--;
        mqttTimerCurrent--;

        tickSensors();        

        if (onTick1Timer != NULL) {
            onTick1Timer();
        }
    }

    if (testInputButtonPin > -1) {
                
        // if (buttonPressed > 0) {
            // buttonPressed += (now - buttonPressedStart);
            // buttonPressedStart = now;
        // }

        if (buttonPressed >= 1 && digitalRead(testInputButtonPin) == LOW) {
            
            //if (buttonPressed >= 3000) {
            //    
            //    setMode(modeSleepUser);
            //    modeTimerCurrent = 3600;
            
            onButtonPress();
            buttonPressed = 0;

        } else if (buttonPressed <= 0 && digitalRead(testInputButtonPin) == HIGH) {
            buttonPressed = 1;
            //buttonPressedStart = now;
        } 
    }

    if (mqtt) {
        _mqttClient.loop();
    }

    if (mqtt && mqttTimerCurrent <= 0) {

        wifi->updateStatus();
        mqttTimerCurrent = 10;

        if (wifi->lastConnectStatus != WL_CONNECTED) {

            log("WiFi connection lost, attempt to reconect");

            wifi->connect(
                getConfig()->getString(cWifiNetwork), 
                getConfig()->getString(cWifiPassword)
            );
        }

        mqttSync();
    }

    if (modeChange && modeTimerCurrent <= 0) {
        nextMode();
    }    
}

void Env::onButtonPress() {

    if (currentMode == modeText) {
        deleteNotification(nCursor);
    } else {
        nextMode();
    }

}

void Env::mqttMessageReceivedCallback(char* topic, uint8_t* payload, unsigned int length)
{
 
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void Env::mqttInit() {
    
    if (getConfig()->cfgValues[cMqttHost].length() > 0) {

        _mqttClient.setClient(_wifiClient);
        _mqttClient.setServer(getConfig()->cfgValues[cMqttHost].c_str(), getConfig()->getInt(cMqttPort)); 
        _mqttClient.setCallback([this](char* topic, uint8_t* payload, unsigned int length) {this->mqttMessageReceivedCallback(topic, payload, length);});

        mqtt = true;
        Serial.println("MQTT Enabled ..."); 
    }
}

void Env::mqttLoadDeviceIds() {
    
    mqttDeviceIds.clear();

    if (cfg.cfgValues[cMqttHost].length() > 0) {

        unsigned int maxDevices = 3; // virtual devices data items
        std::vector<String> rawMqttIds;
        cfg.getStringList(cfg.cfgValues[cMqttDevicesIds], rawMqttIds, ',', maxDevices);

        unsigned int rawMaxIndex = rawMqttIds.size()-1;
        for (unsigned int i = 0; i < maxDevices; i++) {

            if (i > rawMaxIndex) {
                mqttDeviceIds.push_back(0);
            } else {
                unsigned int testMqttId = cfg.sanitizeInt(rawMqttIds[i]);

                if (cfg.sanitizeError) mqttDeviceIds.push_back(0);
                else mqttDeviceIds.push_back(testMqttId);
            }

            log(String(mqttDeviceIds[i]));
        }

        log("Ids length : " + String(rawMaxIndex) + " ----- " + String(mqttDeviceIds.size()));
    }
}

void Env::mqttSync() {
    
    if (!mqtt) return;
    if (wifi->lastConnectStatus != WL_CONNECTED) return;

    log("MQTT check connection..."); 

    if (!_mqttClient.connected()) {
        
        log("generate clientId");
        // Create a random client ID
        String clientId = "ESP8266ClockClient-";
        clientId += String(random(0xffff), HEX);

        log(clientId);

        bool result = false;
        if (cfg.cfgValues[cMqttLogin].length() > 0) {
            log("MQTT connect... with pass"); 
            result = _mqttClient.connect(clientId.c_str(), cfg.cfgValues[cMqttLogin].c_str(), cfg.cfgValues[cMqttPassword].c_str());
        } else {
            log("MQTT connect... anon"); 
            result = _mqttClient.connect(clientId.c_str());
        }

        if (result) {

            Serial.println("connected");
            // client.subscribe("inTopic");

        } else {
            Serial.print("failed, rc=");
            Serial.print(_mqttClient.state());
        }
  
    } else {
        log("MQTT already connected..."); 
    }

    if (_mqttClient.connected()) {

        log("MQTT test send..."); 

        // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT;BAR;BAR_FOR
        // HUM 0=Normal 1=Comfortable 2=Dry 3=Wet
        // BAR_FOR 0 = No Info 1 = Sunny 2 = Partly Cloudy 3 = Cloudy 4 = Rain
        // /json.htm?type=command&param=udevice&idx=<idx>&nvalue=<numeric value>&svalue=<string value>&rssi=<numeric value>&battery=<numeric value>
        // battery level 255 = no battery device, else 0-100
        // rssi=10&battery=89 signal level (default 12) 
        // https://www.domoticz.com/wiki/Domoticz_API/JSON_URL%27s

        float h = readHumidity();
        unsigned int hStat = 0;
        if (h <= 20) hStat = 2;
        else if (h >= 40 && h <= 60) hStat = 1;
        else if (h > 60) hStat = 3;

        unsigned int devicesNum = mqttDeviceIds.size();
        String tmp;
        String metric = String(readTemperature());

        if (devicesNum >= 3 && mqttDeviceIds[2] > 0) { // temp

            log("MQTT test send... [3]"); 
            tmp = "{\"idx\":" + String(mqttDeviceIds[2]) + ",\"nvalue\":0,\"svalue\":\"" + metric + "\"}";
            _mqttClient.publish("domoticz/in", tmp.c_str());
        }

        metric += ";" + String(h) + ";" + String(hStat);
        if (devicesNum >= 2 && mqttDeviceIds[1] > 0) { // temp & hum

            log("MQTT test send... [2]");
            tmp = "{\"idx\":" + String(mqttDeviceIds[1]) + ",\"nvalue\":0,\"svalue\":\"" + metric + "\"}";
            _mqttClient.publish("domoticz/in", tmp.c_str());
        }

        metric += ";" + String((float) (readPressure() / 100.0)) + ";0";
        if (devicesNum >= 1 && mqttDeviceIds[0] > 0) { // temp & hum & bar

            log("MQTT test send... [1]");
            tmp = "{\"idx\":" + String(mqttDeviceIds[0]) + ",\"nvalue\":0,\"svalue\":\"" + metric + "\"}";
            _mqttClient.publish("domoticz/in", tmp.c_str());
        }
    }
}

void Env::nextMode() {

    if (currentMode == modeSleep && clock->sleepRequired) {
        
        log("modeSleep - nextMode skip. Keep sleep until wake up...");
        setMode(modeSleep);
        return;

    } else if (currentMode == modeText) {

        unsigned int maxSize = notifications.size();
        if (maxSize > 0) {

            envNote * note = &notifications[nCursor];

            log("modeText - end cycle [" + String(nCursor) + "]");

            if (note->showTimes != -1 && note->showCurrent >= note->showTimes) {
                notifications.erase(notifications.begin() + nCursor);
                maxSize--;
                log("modeText - delete note [" + String(nCursor) + "] by cycle counter");
            }

            nCursor++;

            if (nCursor > maxSize-1) {
                nCursor = 0;
                log("modeText - all notifications showed, go to next mode");
            } else {
                log("modeText - goto next notification");
                setMode(modeText);
                return;
            }
        }
    }

    int modeOrderSize = modeListR.size(); // sizeof(modeOrder) / sizeof(modeOrder[0]);

    int newMode = 0;
    while (newMode < modeOrderSize)
    {
        if (currentMode == modeListR[newMode]) { // if (currentMode == modeOrder[newMode]) {
            break;
        }

        newMode++;
    }

    log("newMode [INDEX: " + String(newMode) + "]"); 

    if (newMode < modeOrderSize) {
      newMode++;
    }

    if (newMode >= modeOrderSize) {
       newMode = 0;
    }

    setMode(modeListR[newMode]);
    // setMode(modeOrder[newMode]);
}

Config * Env::getConfig() {
    return &cfg;
}

void Env::setMode(tMode mode) {
    
    if (mode == modeText) {

        if (notifications.size() <= 0) {
            log("Skip modeText - no any notifications added"); 
            currentMode = modeText;
            nextMode();
            return;
        }

        log("modeText - show notificcation [" + String(nCursor) + "]");

        if (notifications[nCursor].duration <= -1) {
            modeChangeInterval = 3600;
        } else {
            modeChangeInterval = notifications[nCursor].duration;
        }

        if (notifications[nCursor].showTimes != -1) {
            notifications[nCursor].showCurrent++;        
        }

    } else {

        modeChangeInterval = defaultInterval;
    }

    onModeUpdate(mode, currentMode);

    modeTimerCurrent = modeChangeInterval;
    currentMode = mode;
}

Clock * Env::getScreenClock() {
    return clock;
}

LedControl * Env::getScreen() {
    return screen;
}