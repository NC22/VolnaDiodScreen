#include <WebServerBase.h>


#if defined(LOCALE_RU)
// [Russian]
//#include <locale/LocaleRu.h>
#include <ui/out/locale_ru/locale.h>
#elif defined(LOCALE_EN)
// [English]
//#include <locale/LocaleEn.h>
#include <ui/out/locale_en/locale.h>
#endif

// extern const unsigned int webdataSize_locale_js PROGMEM;
// extern const char webdata_locale_js[] PROGMEM;

class WebServerLed:public WebServerBase {
    protected : 
        void router() override;
        String getInfo() override;
        
    private :

        void apiTempSleep();
        void apiTempModelist();
        void apiGet();
        void apiGetLanguage();
        void apiDeleteNotification();    

        void apiDirectInput();    
        
        void showNotification();  
        void switchMode();     

    public:
       
        WebServerLed(Env * env, int port = 80);
};




