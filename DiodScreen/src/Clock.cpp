#include <Clock.h>

Clock::Clock(LedControl * ascreen) {
    
    enabled = false;
    screen = ascreen;
    
    fTime.timeText = timeBefore;
    fTime.t = 0;

    for (int i = 0; i < 6; i++) {        
          flips[i].enabled = false;
    }
    
}

void Clock::initTime(time_t newTime) {
    
    if (newTime > 0) {
        defaultTime = newTime;
    }

    setenv("TZ", timezone.c_str(), 1);
    tzset();
      
    // todo ntp
    
    timeval tv = { defaultTime, 0 };
    settimeofday(&tv, nullptr);
    
}

void Clock::drawDate(int screenX, int screenY) {

    posX = screenX;
    posY = screenY;
    
    updateTime(true);
    
    uText timeString = screen->getUText(fTime.date);

    if (timeString.pixelWidth < screen->getWidth()) {

      int baseX = (int) ceil((float) ( (screen->getWidth() - timeString.pixelWidth) / 2.0 ));
       
      // char buffer[128];
      // sprintf(buffer, "[basex: %d] %.2f %d", baseX, (float) ( ((float) screen->getWidth() - (float) textWidth) / 2.0 ), textWidth);
      // Serial.println(buffer);

      screen->drawString(baseX, posY, fTime.date);

    } else {

      screen->drawStringScrolled(posX, posY, fTime.date);
    }
    
}

void Clock::drawClock(int screenX, int screenY) {

    posX = screenX;
    posY = screenY;
    
    enabled = true;

    textWidth = 0;

    secondTimerStart = millis();
    flipTimerStart = millis();
    blinkTimerStart = millis();

    fontManifest screenFont = screen->getFont();
    int baseX = posX;
    int timePosCursor = 0;
    
    updateTime();

    Serial.println(fTime.timeText);
    int dbIndexes[8];
        
    for (int i = 0; i < 8; i++) {
      dbIndexes[i] = screen->getGlyphIndex(fTime.timeText[i]);

      if (i > 0) textWidth += screenFont.settings[dbIndexes[i]].offsetXb;  
      textWidth += screenFont.width;
      textWidth += screenFont.settings[dbIndexes[i]].offsetXa;
    }

    if (alignCenter && textWidth < screen->getWidth()) {
       baseX = (int) ceil((float) ( (screen->getWidth() - textWidth) / 2.0 ));
       posX = baseX;
    }    

     char buffer[128];
     sprintf(buffer, "width x : %d | posX : %d", textWidth, posX);
     Serial.println(buffer);

    for (int i = 0; i < 8; i++) {

      if (i > 0) {
        baseX += screenFont.settings[dbIndexes[i]].offsetXb;
      }

      if (i != 2 && i != 5) { // exclude separators  :

          timePositions[timePosCursor] = baseX;
          timePosCursor++;

      } else {
          separatorPositions[(i == 2) ? 0 : 1] = baseX;         
      }

      screen->drawGlyph(baseX, posY, dbIndexes[i]);      
      baseX += screenFont.width;

      if (i != 7) baseX += screenFont.settings[dbIndexes[i]].offsetXa;
    }

    timeBefore = fTime.time;

    // todo think init animation - time before
    //tick();

    screen->update();

    /*
    timeString = screen->getUText(fTime.timeText);

    std::vector<int> tPoses = screen->getStringCharsPoses(posX, timeString);

    timePositions[0] = tPoses[0];
    timePositions[1] = tPoses[1];
    timePositions[2] = tPoses[3];
    timePositions[3] = tPoses[4];
    timePositions[4] = tPoses[6];
    timePositions[5] = tPoses[7];

    screen->drawString(posX, posY, timeString);   
     */ 
}

void Clock::stop() {    
    enabled = false;
}

bool Clock::isAnimating() {

    for (int i = 0; i < 6; i++) {
        if (flips[i].enabled) return true;
    } 

    return false;      
}

