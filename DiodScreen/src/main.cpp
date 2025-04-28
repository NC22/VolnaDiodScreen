#include <Arduino.h>

#include <UserDefines.h>
#include <EnvConfig.h>
#include <UserConfig.h>


#include <LedControl.h>
#include <WebServerLed.h>
#include <WifiManager.h>
#include <Clock.h>
#include <Env.h>


#ifdef WEBSOCKETPORT
#include <WebSocketsServer.h>
#endif

// init vars

#ifdef NEWPINOUT

int outData =   12;  // DIN | D6
int outClock =  14;  // CLK | D5
int outCommit = 13;  // CS  | D7

#else

int outData =   2;
int outClock =  14; // 4;
int outCommit = 0;

#endif

int intens = 2;
int posX = 1;

bool waitAnim = false;

WiFiManager wifi = WiFiManager();
LedControl screen = LedControl(outData, outClock, outCommit, 4, 1);
Clock screenClock = Clock(&screen);

#ifdef WEBSOCKETPORT
WebSocketsServer webSocket = WebSocketsServer(81);
#endif

Env env = Env(&screen, &wifi, &screenClock);

WebServerLed server = WebServerLed(&env);

// WiFiClient wMqttClient;
// PubSubClient mClient(wMqttClient);

// appEnv app;

void displayInit() {

  //screen.initDevices();

  screen.shutdown(false);
  screen.setIntensity(env.getConfig()->getInt(cScreenContrast));
  screen.setRotate(env.getConfig()->getBool(cScreenRotate));
  // screen.setRotate(false);
  screen.update();
}

// todo - заменить text на оповещения - cписок пуш уведомлений которые можно чистить \ добавлять через апи веб и там же чистить
// листаются по очереди, макс 8

void setMode(tMode newMode, tMode prevMode) {
 
    Serial.println("new mode"); 

    screenClock.stop();

    // store output to hidden buffer, for smooth transition
    if (prevMode != modeNun) screen.enableBufferOutput(true); 

    screen.clear();

    if (newMode == modeText) {
      
      Serial.println("modeText"); 
      envNote * note = &env.notifications[env.nCursor];
      uText text = screen.getUText(note->text);
      
      if (note->scroll == 1 || (note->scroll == -1 && text.pixelWidth > screen.getWidth())) {
         screen.drawStringScrolledUtext(env.textModeX,  env.textModeY, text);
      } else {
         screen.drawStringUtext(env.textModeX, env.textModeY, text);
      }
      
      //screen.drawString(2,2, "тест");

    } else if (newMode == modeTime) {

      Serial.println("modeTime"); 
      //screenClock.drawClock();
      //screen.drawString(2,1, "3345");
      screenClock.drawClock(env.textModeX, env.textModeY);
      // screen.drawStringScrolled(1,8, "20:44", 8);

    } else if (newMode == modeDate) {

      Serial.println("modeDate"); 
      screenClock.drawDate(env.textModeX, env.textModeY);

    } else if (newMode == modeIP) {

      Serial.println("modeIp");
      if (wifi.isAPmode()) {

        screen.drawStringScrolled(screen.getWidth() - 5,  env.textModeY, wifi.getIP().toString() + " - wifi lost");

      } else {

         screen.drawStringScrolled(screen.getWidth() - 5,  env.textModeY, wifi.getIP().toString());
      }
      

    } else if (newMode == modeTemp) {

      if (env.isSensorEnabled()) {
        Serial.println("---modeTemp---");     
        
        char buffer[128];
        sprintf(buffer, "%.1f %c%c", env.readTemperature(), 0xc2, 0xa1);

        String text = buffer;
        uText utext = screen.getUText(text);      
        screen.drawStringUtext(env.textModeX + 2, env.textModeY, utext); 
      } else {
        
        screen.drawStringScrolled(env.textModeX, env.textModeY, "Temp sensor lost");
        Serial.println("---no temp sensor---");     
      }      
          
    } else if (newMode == modeHum) {

      if (env.isSensorEnabled()) {
        
        Serial.println("---modeHum---");     
        
        char buffer[128];
        sprintf(buffer, "%.1f %%", env.readHumidity());

        String text = buffer;
        uText utext = screen.getUText(text);      
        screen.drawStringUtext(env.textModeX + 2, env.textModeY, utext); 

      } else {

        screen.drawStringScrolled(env.textModeX, env.textModeY, "Hum sensor lost");
        Serial.println("---no hum sensor---");     
      }      
          
    } else if (newMode == modeSleep) {

      Serial.println("[Auto] Sleep mode -__- Zzzz..."); 

    } else if (newMode == modeSleepUser) {

      Serial.println("[Manual] Sleep mode -__- Zzzz..."); 

    } else if (newMode == modeDirectWrite) {

      Serial.println("Screen direct input"); 

    } else {

      Serial.println("Unknown mode"); 
      Serial.println(newMode); 

      screen.drawStringScrolled(env.textModeX, env.textModeY, "unknown mode");
    }

    if (prevMode != modeNun) {
      screen.enableBufferOutput(false);
      screen.transitionScreen();
    }

    screen.update();
}

