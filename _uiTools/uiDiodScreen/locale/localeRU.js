var ENVLOCALE = {
   
    "cfg__wifi" : "Доступ к WiFi",
    "cfg_wifiNetwork" : "Название сети (SSID)",
    "cfg_wifiPassword" : "Пароль",
    "cfg_wifiNetworkFallback" : "[Резервный] Название сети (SSID)",
    "cfg_wifiPasswordFallback" : "[Резервный] Пароль",
    "cfg__general" : "Общие настройки",
    "cfg__general_help" : "Формат поля даты и времени : 2024-05-01 13:37:00",
    "cfg_timezone" : "Часовой пояс",
    "cfg_toFahrenheit" : "Единицы измерения температуры (0 - Цельсий, 1 - Фаренгейт)",
    "cfg_timeFormat12" : "12-часовой формат (0-1)",
    "cfg_updateMinutes" : "Обновлять минуты (0-1 или кол-во секунд)",    
    "cfg_titleIndoor" : "Заголовок датчика помещения",
    "cfg_titleExternal" : "Заголовок внешнего датчика",
    "cfg_timestamp" : "Дата & Время",
    "cfg_tempOffset" : "Корректировка температуры (от -10℃ до 10℃)",  
    "cfg_ntpHosts" : "NTP сервер (off - не использовать)",

    "cfg__extSensor" : "Внешний датчик (Domoticz & HA & OpenWeather)",      
    "cfg__extSensor_help" : "<b>DZ (Domoticz)</b><br>http:/" + "/[IP или домен сервера]:8080/json.htm?type=devices&rid=[ID устройства]<br>Опционально требуется доступ по логину и паролю, можно создать пользователя в разделе Настройка -> Users (/#/Users)<br><br><b>HA (Home Assistant)</b><br>http:/" + "/[IP или домен сервера]:8123/api/states/[ID устройства]<br>Токен можно получить в HA в разделе Безопасность (/profile/security) -> Долгосрочные токены доступа",
    "cfg_extSensorLink" : "Url-адрес",
    "cfg_extSensorLogin" : "Логин",
    "cfg_extSensorPassword" : "Пароль или Token",

    "cfg__mqtt" : "MQTT сервер",
    "cfg__mqtt_help" : "Порядок проверки настроек [HA] -> [DZ]<br>Заполните поля в зависимости от серверного ПО<br>Для Domoticz укажите Префикс IN : <b>domoticz/in</b> и создайте виртуальные сенсоры и укажите их id здесь.<br> Для HA<br>1. Укажите Discovery префикс для автообнаружения (обычно по умолчанию <b>homeassistant</b>)<br>2. Придумайте уникальный идентификатор устройства в системе и имя устройства. Например : <b>kelly_einkscreen270524, Kelly E-Ink Screen</b>",
    "cfg_mqttHost" : "Адрес сервера",
    "cfg_mqttPort" : "Порт",
    "cfg_mqttLogin" : "Логин",
    "cfg_mqttPassword" : "Пароль",
    "cfg_mqttPrefixIn" : "[DZ] Префикс Topic In",
    "cfg_mqttDevicesIds" : "[DZ] Id устройств (T&H&B, T&H, T)",
        
    "cfg_mqttHAPrefix": "[HA] Префикс Discovery",
    "cfg_mqttHADevice": "[HA] Device id, Device name",  

    "cfg_sleepTime" : "Глубокий сон экрана (1-30 минут)",
    "cfg_syncEvery" : "Синхронизация после n кол-ва пробуждений (1-30)",

    "cfg__screen" : "Управление экраном",
    "cfg_screenRotate" : "Перевернуть экран (0-1)",
    "cfg_screenLandscape" : "Альбомный режим (0-1)",

    "cfg__imageFeed" : "Свое оформление",
    "cfg_imagePreset" : "Пресет (-loop - по порядку)",

    "form_text_send" : "Отправить",   

    "help" : "Подсказка",
    "title" : "LED Часы Волна",
    "wifiSearch" : "Поиск WiFi сети",
    "save" : "Сохранить изменения",
    "save_skip" : "Без записи в постоянную память",

    "cfg__device" : "Управление устройством",

    "reboot" : "Перезагрузить",
    "resetntp" : "Перезапустить синх. времени (NTP)",
    "restore" : "Сбросить настройки",    
    "refresh" : "Обновить экран",
    "clear" : "Очистить экран",

    "imageUpload" : "Загрузить свою картинку",

    "downloadBuffer1" : "Скачать основной буфер экрана",
    "downloadBuffer2" : "Скачать вспомогательный буфер",  

    "show_wifi_list" : "Показать список WiFi сетей",
    "reboot_ok" : "Перезагрузка выполняется",
    "command_ok" : "Команда отправлена успешно",
    "conn_lost" : "Устройство не доступно",

    "notifications_empty" : "Оповещений нет",

    "commit_reset_confirmed" : "Выполняю сброс настроек",
    "commit_reset_confirm" : "Настройки будут сброшены. Для подтверждения <b>нажмите на кнопку сброса еще раз</b>",
    "commit_reset_ok" : "Настройки сброшены",
    "commit_nothing" : "Нет изменений",
    "commit_save" : "Сохраняю изменения...",
    "commit_save_ok" : "Изменения сохранены.",      
    "commit_reboot_required" : "Требуется перезагрузка устройства",

    "getinfo_fail" : "Ошибка обновления",
    "getinfo_sensors_empty" : "Датчики не инициализированы",
    "getinfo_sensors" : "Датчики",
    "getinfo_sensor" : "датчик",
    "getinfo_sensor_pres" : "Давление",
    "getinfo_sensor_temp" : "Температура",
    "getinfo_sensor_hum" : "Влажность",
    "getinfo_sensor_co2" : "Углекислый газ (CO2)",
    "getinfo_sensor_loc" : "Орг. вещества TVOC (ЛОС)",
    "getinfo_sensor_weather_state" : "Статус погоды",
    "getinfo_wifi_lvl" : "Уровень сигнала",
    "getinfo_wifi_fail" : "Точка не доступна",
    "getinfo_wifis1" : "Сбой подключения",
    "getinfo_wifis2" : "Неправильный пароль",
    "getinfo_wifis3" : "Нет сети с указанным именем",

    "getinfo_external_sens_sync_times" : "Успешных подключений",
    "getinfo_external_sens_sync_last" : "Дата последней синхронизации",
    "getinfo_external_sens" : "Внешний датчик",
    "getinfo_bat_percent" : "Заряд",
    "getinfo_v" : "Напряжение",
    "getinfo_bat" : "Аккумулятор",
    "getinfo_connected" : "Подключено",
    "getinfo_not_connected" : "Не подключено",
    
    "getinfo_refresh" : "Обновить",


    "notifications_duration_sec" : "сек.",
    "notifications_remove_process" : "Удаляю оповещение",

    "mqttSetDefaultsDomoticz": "Настройки по умолчанию для Dmoticz", 
    "mqttSetDefaultsHA": "Настройки по умолчанию для Home Assistant", 
          
    "mode_time" : "Время",
    "mode_date" : "Дата",
    "mode_temp" : "Температура",
    "mode_text" : "Оповещения",
    "mode_hum" : "Влажность",
    "mode_ext" : "Внешние данные",
    
    "cfg__modes" : "Режимы отображения",
    "cfg_modeDuration" : "Время отображения режима (сек)",
    "cfg_modeList" : "Порядок режимов",
    "cfg_screenAutoTurnOff" : "Время выключения (00:00)",
    "cfg_screenAutoTurnOn" : "Время включения (00:00)",
    "cfg_screenContrast" : "Яркость экрана (0-16)",
    "cfg_screenRotate" : "Перевернуть экран (0-1)",
};