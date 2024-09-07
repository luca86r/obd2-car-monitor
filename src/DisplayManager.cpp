#include "DisplayManager.h"

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

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    int barWidth = SCREEN_WIDTH - 20; // Larghezza della barra di caricamento
    int barHeight = 10; // Altezza della barra di caricamento
    int barX = 10; // Posizione orizzontale della barra (in modo che sia centrata)
    int barY = SCREEN_HEIGHT / 2 - 5; // Posizione verticale della barra

    bool isGrowing = true;

    for (int j = 0; j <= 1; j++) {

      for (int i = 0; i <= barWidth; i++) {
        // Pulisci il display
        display.clearDisplay();

        // Disegna il testo "Loading..."
        display.setCursor((SCREEN_WIDTH - 90) / 2, barY - 20);
        display.print("Initializing...");

        // Disegna il contorno della barra
        display.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE);

        if (isGrowing) {
          // Disegna la barra di caricamento che cresce
          display.fillRect(barX, barY, i, barHeight, SSD1306_WHITE);
        }
        else {
          // Disegna la barra di caricamento che decresce
          display.fillRect(barX + i, barY, barWidth - i, barHeight, SSD1306_WHITE);
        }
        // Mostra l'immagine
        display.display();

        // Ritardo per l'animazione
        delay(10);
      }

      isGrowing = !isGrowing;
    }
}


void DisplayManager::printSinglePID(String pidName, String pidValue, String pidUnit) {

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
  display.setCursor(valueX, 30);

  display.print(value);
  
  display.display();
}
