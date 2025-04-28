#ifndef Clock_h
#define Clock_h

#include <Arduino.h>
#include <LedControl.h>
#include <time.h>      // time() ctime()
#include "sys/time.h"
#include <TZ.h>

typedef struct {
    
  String time;
  String date; 
  String timeText;
  String dayText;
  String monthText;

  time_t t;
  
} clockFormatted;

typedef struct {
    
  unsigned int h;
  unsigned int m;
  
} timeState;

typedef struct {
    
  char from;
  char to;
                        
  int scrollY;
    
  int baseX;
  int baseY;                        
  
  bool enabled;
  
} digitFlipAnimation;

class Clock {
    
    private :

        LedControl * screen;

        int posX;
        int posY;

        int textWidth;
        
        int syncTimeEvery = 10;
        int syncTimeCurrent = syncTimeEvery;

        unsigned long sleepTimerStart;
        unsigned long secondTimerStart;
        unsigned long flipTimerStart;
        unsigned long blinkTimerStart;
        bool separatorShowed = true;

        unsigned long flipSpeed = 100; 
        unsigned long blinkSeparatorSpeed = 0; // 1000;       
        String timeBefore = "133723";
        
        int timePositions[6];     
        int separatorPositions[2];   
        digitFlipAnimation flips[6];

        void hideSeparator(int n);
        
        clockFormatted fTime;
        bool enabled;
        bool updateRequired;

    public:

        Clock(LedControl * ascreen);
        
        int visibleYoffset = 2;
        int flipYPadding = 1;
        int digitWidth = 3;
        int digitHeight = 6;

        bool sleepControll = false; // is sleep controller enabled
        bool sleepRequired = false; // last sleep state  by isSleep() method
        timeState cTime;
        timeState sfrom;
        timeState sto;

        bool alignCenter = true;

        String timezone = "MSK-3";
        time_t defaultTime = 1510592825;
        
        void initTime(time_t newTime = 0);
        void drawClock(int screenX = 2, int screenY = 2);
        void drawDate(int screenX = 2, int screenY = 2);
        void stop();
        bool tick();
        bool isAnimating();

        int timeCompare(timeState & t1, timeState & t2);
        bool setSleepState(String t, bool from);
        void enableSleepControll(bool state);
        bool isSleep();
        void sleepTick();
        void updateSleepTimeState();

        clockFormatted getTime();
        void updateTime(bool full = false);
        
};






#endif