#ifndef ENV_H
#define ENV_H
#include <Arduino.h>

#include <UserDefines.h>
// #include <LedControl.h>
#include <Clock.h>
#include <Wire.h>
#include <WifiManager.h>

#include <Config.h>
#include <ESP8266WebServer.h>
#include <thirdparty/PubSubClient.h>

extern const unsigned int cdConfigVersion PROGMEM;
extern const char cdWifiNetwork[] PROGMEM;
extern const char cdWifiPassword[] PROGMEM;
extern const char cdTimezone[] PROGMEM;
extern const char cdNtpHosts[] PROGMEM;
extern const bool cdRotate PROGMEM;

#ifdef Sensor_BMP180

// только температура - GY-68 aka BMP180
// adafruit/Adafruit BMP085 Library@^1.2.2
// adafruit/Adafruit BusIO@^1.14.1

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_BMP085.h> // bmp180 - сенсор температуры

#define seaLevelPressure_hPa 1013.25

#elif Sensor_CJMCU8128

// 3 в одном - температура \ +температура \ давление \ углекислый газ \ орг. вещества
// adafruit/Adafruit BMP280 Library@^2.6.8
// adafruit/Adafruit Si7021 Library@^1.5.1

#include <Adafruit_BMP280.h>
#include "Adafruit_Si7021.h"
#include "thirdparty/SparkFunCCS811.h" 

#elif Sensor_BME280

// температура \ давление

#include "thirdparty/GyverBME280.h" 

#endif

typedef struct {
  String text;
  int duration;  // -1 - 3600 \ in sec
  int showTimes; // -1 - infinit
  int showCurrent;
  int scroll; // -1 - auto
  String key;
} envNote;

// modeNun - to detect undefined \ wrong mode names in getModeByTextKey \ other methods
enum tMode { modeText, modeTime, modeTemp, modeHum, modeDate, modeIP, modeSleep, modeSleepUser, modeDirectWrite, modeNun };

class Env {
    
    private :

        Config cfg;

        LedControl * screen;
        Clock * clock;
        WiFiManager * wifi;

        unsigned int modeNum;
        String dModeList;
        
        WiFiClient _wifiClient;
        PubSubClient _mqttClient;
        std::vector<unsigned int> mqttDeviceIds;
        void mqttLoadDeviceIds();
        void mqttMessageReceivedCallback(char* topic, uint8_t* payload, unsigned int length);

        bool sensor = false;
        int testInputButtonPin = 16;
        unsigned long buttonPressed = 0;
        // unsigned long buttonPressedStart = 0;

        #ifdef Sensor_BMP180
            Adafruit_BMP085 tempSensor;
        #elif Sensor_CJMCU8128
            Adafruit_BMP280 tempSensor; // Temp | Pressure
            CCS811 ccs811; // Co2 
            Adafruit_Si7021 SI702x; // Temp | Humidity
            float lastCO2 = 0;
            float lastTVOC = 0;
        #elif Sensor_BME280
            GyverBME280 tempSensor;
        #endif

        unsigned long secondTimerStart;
        float tempOffset = 0.0;        

        void setCfgStr(char * cfg, String input);
        String readCfgStr(char * cfg);
        void initDefaultModeList();

        void tickWiFiBlocking();
        void tickSensors();
        void onButtonPress();

    public:

        Env(LedControl * ascreen, WiFiManager * awifi, Clock * aclock);
        LedControl * getScreen();
        Clock * getScreenClock();

        Config * getConfig();

        bool commitConfig();
        bool resetConfig(bool resetWifi = false);
        tMode getModeByTextKey(String & key);

        //  WiFiManager * getWiFiManager();

        void initModeList(bool restoreDefault = false);
        void begin();
        void tick();
        void restart(String reason = "not specifed", bool skipRTCSanitize = true);

        float readTemperature();
        float readPressure();
        float readHumidity();

        bool isSensorEnabled();
        void initSensors();

        String getSensorsJSON();
        String getNotificationsJSON();

        void validateConfig(unsigned int version = -1, std::vector<cfgOptionKeys> * updatedKeys = NULL);

        int addNotification(String text, int duration = 3, int times = 3, int scroll = -1, String key = "");
        int getNotificationIndexByKey(String key);
        bool deleteNotification(unsigned int index);

        void setMode(tMode newMode);
        void nextMode();
        void log(String notice);

        int textModeX = 2;
        int textModeY = 1;

        unsigned int defaultInterval = 10; // todo - add to config
        unsigned int modeChangeInterval = defaultInterval; // current interval
        unsigned int modeTimerCurrent;
        unsigned int mqttTimerCurrent = 3; // wifi reattempt & mqtt timer

        bool modeChange = true;

        std::function<void(tMode newMode, tMode prevMode)> onModeUpdate;
        std::function<void()> onTick1Timer = NULL; // 1 second timer events

        // load all possible modes in initDefaultModeList, then exclude disabled by user
        std::vector<tMode> modeListR;

        unsigned int nCursor = 0;
        std::vector<envNote> notifications;

        // todo - take mode from modelist based on user config - modeListR 
        // tMode modeOrder[4] = {modeTemp, modeText, modeTime, modeDate};
        tMode currentMode = modeNun;

        void mqttInit();
        void mqttSync();
        
        bool ntp = false;
        bool mqtt = false;
};


#endif

