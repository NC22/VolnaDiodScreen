#include <EnvConfig.h>

const cfgOption cfgOptions[] = { 
    
    {cWifiNetwork, ctypeString, false, "wifiNetwork", true},
    {cWifiPassword, ctypeString, true, "wifiPassword", true},
    
    {cTimezone, ctypeString, false, "timezone", true},    
    {cTempOffset, ctypeFloat, false, "tempOffset", false},

    {cModeDuration, ctypeInt, false, "modeDuration", false},
    {cModeList, ctypeString, false, "modeList", false},
    {cModeListEnabled, ctypeString, false, "modeListEnabled", false},
    
    {cNtpHosts, ctypeString, false, "ntpHosts", true},
    {cExternalHosts, ctypeString, false, "externalHosts", true},
    
    {cMqttHost, ctypeString, false, "mqttHost", true},
    {cMqttPort, ctypeInt, false, "mqttPort", true},
    {cMqttLogin, ctypeString, false, "mqttLogin", true},
    {cMqttPassword, ctypeString, true, "mqttPassword", true},
    {cMqttPrefix, ctypeString, false, "mqttPrefix", false},
    {cMqttPrefixIn, ctypeString, false, "mqttPrefixIn", false},
    {cMqttDevicesIds, ctypeString, false, "mqttDevicesIds", false},

    {cScreenAutoTurnOff, ctypeString, false, "screenAutoTurnOff", false},
    {cScreenAutoTurnOn, ctypeString, false, "screenAutoTurnOn", false},
    {cScreenContrast, ctypeInt, false, "screenContrast", false},
    {cScreenRotate, ctypeBool, false, "screenRotate", false},

    {cTimestamp, ctypeString, false, "timestamp", false},
    
    {cWifiNetworkFallback, ctypeString, false, "wifiNetworkFallback", false},
    {cWifiPasswordFallback, ctypeString, false, "wifiPasswordFallback", false},
};

const size_t cfgOptionsSize PROGMEM = sizeof(cfgOptions) / sizeof(cfgOptions[0]);


