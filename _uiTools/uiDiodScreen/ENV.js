// ftype - input field type - bool, action, text (default)

var ENVDATA = {
    
    cfg : {
        
      "_wifi" : null,
          
          "wifiNetwork": {"filter" : "string"},      
          "wifiPassword": {"secret" : true,},
          "wifiNetworkFallback": {"filter" : "string"},      
          "wifiPasswordFallback": {"secret" : true,},
          
      "_/wifi" : null,
      "_modes" : null,
           
          "modeDuration" : {"filter" : "int"},   
          "modeList": {"filter" : "string"},
          "modeListEnabled": {"filter" : "string"},
          
      "_/modes" : null, 
      "_general" : null,
      
          "ntpHosts": {"filter" : "string"},
          "timezone": {"filter" : "string"},   
          "timestamp" : {"filter" : "string"}, 
          "tempOffset" : {"filter" : "string"},   
          
      "_/general" : null,  
      
/*
      "_ntp" : null,      
      
           "ntpHosts": {"filter" : "string"},
           
      "_/ntp" : null,      
      "_external": null,      
      
           "externalHosts": {"filter" : "string"},
        
      "_/external" : null,  
*/      
      "_mqtt" : null,
          
          "mqttHost": {"filter" : "string"},      
          "mqttPort": {"filter" : "int" },
          "mqttLogin": {"filter" : "string"},      
          "mqttPassword": {"secret" : true, "filter" : "string"},
          "mqttPrefix": {"filter" : "string"},      
          "mqttPrefixIn": {"filter" : "string" },
          "mqttDevicesIds": {"filter" : "string"},      
          
      "_/mqtt" : null,      
      "_screen" : null,
      
      "screenAutoTurnOff" : {"filter" : "string", },
      "screenAutoTurnOn" : {"filter" : "int",},
      "screenContrast" : {"filter" : "int"},
      "screenRotate" : {"filter" : "int"},
      
      "_/screen" : null,
      "_device" : null,
      
      /*
          "showText" : {"ftype" : "action"},
          "showLog" : {"ftype" : "action"},
      */
          "reboot" : {"ftype" : "action"},
          "restore" : {"ftype" : "action"},
          
      "_/device" : null,
  },   
};  