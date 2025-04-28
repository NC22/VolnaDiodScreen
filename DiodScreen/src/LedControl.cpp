/*
 *   based on LedControl.cpp - A library for controling Leds with a MAX7219/MAX7221
 */


#include "LedControl.h"
#include "Font8x8.h"
// #include "Font5x6.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

LedControl::LedControl(int dataPin, int clkPin, int csPin, int numDevicesX, int numDevicesY) {

    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;

    font = font8x8Config; //font5x6Config; // font8x8Config;
    textScroll.enabled = false;

    maxDevices=numDevicesX * numDevicesY;
    width = numDevicesX * 8;
    height = numDevicesY * 8;
    bufferWidth = maxDevices * 8;
    drawFrameEt.enabled = false;

    if(maxDevices<=0 || maxDevices>8 )
        maxDevices=8;

    statusSize = maxDevices * 8 * 8; // pixel buffer size for LED matrix 8x8
    status = new bool[statusSize];
    hiddenScreen = new bool[statusSize];

    pinMode(SPI_MOSI, OUTPUT);
    pinMode(SPI_CLK, OUTPUT);
    pinMode(SPI_CS, OUTPUT);

    digitalWrite(SPI_CS, HIGH);

    initDevices();
}

void LedControl::initDevices() {
    
    for(int i=0;i<statusSize;i++) {
        hiddenScreen[i]=false;
        status[i]=false;
    } 
        
    for(int i=0;i<maxDevices;i++) {
        spiTransfer(i,OP_DISPLAYTEST,0);
        //decode is done in source
        spiTransfer(i, OP_SCANLIMIT, 7);
        spiTransfer(i,OP_DECODEMODE,0);
        clearDisplay(i);
        spiTransfer(i, OP_SHUTDOWN,1);
    }   
    
    //scanlimit is set to max on startup
    //setScanLimit(7);
    //shutdown(true);
}

void LedControl::applyBinBuffer(u_int8_t * buffer, int len) {
    
    int byteCursor = 0; 
    int bitCursor = 0;
    int bufferIndex = 0;
    if (len <= 0) return;
    
    for (int y = 1; y <= height; y++) {
            
        for (int x = 1; x <= width; x++) {
            
            int charByte = buffer[byteCursor];

            if ( getBit(charByte, bitCursor) > 0 ) {
                status[bufferIndex] = true;
            } else {
                status[bufferIndex] = false;
            }

            bitCursor++;
            bufferIndex++;

            if (bitCursor > 7) {
                byteCursor++;
                bitCursor = 0;  
                
                if (byteCursor >= len) return;                         
            }
        }
    }
}


int LedControl::getDeviceCount() {
    return maxDevices;
}

void LedControl::shutdown(bool b) {
  
    for(int display=0;display<maxDevices;display++) {

        if(b)
            spiTransfer(display, OP_SHUTDOWN, 0);
        else
            spiTransfer(display, OP_SHUTDOWN, 1);

    } 
}

int LedControl::getGlyphIndex(int s) {

  // char buffer[128];
  // sprintf(buffer, "map length : %d  | char index : %d", font.size, s);
  // Serial.println(buffer);

    int i = 0;
    while (i < font.size)
    {
        if (font.map[i] == s) {
            break;
        }
        i++;
    }
 
    if (i < font.size)
    {
       return i;
    }

    return -1;
}

uText LedControl::getUText(String text) {

    int length = text.length();
    char const *c = text.c_str();
    
    uText textU;
    // memcpy(buffer, tt.data(), tt.length());
    
    std::vector<int> codeList;

    for ( int i = 0; i < length; i++) {
          
          //unsigned char ct = c[i];
          uCharCode ct = uniord(c, i);
          if (ct.size > 1) {
                i += ct.size-1;
          }
         
          codeList.push_back(ct.code);
          //sprintf(buffer, "Draw char : %s Index : %d", symbol, (int) symbol);
    }

    textU.size = codeList.size();
    textU.charCodes = codeList;
    textU.text = text;

    textU.pixelWidth = getStringWidth(textU);

    return textU;
}


