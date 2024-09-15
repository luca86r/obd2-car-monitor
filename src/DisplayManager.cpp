#include "DisplayManager.h"
#include "config.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


void DisplayManager::init() {

    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        oled_ko = true;
    }
    else {
        display.clearDisplay();
        display.display();      

        Serial.println(F("SSD1306 ok"));
    }
}

void DisplayManager::printText(String text) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print(text);
  display.print("\n");

  display.display();
}

void DisplayManager::printFloat(String pidName, float pidValue, String valueUnit, String error) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print(pidName + ": ");
  display.print(pidValue);
  display.print(" " + valueUnit);
  display.print("\n");
  display.print("Errore: " + error);
  display.print("\n");

  display.display();
}

void DisplayManager::printInt(String pidName, int32_t pidValue, String error) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print(pidName + ": ");
  display.print(pidValue);
  display.print("\n");
  display.print("Errore: " + error);
  display.print("\n");

  display.display();
}

void DisplayManager::clearDisplay() {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.display();
}

void DisplayManager::loadingAnimation() {

  if (oled_ko) {
    return;  
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int barWidth = SCREEN_WIDTH - 20; 
  int barHeight = 10;
  int barX = 10;
  int barY = SCREEN_HEIGHT / 2 - 5;

  loadingBarProgress++;
  if (loadingBarProgress > barWidth) {
    loadingBarProgress = 0;
    isLoadingAnimationGrowing = !isLoadingAnimationGrowing;
  }
  
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 90) / 2, barY - 20);
  display.print("Initializing...");

  display.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE);

  if (isLoadingAnimationGrowing) {
    display.fillRect(barX, barY, loadingBarProgress, barHeight, SSD1306_WHITE);
  }
  else {
    display.fillRect(barX + loadingBarProgress, barY, barWidth - loadingBarProgress, barHeight, SSD1306_WHITE);
  }

  display.display();
  delay(10);
}

void DisplayManager::printGaugePID(String pidName, String pidValue, String pidUnit, int percentage) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  // With text size=1: 6px per char
  unsigned int pidNamePx = pidName.length() * 6;
  int pidNameX = (SCREEN_WIDTH / 2) - (pidNamePx / 2);
  pidNameX = pidNameX < 0 ? 0 : pidNameX;

  display.setTextSize(1);
  display.setCursor(pidNameX , 0);
  display.print(pidName);

  String value = "(" + pidValue + ")";
  unsigned int valuePx = value.length() * 6;
  int valueX = (SCREEN_WIDTH / 2) - (valuePx / 2);
  valueX = valueX < 0 ? 0 : valueX;

  String unit = pidUnit;
  unsigned int unitPx = unit.length() * 6;
  int unitX = (SCREEN_WIDTH / 2) - (unitPx / 2);
  unitX = unitX < 0 ? 0 : unitX;

  display.setCursor(unitX, 0);
  display.print(unit);

  int r = 40;
  display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT, r + 10, WHITE);
  display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT, r + 6, WHITE);
  display.fillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT, 5, WHITE);

  //TODO calc x and y; indicator radius = r
  int x = 35; 
  int y = 38;
  display.fillTriangle(x, y, (SCREEN_WIDTH / 2) - 2, SCREEN_HEIGHT, (SCREEN_WIDTH / 2) + 2, SCREEN_HEIGHT,WHITE);


  if (showLoopIndicator) {
    display.setCursor(0, 0);
    display.fillRect(SCREEN_WIDTH - 4, 4, 3, 3, WHITE);
  }

  display.display();
}