void configDefaultTime() {

    String defaultTimestamp = env.getConfig()->cfgValues[cTimestamp];
    if (defaultTimestamp.length() > 0) {

      struct tm tm;
      time_t t;

      if (strptime(defaultTimestamp.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
        
        Serial.println("Default time reassigned by user config - WRONG FORMAT");

      } else {

        tm.tm_isdst = -1; 
        t = mktime(&tm);
        screenClock.defaultTime = t;
        Serial.println("Default time reassigned by user config");
      }
    }

    setenv("TZ", screenClock.timezone.c_str(), 1);
    tzset();
    
    timeval tv = { screenClock.defaultTime, 0 };
    settimeofday(&tv, nullptr);
    env.ntp = false;

    Serial.println("Manually setting time");
}

bool setupNTP() {
   
   String ntpServer = env.getConfig()->cfgValues[cNtpHosts];
   if (ntpServer == "off") {

     Serial.println("No NTP servers specified, setting time by default timestamp");
     configDefaultTime();
     return false;
   }

   configTime(screenClock.timezone.c_str(), env.getConfig()->cfgValues[cNtpHosts]);

    Serial.print("Waiting for NTP time sync: ");

    int i = 0;
    time_t now = time(nullptr);
    while (now < 1000000000) {
      now = time(nullptr);
      i++;

      if (i > 60) {

        Serial.println("");
        Serial.println("Time sync failed!");
        env.addNotification("Ошибка настройки NTP сервера", 3, -2);
        
        configDefaultTime();
        return false;
      };
      
      Serial.print(".");
      delay(500);
    }

    Serial.print("NTP ready!");
    env.ntp = true;
    screenClock.defaultTime = time(nullptr);
    return true;
}

#ifdef WEBSOCKETPORT

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  // Serial.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        String test = "test";
        webSocket.sendTXT(num, test);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\r\n", num, payload);

      // send data to all connected clients
      webSocket.broadcastTXT(payload, length);
      break;
    case WStype_BIN:

      // Serial.printf("[%u] get binary length: %u\r\n", num, length);
      // hexdump(payload, length);
      webSocket.sendBIN(num, payload, length);

      if (length == 32) {
          env.getScreen()->applyBinBuffer(payload, length);
          env.getScreen()->update();
          env.modeTimerCurrent = 86400;
      }

      break;
    default:
      Serial.printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}

#endif

void setup() {

  Serial.begin(9600);
  
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH); 

  env.begin();
  env.onModeUpdate = setMode;
  // env.onTick1Timer = asyncTimer;

  // env.modeChangeInterval = 10;

  screenClock.initTime();
  displayInit();

  screen.drawStringScrolled(env.textModeX, env.textModeY, "wifi");
  screen.update();

  wl_status_t wifiStatus = wifi.connect(
    env.getConfig()->getString(cWifiNetwork), 
    env.getConfig()->getString(cWifiPassword)
  ); 

  if (wifiStatus != WL_CONNECTED && env.getConfig()->getString(cWifiNetworkFallback).length() > 0) {
      wifiStatus = wifi.connect(env.getConfig()->getString(cWifiNetworkFallback), env.getConfig()->getString(cWifiPasswordFallback)); 
      Serial.println("Fail to connect main Wifi. Fallback wifi used");
  }

  if (env.getConfig()->cfgValues[cScreenAutoTurnOff].length() > 0 && 
      env.getConfig()->cfgValues[cScreenAutoTurnOn].length() > 0) {
      if (screenClock.setSleepState(env.getConfig()->cfgValues[cScreenAutoTurnOff], true) &&
          screenClock.setSleepState(env.getConfig()->cfgValues[cScreenAutoTurnOn], false)) {
              screenClock.enableSleepControll(true);
          }
  }

  // wl_status_t wifiStatus = WL_CONNECTION_LOST;

  if (wifiStatus == WL_CONNECTED) {
     
     Serial.println("connected to Wifi - IP: " + wifi.getIP().toString());

     server.runConfigured();

     // screen.drawStringScrolled(env.textModeX, env.textModeY, "ntp");
     // screen.update();
     
     setupNTP();
     screenClock.updateTime();
     env.mqttInit();

  } else {
    
    Serial.println("fail to connect Wifi");

    env.addNotification("Ошибка подключения к WiFi" , 3, -2);

    wifi.runAsAccesspoint();
    server.runConfigured();    
    // todo - возможность задать время по умолчанию для автономного режима
  }

  Serial.println("init");
  screen.clear();
  // screen.drawString(2,2, "13:37:47");
  // screen.update();

  // delay(3000);

  env.setMode(modeIP);
  env.modeTimerCurrent = !wifi.isAPmode() ? 4 : 10;

  //setMode(modeTime);

  //delay(3000);
  //lc.clear();
  #ifdef WEBSOCKETPORT
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  #endif
}

void loop() {

 // int commandState = 0;
 //
 // commandState = analogRead(A0);
  screenClock.sleepTick();

  #ifdef WEBSOCKETPORT
    if (env.currentMode == modeDirectWrite) {
      webSocket.loop();
    }
  #endif

  server.tick();
  
  if (screenClock.sleepRequired) {

    if (env.currentMode != modeSleep && 
        env.currentMode != modeSleepUser && 
        env.currentMode != modeDirectWrite) {
        env.setMode(modeSleep);
    }

  } else {

    if (env.currentMode == modeSleep) {
      env.nextMode();
    }      
  }

  // wait for clock digit animation to finish before scroll screen to next screenbuffer on new mode
  waitAnim = false; 
  if (env.currentMode == modeTime && !screen.screenScroll.enabled) {
      waitAnim = screenClock.tick();
  } 

  screen.tick();  
  if (!waitAnim) env.tick();

  delay(10);
  //lc.drawPixel(posX, 2, true);
  //lc.update();

 
  //delay(1000);
  // posX++;
  //if (posX > 32) {
  //  posX = 1;
  //  lc.clear();
  //}

}