// https://stackoverflow.com/questions/10333098/utf-8-safe-equivalent-of-ord-or-charcodeat-in-php
// todo add protection from hack symbol code (check max lenght)  - (char * c, unsigned int offset, unsigned int maxLength)

uCharCode LedControl::uniord(String c, int offset) {

    unsigned char h = (unsigned char) c[offset];

    uCharCode cn;
              cn.size = 1;

    if (h <= 0x7F) {
        
        cn.code = h;

    } else if (h < 0xC2) {

        cn.code = 0;

    } else if (h <= 0xDF) {
        
        cn.code = (h & 0x1F) << 6 | ((unsigned char)c[offset + 1] & 0x3F);
        cn.size = 2;

    } else if (h <= 0xEF) {

        cn.code = (h & 0x0F) << 12 | ((unsigned char)c[offset + 1] & 0x3F) << 6
                                 | ((unsigned char)c[offset + 2] & 0x3F);
        cn.size = 3;

    } else if (h <= 0xF4) {

        cn.code = (h & 0x0F) << 18 | ((unsigned char)c[offset + 1] & 0x3F) << 12
                                 | ((unsigned char)c[offset + 2] & 0x3F) << 6
                                 | ((unsigned char)c[offset + 3] & 0x3F);

        cn.size = 4;     

    } else {

        cn.size = 0;
    }

    return cn;
}

int LedControl::getBit(int byte, int position) // position in range 0-7
{
    return (byte >> (7 - position)) & 0x1;
}

fontManifest & LedControl::getFont() {
    
    return font;
}

// utf-8 unsafe, for digits \\ english

void LedControl::drawGlyphByChar(int screenX, int screenY, char c) {
    
    int dbIndex = getGlyphIndex((int) c);
    if (dbIndex >= 0) {
        drawGlyph(screenX, screenY, dbIndex);
    }
}

void LedControl::drawGlyph(int screenX, int screenY, int dbIndex) {
      
    int byteCursor = (int) ceil((float) (font.width * font.height) / 8) * dbIndex; 
    int bitCursor = 0;
        
    //char buffer[128];
    //sprintf(buffer, "[INDEX: %d] %d | %d %d", dbIndex, screenX, screenY, byteCursor);
    //Serial.println(buffer);

    for (int glyphY = 1; glyphY <= font.height; glyphY++) {
            
        for (int glyphX = 1; glyphX <= font.width; glyphX++) {
            
            int charByte = pgm_read_byte(&font.glyphs[byteCursor]); 
  
            //sprintf(buffer, "lasssssssst %02X || %d", charByte, charByte);
            //Serial.println(buffer);

            if ( getBit(charByte, bitCursor) > 0 ) {
                // todo enable overwrite mode ?
                drawPixel(screenX + glyphX - 1, screenY + glyphY + font.settings[dbIndex].offsetY - 1, true);
            } else if (overwriteMode) {
                drawPixel(screenX + glyphX - 1, screenY + glyphY + font.settings[dbIndex].offsetY - 1, false);
            }

            bitCursor++;
            
            if (bitCursor > 7) {
                byteCursor++;
                bitCursor = 0;                            
            }                   
        }          
    }
}

/*
std::vector<int> LedControl::getStringCharsPoses(int baseX, uText & text) {
  
    std::vector<int> posXList;

    for (unsigned int i = 0; i < text.size; i++) {
        
          int dbIndex = getGlyphIndex(text.charCodes[i]);
          if (dbIndex >= 0) {
              
                posXList.push_back(baseX);
                
                if (i > 0)  baseX += font.settings[dbIndex].offsetXb;
                baseX += font.width + font.settings[dbIndex].offsetXa;
          }
    }

    return posXList;
}
*/

int LedControl::getStringWidth(uText & utext) {
    
    int pixelWidth = 0;

    for (unsigned int i = 0; i < utext.size; i++) {
        
          int dbIndex = getGlyphIndex(utext.charCodes[i]);
          if (dbIndex >= 0) {
                if (i > 0)  pixelWidth += font.settings[dbIndex].offsetXb;
                pixelWidth += font.width + font.settings[dbIndex].offsetXa;
          }
    }
    
    if (pixelWidth <= 0) pixelWidth = 0;

    return pixelWidth;
}