void DisplayManager::printSinglePID(String pidName, String pidValue, String pidUnit, int percentage) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  // With text size=1: 6px per char
  unsigned int pidNamePx = pidName.length() * 6;
  int pidNameX = (SCREEN_WIDTH / 2) - (pidNamePx / 2);
  pidNameX = pidNameX < 0 ? 0 : pidNameX;

  display.setTextSize(1);
  display.setCursor(pidNameX , 0);
  display.print(pidName);

  // With text size=3: 18px per char
  String value = pidValue + pidUnit;
  unsigned int valuePx = value.length() * 18;
  int valueX = (SCREEN_WIDTH / 2) - (valuePx / 2);
  valueX = valueX < 0 ? 0 : valueX;

  display.setTextSize(3);
  display.setCursor(valueX, 25);
  display.print(value);

  if (showLoopIndicator) {
    display.setCursor(0, 0);
    display.fillRect(SCREEN_WIDTH - 4, 4, 3, 3, WHITE);
  }

  if (percentage >= 0) {
    display.drawRect(0, SCREEN_HEIGHT - 6, SCREEN_WIDTH, 5, 1);
    display.fillRect(0, SCREEN_HEIGHT - 6, SCREEN_WIDTH * abs(percentage) / 100, 5, 1);
  }

  display.display();
}

void DisplayManager::printSinglePIDWithWarning(String pidName, String pidValue, String pidUnit, int percentage, String warning1, String warning2) {

  if (oled_ko) {
    return;  
  }

  unsigned long currentMillis = millis();
  bool blinkDurationOk = (currentMillis - warningLastBlinkMs) < DISPLAY_WARNING_BLINK_DURATION;
  bool valueDurationOk = (currentMillis - warningLastValueDisplayMs) < DISPLAY_WARNING_VALUE_DURATION;

  if (isWarningBlinking) {
    if (warningBlinkCounter <= DISPLAY_WARNING_BLINK_COUNT) {
      if(!blinkDurationOk) {
        warningLastBlinkMs = millis();
        warningBlinkCounter++;
        if (warningBlinkCounter <= DISPLAY_WARNING_BLINK_COUNT) {
          // If the counter reach the limit, don't change color for a single cicle
          isWarningColorInverted = !isWarningColorInverted;
        }
      }
    }
    else {
      isWarningBlinking = false;
      warningBlinkCounter = 0;
      warningLastValueDisplayMs = millis();
    }
  }
  else {
    if (!valueDurationOk) {
      isWarningBlinking = true;
      warningLastBlinkMs = millis();
      warningBlinkCounter = 0;
    }
  }

  if (isWarningBlinking) {
    // Display warning blinking
    // With text size=2: 12px per char
    unsigned int w1Px = warning1.length() * 12;
    int w1X = (SCREEN_WIDTH / 2) - (w1Px / 2);
    w1X = w1X < 0 ? 0 : w1X;

    unsigned int w2Px = warning2.length() * 12;
    int w2X = (SCREEN_WIDTH / 2) - (w2Px / 2);
    w2X = w2X < 0 ? 0 : w2X;

    display.clearDisplay();
    display.setTextColor(isWarningColorInverted ? BLACK : WHITE);

    if (isWarningColorInverted) {
      display.fillScreen(WHITE);
    }

    display.setTextSize(2);
    display.setCursor(w1X, 10);
    display.print(warning1);

    display.setTextSize(2);
    display.setCursor(w2X, 42);
    display.print(warning2);

    display.display();
  }
  else {
    // Display data
    display.clearDisplay();
    display.setTextColor(WHITE);

    // With text size=1: 6px per char
    unsigned int pidNamePx = pidName.length() * 6;
    int pidNameX = (SCREEN_WIDTH / 2) - (pidNamePx / 2);
    pidNameX = pidNameX < 0 ? 0 : pidNameX;

    display.setTextSize(1);
    display.setCursor(pidNameX , 0);
    display.print(pidName);

    // With text size=3: 18px per char
    String value = pidValue + pidUnit;
    unsigned int valuePx = value.length() * 18;
    int valueX = (SCREEN_WIDTH / 2) - (valuePx / 2);
    valueX = valueX < 0 ? 0 : valueX;

    display.setTextSize(3);
    display.setCursor(valueX, 25);

    if (percentage >= 0) {
      display.drawRect(0, SCREEN_HEIGHT - 6, SCREEN_WIDTH, 5, 1);
      display.fillRect(0, SCREEN_HEIGHT - 6, SCREEN_WIDTH * abs(percentage) / 100, 5, 1);
    }

    display.print(value);
    display.display();
  }
}

void DisplayManager::setLoopIndicator(bool show) {
  showLoopIndicator = show;
}