void Clock::hideSeparator(int n) {
  
    int xOffset = screen->getFont().settings[screen->getGlyphIndex(':')].offsetXa;             
    int xTo = screen->getFont().width + xOffset + separatorPositions[n];

    // char buffer[128];
    // sprintf(buffer, "separator x start : %d to: %d glyph base offset [%d]", separatorPositions[n], xTo, xOffset);
    // Serial.println(buffer);

    for (int frameY = posY; frameY <= posY + screen->getFont().height; frameY++) {
          for (int frameX = separatorPositions[n]; frameX <= xTo; frameX++) {
                screen->drawPixel(frameX, frameY, false);
          }                          
    }
}

bool Clock::setSleepState(String text, bool from) {

  Serial.println("setSleepState : " + text.length());
  if (text.length() < 5) return false; 

  String tmp;
         tmp += text[0];
         tmp += text[1];

  unsigned int h;
  if(sscanf(tmp.c_str(), "%d", &h) != 1) {
      return false; 
  }

  unsigned int m;

  tmp = "";
  tmp += text[3];
  tmp += text[4];

  if(sscanf(tmp.c_str(), "%d", &m) != 1) {
      return false; 
  }

  if (from) {
  //  sfrom.

    sfrom.h = h;
    sfrom.m = m;

  } else {

    sto.h = h;
    sto.m = m;
  }

  Serial.println("h : " + String(h) + " - m : " + String(m));
  return true; 
}

void Clock::enableSleepControll(bool state) {

  if (timeCompare(sfrom, sto) >= 1) {    
      timeState tmp = sfrom;
      sfrom = sto;
      sto = tmp;
  }

  sleepControll = state;
  if (state) {
    sleepTimerStart = millis();
    updateSleepTimeState();
  }
}

// t1 bigger (newer) +1 | t1 == t2 - 0 | t2 bigger (newer) -1
int Clock::timeCompare(timeState & t1, timeState & t2) {
 
  if (t1.h == t2.h) {
    if (t1.m > t2.m) {
      return 1;
    } else if (t1.m == t2.m) {
      return 0;
    }
  } else if (t1.h > t2.h) {
    return 1;
  }

  return -1;
}

bool Clock::isSleep() {
  if (!sleepControll) return false;
  
  if (timeCompare(cTime, sfrom) >= 0 && timeCompare(sto, cTime) >= 1) {
    return true;
  } 

  return false;
}

void Clock::sleepTick() {

  if (!sleepControll) return;

  unsigned long now = millis();
  if (now - sleepTimerStart >= 10000) {
        
      sleepTimerStart = now;  
      updateSleepTimeState();
      sleepRequired = isSleep();
      
      Serial.println("sleepTick : " + String(cTime.h) + ":" + String(cTime.m));
  }

}

void Clock::updateSleepTimeState() {

    time_t tnow = time(nullptr);
    struct tm stnow;
    localtime_r(&tnow, &stnow);

    char buffer[128];
    
    String tmp = "";
    
    strftime(buffer, sizeof(buffer), "%H", &stnow);
    tmp = buffer;
    sscanf(tmp.c_str(), "%d", &cTime.h);

    strftime(buffer, sizeof(buffer), "%M", &stnow);
    tmp = buffer;
    sscanf(tmp.c_str(), "%d", &cTime.m);
}