void LedControl::tick() {

    if (screenScroll.enabled) {
      
        if (millis() - screenScroll.startTime >= screenScroll.scrollSpeed) {

            screenScroll.startTime = millis();
            int maxY = screenScroll.baseY + screenScroll.height - 1;
            int maxX = screenScroll.baseX + screenScroll.width - 1;

            // move current buffer bottom

            for (int frameY = maxY; frameY >= screenScroll.currentY; frameY--) {            
                for (int frameX = screenScroll.baseX; frameX <= maxX; frameX++) {                    
                    drawPixel(frameX, frameY, getPixel(frameX, frameY-1, false)); 
                }
            }

            /*
            int dashedPadding = 2;
            int dashedYStart = screenScroll.currentY - dashedPadding;
            
            if (dashedPadding > 0 && dashedYStart + dashedPadding > 0) {

                int dashed = 4;
                for (int frameY = dashedYStart - dashedPadding; frameY <= dashedYStart + dashedPadding; frameY++) {
        
                    for (int frameX = screenScroll.baseX; frameX <= maxX; frameX++) {
                        if (frameY <= 0) continue;
                        if (frameY == dashedYStart) {
                            drawPixel(frameX, dashedYStart, dashed <= 2 ? false : true); 
                            dashed--;
                            if (dashed <= 0) dashed = 4;
                        } else drawPixel(frameX, frameY, true);   
                    }
                }
            }
            */
            // display hidden buffer

            for (int frameY = screenScroll.baseY; frameY <= maxY; frameY++) {
            
                for (int frameX = screenScroll.baseX; frameX <= maxX; frameX++) {

                    int scrollY = screenScroll.currentY + frameY - screenScroll.height; // - dashedPadding; 
                    if (scrollY > 0) {
                         drawPixel(frameX, scrollY, getPixel(frameX, frameY, true));     
                    }
                   
                }
            }

            screenScroll.currentY++;

            update();

            if (screenScroll.currentY > screenScroll.height) { //  + (dashedPadding * 2)
                screenScroll.enabled = false;
                //Serial.println("finished");
            } 

        }


    } else if (textScroll.enabled) {
        
        if (millis() - textScroll.startTime >= textScroll.scrollSpeed) {
            
            textScroll.startTime = millis();

            for (int frameY = textScroll.baseY; frameY <= textScroll.baseY + font.height; frameY++) {
            
                for (int frameX = 1; frameX <= getWidth(); frameX++) {                   
                    drawPixel(frameX, frameY, false);
                }                          
            }

            drawStringUtext(textScroll.currentX, textScroll.baseY, textScroll.text, textScroll.paddingX);  
            textScroll.currentX--;

            int pathLength = abs(textScroll.baseX - textScroll.currentX);
            int textRowLength = textScroll.text.pixelWidth + textScroll.paddingX;

            //char buffer[128];
            //sprintf(buffer, "[pathLength: %d] [textLength: %d]", pathLength, textRowLength);
            //Serial.println(buffer);

            if (pathLength >= textRowLength) {
                textScroll.currentX = textScroll.baseX;
            }
            
            // for (int frameY = 1; frameY <= getHeight(); frameY++) {                   
            //    drawPixel(32, frameY, false);
            // }  

            update();   
        } 
    }
}

void LedControl::enableBufferOutput(bool state) {
    bufferOutput = state;
}


void LedControl::drawStringScrolled(int screenX, int screenY, String text, int paddingX) {
    
    uText utext = getUText(text);
    drawStringScrolledUtext(screenX, screenY, utext, paddingX);
}

void LedControl::drawStringScrolledUtext(int scrollX, int scrollY, uText & utext, int paddingX) {

    if (!paddingX) paddingX = 4;
    
    textScroll.enabled = true;
    textScroll.scrollSpeed = 100;
    textScroll.startTime = millis();
    textScroll.text = utext;
    textScroll.baseX = scrollX;
    textScroll.baseY = scrollY;
    textScroll.currentX = textScroll.baseX;
    textScroll.paddingX = paddingX;

    drawStringUtext(scrollX, scrollY, textScroll.text, paddingX);    
}    

