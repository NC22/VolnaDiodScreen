#ifndef EnvCfgTypes_h
#define EnvCfgTypes_h

enum cfgOptionType { ctypeString, ctypeBool, ctypeInt, ctypeFloat};

enum cfgOptionKeys {

    cWifiNetwork,
    cWifiPassword,
    cTimezone,
    cTempOffset,

    cModeDuration,
    cModeList, 
    cModeListEnabled,
        
    cNtpHosts,
    cExternalHosts,
    
    cMqttHost,
    cMqttPort,
    cMqttLogin,
    cMqttPassword,
    cMqttPrefix,
    cMqttPrefixIn,
    cMqttDevicesIds,

    cScreenAutoTurnOff,
    cScreenAutoTurnOn,
    cScreenContrast,
    cScreenRotate,

    cTimestamp,
    
    cWifiNetworkFallback,
    cWifiPasswordFallback,
    
    cNull,
};

typedef struct {
  cfgOptionKeys key;
  cfgOptionType type;
  bool secured;
  const char * keyStr;
  bool rebootRequired;
} cfgOption;

#endif