bool Clock::tick() {
    
    if (!enabled) return true;
    
    bool animationInProgress = isAnimating();
    unsigned long now = millis();
    updateRequired = false;

    if (now - secondTimerStart >= 1000) {
        
        secondTimerStart = now;  
        syncTimeCurrent--;

        if (syncTimeCurrent <= 0) {

           syncTimeCurrent = syncTimeEvery;
           defaultTime = time(nullptr); 

        } else {

           defaultTime += 1; 
        }
   
        updateTime(); 

        for (int i = 0; i < 6; i++) {
            
              if (timeBefore[i] == fTime.time[i]) continue;
              
              if (!flips[i].enabled) {
                  flips[i].enabled = true; 
                  flips[i].scrollY = 1;
              }
              
              flips[i].baseX = timePositions[i];
              flips[i].baseY = posY;
              flips[i].from = timeBefore[i];
              flips[i].to = fTime.time[i];
        }
        
        timeBefore = fTime.time;
    }
        
    if (blinkSeparatorSpeed > 0 && now - blinkTimerStart >= blinkSeparatorSpeed) {

        blinkTimerStart = now; 
        updateRequired = true;

        if (separatorShowed) {
            hideSeparator(0);
            hideSeparator(1);
        } else {            
            screen->drawGlyphByChar(separatorPositions[0], posY, ':');
            screen->drawGlyphByChar(separatorPositions[1], posY, ':');
        }

        separatorShowed = separatorShowed ? false : true;
    }    

    if (now - flipTimerStart >= flipSpeed) {
        
        flipTimerStart = now;
      
        for (int i = 0; i < 6; i++) {
              
              if (!flips[i].enabled) continue;

              if (!screen->drawFrameEt.enabled) {
                  screen->setDrawFrame(posX, textWidth, posY - flipYPadding, digitHeight + visibleYoffset + flipYPadding);
              }

              updateRequired = true;
              
              for (int frameY = flips[i].baseY - flipYPadding; frameY <= flips[i].baseY + digitHeight + flipYPadding; frameY++) {

                    for (int frameX = flips[i].baseX; frameX <= flips[i].baseX + digitWidth; frameX++) {
                          
                          screen->drawPixel(frameX, frameY, false);
                    }                          
              }

              screen->drawGlyphByChar(flips[i].baseX, flips[i].baseY + flips[i].scrollY, flips[i].from);
              screen->drawGlyphByChar(flips[i].baseX, flips[i].baseY - digitHeight - flipYPadding + flips[i].scrollY, flips[i].to);                        

              if (flips[i].scrollY < digitHeight + flipYPadding) {
                  
              } else {

                  flips[i].enabled = false;
              }
              
              flips[i].scrollY++;
        }
        
    }
    
    if (updateRequired) {
        screen->setDrawFrame();
        screen->update();
    }

    return animationInProgress;
}

clockFormatted Clock::getTime() {
    return fTime;
}

void Clock::updateTime(bool full) {
    
    fTime.t = time(&defaultTime); // defaultTime - can be used asis, but before show clock needs to be synced // time(&defaultTime); // time(nullptr); 
    struct tm stnow;
    localtime_r(&fTime.t, &stnow);

    char buffer[128];
    
    String tmp = "";

    strftime(buffer, sizeof(buffer), "%d.%m.%y", &stnow);
    fTime.date = buffer;
    
    strftime(buffer, sizeof(buffer), "%H%M%S", &stnow);
    fTime.time = buffer;
   
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &stnow);
    fTime.timeText = buffer;

    if (full) {

        strftime(buffer, sizeof(buffer), "%w", &stnow);
        tmp = buffer;

        int dayN;
        sscanf(tmp.c_str(), "%d", &dayN);

        switch(dayN) {
          case 0:
            fTime.dayText = "вc";
            break;
          case 1:
            fTime.dayText = "пн";
            break;
          case 2:
            fTime.dayText = "вт";
            break;
          case 3:
            fTime.dayText = "ср";
            break;
          case 4:
            fTime.dayText = "чт";
            break;
          case 5:
            fTime.dayText = "пт";
            break;
          case 6:
            fTime.dayText = "сб";
            break;
        }

        strftime(buffer, sizeof(buffer), "%m", &stnow);
        tmp = buffer;

        int monthN;
        sscanf(tmp.c_str(), "%d", &monthN);

        switch(monthN) {
          case 1:
            fTime.monthText = "янв";
            break;
          case 2:
            fTime.monthText = "фев";
            break;
          case 3:
            fTime.monthText = "март";
            break;
          case 4:
            fTime.monthText = "апр";
            break;
          case 5:
            fTime.monthText = "май";
            break;
          case 6:
            fTime.monthText = "июнь";
            break;
          case 7:
            fTime.monthText = "июль";
            break;  
          case 8:
            fTime.monthText = "авг";
            break; 
           case 9:
            fTime.monthText = "сент";
            break;
          case 10:
            fTime.monthText = "окт";
            break;  
          case 11:
            fTime.monthText = "нояб";
            break; 
          case 12:
            fTime.monthText = "дек";
            break; 
        }
        
    }
}