void LedControl::drawString(int screenX, int screenY, String text, int loopPaddingX) {
    
    uText utext = getUText(text);
    drawStringUtext(screenX, screenY, utext, loopPaddingX);
}

void LedControl::drawStringUtext(int screenX, int screenY, uText & utext, int loopPaddingX) {
 
    if (utext.pixelWidth <= 0) return;
    int posX = screenX;
    // delete [] drawPosMap;
    // drawPosMap = new int[text.length()];
    
    // char buffer[255];
    // if (loopPaddingX > 0) {
    //    
    //    sprintf(buffer, "Draw string : %s Length : %d Pixels width : %d padX : %d", utext.text.c_str(), utext.size, utext.pixelWidth, loopPaddingX);
    //    Serial.println(buffer);
    // }

    for (unsigned int i = 0; i < utext.size; i++) {
          
          int dbIndex = getGlyphIndex(utext.charCodes[i]);
          
          // sprintf(buffer, "Draw char code : %d | DB index %d", textu.charCodes[i], dbIndex);
          // Serial.println(buffer);
          if (dbIndex < 0) {

            char buffer[128];
            sprintf(buffer, "Unknown code : %d", utext.charCodes[i]);
            Serial.println(buffer);

            continue;
          }


          if (i > 0) {
            posX += font.settings[dbIndex].offsetXb;
          }

          // drawPosMap[i] = posX;
          
          drawGlyph(posX, screenY, dbIndex);
          
          posX += font.width + font.settings[dbIndex].offsetXa;
          if (posX > getWidth()) break;
    }

    if (loopPaddingX > 0) {
        
        posX += loopPaddingX;
        int iteration = 0;
        int leftPosX = screenX;
            leftPosX -= utext.pixelWidth;
            leftPosX -= loopPaddingX;
        
        // sprintf(buffer, "loop left x : %d ", leftPosX);
        // Serial.println(buffer);

        while (leftPosX + utext.pixelWidth >= 1 && iteration < 100) {
            
            iteration++;
            drawStringUtext(leftPosX, screenY, utext, -1);
            
            leftPosX -= utext.pixelWidth;
            leftPosX -= loopPaddingX;
        }
          
        // sprintf(buffer, "loops : %d ", iteration);
        // Serial.println(buffer);

        iteration = 0;
        while (posX <= getWidth() && iteration < 100) {
            iteration++;
            drawStringUtext(posX, screenY, utext, -1);
            posX += utext.pixelWidth + loopPaddingX;
        }
        
    }
        
}

int LedControl::getWidth() {
    
    return width;
}

int LedControl::getHeight() {
  
    return height;
}

void LedControl::setDrawFrame(int fromX, int width, int fromY, int height) {

    if (fromX <= 0) {

        drawFrameEt.enabled = false;

    } else {

        drawFrameEt.enabled = true;
        drawFrameEt.fromX = fromX;
        drawFrameEt.width = width;
        drawFrameEt.fromY = fromY;
        drawFrameEt.height = height;

    }
}

void LedControl::setRotate(bool state) {

    String rotateState = state ? "[rotated]" : "[NOT rotated]";
    Serial.println("Screen : " + rotateState); 

    rotate = state;
}

// from 1 to screenWidth \ screenHeight
void LedControl::drawPixel(int x, int y, bool state) {
    
    if (x <= 0 || y <= 0) return;
    if (x > getWidth() || y > getHeight()) return;

    if (drawFrameEt.enabled) {
        if ( x < drawFrameEt.fromX || x > drawFrameEt.fromX + drawFrameEt.width - 1) {
            return;
        } else if (y < drawFrameEt.fromY || y > drawFrameEt.fromY + drawFrameEt.height - 1) {
            return;
        }
    }

    //char buffer[128];
    //sprintf(buffer, "Draw pixel at : %d %d | displays : %d ", x, y, displaysBottom);
    //Serial.println(buffer);

    int offset = getBufferOffset(x, y);    

    if (offset < 0) return;
    if (offset >= statusSize) return;

    if (bufferOutput) {
        hiddenScreen[offset] = state;
    } else {
        status[offset] = state;
    }    
}

