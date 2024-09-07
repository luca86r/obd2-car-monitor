#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class DisplayManager {
        public:
                void init();
                void printText(String text);
                void printFloat(String pidName, float pidValue, String valueUnit, String error);
                void printInt(String pidName, int32_t pidValue, String error);
                void clearDisplay();
                void loadingAnimation();
                void printSinglePID(String pidName, String pidValue, String pidUnit);
	
	private:
                Adafruit_SSD1306 display;
                bool oled_ko = false;
};

