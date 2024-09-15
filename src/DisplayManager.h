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
                void printGaugePID(String pidName, String pidValue, String pidUnit, int percentage);
                void printSinglePID(String pidName, String pidValue, String pidUnit, int percentage);
                void printSinglePIDWithWarning(String pidName, String pidValue, String pidUnit, int percentage, String warning1, String warning);
                void setLoopIndicator(bool show);
	
	private:
                Adafruit_SSD1306 display;
                bool oled_ko = false;

                bool isLoadingAnimationGrowing = true;
                int loadingBarProgress = 0;

                bool isWarningBlinking = false;
                bool isWarningColorInverted = false;
                int warningBlinkCounter = 0;
                unsigned long warningLastBlinkMs;
                unsigned long warningLastValueDisplayMs;

                bool showLoopIndicator = false;
};

