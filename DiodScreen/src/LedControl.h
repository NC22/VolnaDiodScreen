#ifndef LedControl_h
#define LedControl_h

#include <Arduino.h>

//#include <avr/pgmspace.h>

//#if (ARDUINO >= 100)

//#else
//#include <WProgram.h>
//#endif

 
typedef struct {
  unsigned int size;
  std::vector<int> charCodes;
  String text;
  int pixelWidth;
} uText;

typedef struct {
  int size;
  int code;
} uCharCode;

typedef struct {
  int fromX;
  int width;
  int height;
  int fromY;
  int enabled;
} drawFrame;

typedef struct {
  int offsetXb;
  int offsetXa;
  int offsetY;
} glyphSettings;

typedef struct {
  int * glyphs;
  int * map;
  int width;
  int height;
  int size;
  glyphSettings * settings;
} fontManifest;

typedef struct {
  bool enabled; // scroll enabled
  unsigned long scrollSpeed;     // scroll speed in miliseconds
  unsigned long startTime;  
  uText text;
  int paddingX; // padding after text to fill empty space before repeat text
  int currentX; // current scroll offset from base position
  int baseY;    // base position x,y
  int baseX;
} textAnimationX;

typedef struct {
  bool enabled; // scroll enabled
  unsigned long scrollSpeed;     // scroll speed in miliseconds
  unsigned long startTime;  
  int currentY; // current scroll offset from base position
  int baseY;    // base position x,y
  int baseX;
  int width;   
  int height;
} transitionAnimationY;

class LedControl {
    private :
        /* The array for shifting the data to the devices */
        byte spidata[16];
        /* Send out a single command to the device */
        void spiTransfer(int addr, byte opcode, byte data);

        int getBit(int byte, int position);
        uCharCode uniord(String c, int offset);

        int getBufferOffset(int x, int y);

        bool bufferOutput = false; // output to hidden buffer

        bool *status;
        bool *hiddenScreen;

        int statusSize;
        int width;
        int height;
        int bufferWidth;

        /* Data is shifted out of this pin*/
        int SPI_MOSI;
        /* The clock is signaled on this pin */
        int SPI_CLK;
        /* This one is driven LOW for chip selectzion */
        int SPI_CS;
        /* The maximum number of devices we use */
        int maxDevices;

        fontManifest font;

    public:
        /* 
         * Create a new controler 
         * Params :
         * dataPin		pin on the Arduino where data gets shifted out
         * clockPin		pin for the clock
         * csPin		pin for selecting the device 
         * numDevices	maximum number of devices that can be controled
         */
        LedControl(int dataPin, int clkPin, int csPin, int numDevicesX, int numDevicesY);
        
        int getWidth();
        int getHeight();

        drawFrame drawFrameEt;
        bool rotate = false;

        void setRotate(bool state);
        void enableBufferOutput(bool state);
        void initDevices();

        /*
         * Gets the number of devices attached to this LedControl.
         * Returns :
         * int	the number of devices on this LedControl
         */
        int getDeviceCount();

        /* 
         * Set the shutdown (power saving) mode for the device
         * Params :
         * addr	The address of the display to control
         * status	If true the device goes into power-down mode. Set to false
         *		for normal operation.
         */
        void shutdown(bool status);

        /* 
         * Set the number of digits (or rows) to be displayed.
         * See datasheet for sideeffects of the scanlimit on the brightness
         * of the display.
         * Params :
         * addr	address of the display to control
         * limit	number of digits to be displayed (1..8)
         */
        void setScanLimit(int limit);

        /* 
         * Set the brightness of the display.
         * Params:
         * addr		the address of the display to control
         * intensity	the brightness of the display. (0..15)
         */
        void setIntensity(int intensity);

        void clear();
       
        /* 
         * Set all 8 Led's in a row to a new state
         * Params:
         * addr	address of the display
         * row	row which is to be set (0..7)
         * value	each bit set to 1 will light up the
         *		corresponding Led.
         */
        void setRow(int addr, int row, byte value);
        void drawPixel(int x, int y, bool state);
        void drawGlyph(int screenX, int screenY, int charIndex);
        void drawGlyphByChar(int screenX, int screenY, char c);
        bool getPixel(int x, int y, bool hiddenBuffer = false);
        void applyBinBuffer(u_int8_t * buffer, int len);

        void drawString(int screenX, int screenY, String text, int loopPaddingX = -1);
        void drawStringUtext(int screenX, int screenY, uText & utext, int loopPaddingX = -1);
        void drawStringScrolled(int scrollX, int scrollY, String text, int paddingX = 10);
        void drawStringScrolledUtext(int scrollX, int scrollY, uText & text, int paddingX = 10);

        void transitionScreen(int x = 1, int y = 1, int width = -1, int height = -1);        

        void setDrawFrame(int fromX = -1, int width = -1, int fromY = -1, int height = -1);

        void clearDisplay(int addr);
        
        int getGlyphIndex(int s);
        int getStringWidth(uText & text);
        // std::vector<int> getStringCharsPoses(int baseX, uText & text);
            
        uText getUText(String text);
            
        fontManifest & getFont();
        
        void update();
        void tick();

        bool overwriteMode = false;

        textAnimationX textScroll;
        transitionAnimationY screenScroll;
        // todo setBuffer(main \ second)
};

#endif	//LedControl.h



