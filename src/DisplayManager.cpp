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

    // Esempio di animazione di caricamento: puntini che si rincorrono
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    int radius = 20; // Raggio del cerchio
    int numDots = 5; // Numero di puntini
    int angleStep = 360 / numDots; // Angolo tra i puntini

    // Loop per animare i puntini
    for (int frame = 0; frame < numDots; frame++) {
        display.clearDisplay();

        // Disegna il testo "Loading..."
        display.setCursor((SCREEN_WIDTH - 60) / 2, centerY - 30);
        display.print("Initializing...");

        // Disegna i puntini in movimento
        for (int i = 0; i < numDots; i++) {
        // Calcola l'angolo per ogni puntino, tenendo conto dello spostamento per l'animazione
        float angle = (i + frame) * angleStep * DEG_TO_RAD;
        int x = centerX + cos(angle) * radius;
        int y = centerY + sin(angle) * radius;

        // Disegna i puntini
        display.fillCircle(x, y, 3, SSD1306_WHITE);
        }

        // Mostra l'immagine
        display.display();

        // Aspetta un po' per far vedere l'animazione
        delay(150);
    }
  
}