int LedControl::getBufferOffset(int x, int y) {
     
    if (y > 8) {
        int displaysBottom = (int) ceil((float) (y / 8)); 
        x += displaysBottom * getWidth();
        y -= displaysBottom * 8;
    }

    if (rotate) {
        x = abs(x-getWidth()-1);
        y = abs(y-getHeight()-1);
    }

    return (y - 1) * bufferWidth + x - 1;   
}

bool LedControl::getPixel(int x, int y, bool hiddenBuffer) {    

    int offset = getBufferOffset(x, y); 
    
    if (hiddenBuffer) {
        return hiddenScreen[offset];
    } else {
        return status[offset];
    }  
}

void LedControl::transitionScreen(int x, int y, int width, int height) {

    if (width == -1) {
        width = getWidth();
    } 

    if (height == -1) {
        height = getHeight();
    }

    screenScroll.enabled = true;
    screenScroll.baseX = x;
    screenScroll.baseY = y;
    screenScroll.width = width;
    screenScroll.height = height;
    screenScroll.startTime = millis();
    screenScroll.currentY = screenScroll.baseY;
    screenScroll.scrollSpeed = 100;
}

void LedControl::update() {
  
  if (bufferOutput) return;

  int display = maxDevices;  
  int displayRowCursor = 0;

  //char log[128];
  for(int bufferOffset=0; bufferOffset < statusSize; bufferOffset+=8) {

        byte row = 0;
        for (int b = 0; b <= 7; b++){
            row |= (status[bufferOffset + 7-b] ? 1 : 0) << (b);
        }
     
        // sprintf(log, "Display %d ---- %d", display, displayRowCursor);
        // Serial.println(log);

        setRow(display-1, displayRowCursor, row);

        display--;

        if (display <= 0) {
            display = maxDevices;
            displayRowCursor++;
        }
  }
}

void LedControl::setScanLimit(int limit) {

    if(limit>=0 && limit<8) {

    } else {
        limit = 1;
    }
    
    for(int display=0;display<maxDevices;display++) {
        spiTransfer(display, OP_SCANLIMIT, limit);
    } 
}

void LedControl::setIntensity(int intensity) {
    
    char buffer[128];
    sprintf(buffer, "screen %d x %d, buffer %d, screens %d", width , height, statusSize, maxDevices);
    Serial.println(buffer);


    if(intensity>=0 && intensity<16) {

    } else {
        intensity = 1;
    }
    
    for(int display=0;display<maxDevices;display++) {
        spiTransfer(display, OP_INTENSITY, intensity);
    }  
}

void LedControl::clear() {

    textScroll.enabled = false;

    for(int i=0; i < statusSize; i++) {
        if (bufferOutput) {
           hiddenScreen[i]=false;
        } else {
           status[i]=false;     
        }        
    }            

    if (!bufferOutput) update();
}

void LedControl::clearDisplay(int addr) {

    if(addr<0 || addr>=maxDevices)
        return;

    for(int i=0;i<8;i++) {
        spiTransfer(addr, i+1,0);
    }
}

void LedControl::setRow(int addr, int row, byte value) {

    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7)
        return;

    spiTransfer(addr, row+1, value);
}

void LedControl::spiTransfer(int addr, volatile byte opcode, volatile byte data) {

    //Create an array with the data to shift out

    int offset=addr*2;
    int maxbytes=maxDevices*2;

    for(int i=0;i<maxbytes;i++){
        
        spidata[i]=(byte)0;
    }
        
    //put our device data into the array

    spidata[offset+1]=opcode;
    spidata[offset]=data;

    //enable the line 

    digitalWrite(SPI_CS,LOW);

    //Now shift out the data 

    for(int i=maxbytes;i>0;i--) {

        shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
    }        

    //latch the data onto the display
    digitalWrite(SPI_CS,HIGH);
}    


