#include "ELMduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Preferences.h"
#include "config.h"
#include "BluetoothManager.h"
#include "ELM327Manager.h"

// === Preferences ===
Preferences preferences;

// === Bluetooth ===
BluetoothManager bluetoothManager;

// === ELM327 ===
ELM327Manager ELM327Manager;

// === Display ===
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Screen SSD1306 128x64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oled_ko = false;

void oledPrintData() {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print("battery: ");
  display.print(ELM327Manager.getBatteryVoltage());
  display.print(" v");
  display.print("\n");
  
  display.print("EGR cmd: ");
  display.print(ELM327Manager.getCommandedEGR());
  display.print(" %");
  display.print("\n");
  
  display.print("egrError: ");
  display.print(ELM327Manager.getEgrError());
  display.print(" %");
  display.print("\n");
  
  display.print("manifoldPressure: ");
  display.print(ELM327Manager.getManifoldPressure());
  display.print(" kPa");
  display.print("\n");
  
  display.print("dpfDirtLevel: ");
  display.print(ELM327Manager.getDpfDirtLevel());
  display.print(" %");
  display.print("\n");

  display.print("kmsSinceDpf: ");
  display.print(ELM327Manager.getKmsSinceDpf());
  display.print(" km");
  display.print("\n");

  display.print("regenerationStatus: ");
  display.print(ELM327Manager.getRegenerationStatus());
  display.print(" %");

  display.display();
}

void oledPrintText(String text) {

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

void oledPrintFloat(String pidName, float pidValue, String valueUnit, String error) {

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

void oledPrintInt(String pidName, int32_t pidValue, String error) {

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

void oledClearDisplay() {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.display();
}

void oledInit() {

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

void elm327ReadAllData() {

  ELM327Manager.readAllData();
  oledPrintData();
}

void setup()
{
    Serial.begin(115200);

    // Bluetooth init
    bluetoothManager.init();

    // OLED init
    oledInit();    
}

void loop() {

  if (!bluetoothManager.isConnected()) {

    oledPrintText("Initializing...");

    // If BT is not connected, forse EML327 init
    ELM327Manager.resetInitState();
  }

  bluetoothManager.checkOrConnect();
  
  if (bluetoothManager.isConnected()) {
    ELM327Manager.checkOrInit(bluetoothManager.getBtSerial());
  }
  
  elm327ReadAllData();

  delay(100);
}

/*


Altri da provare
"Accumulo di fuliggine DPF Astra-J","Fuliggine DPF","223275","A","0","100","%","7E0"
"Distanza Astra-J dopo l'ultima sostituzione DPF","Dis.ultima sostituzione DPF","223276","A*65536+B*256+C","0","100000","km","7E0"
"Distanza Astra-J dall'ultima rigenerazione DPF","Dist.DPF reg","223277","A*65536+B*256+C","0","100000","km","7E0"
"Stato di rigenerazione Astra-J DPF","Stato DPF","223274","A*100/255","0","100","%","7E0"
"Temperatura media di ingresso DPF Astra-J durante la rigenerazione","Ingresso DPF temp","223279","A*5-40","-40","1235","grado C","7E0" 
"Durata media di rigenerazione DPF Astra-J","Aver.dur.DPF","22327A","A*256+B","0","65535","S","7E0"
"Contatore rigenerazioni interrotte DPF Astra-J","Coun.inter.DPF","223047","A*256+B","0","65535","conteggi","7E0"

PID: 223278
Voller Name: Durchschnittliche Distanz zwischen Reerationen
Abkürzung: Avg Dist between Regenerations
Maximum: 10000
Gerätetyp: km
Gleichung: (((A<8)+B )<8)+C

PID: 223279
Nome completo: temperatura DPF stimata
Abbreviazione: Temp. DPF
Massimo: 1000,0
Tipo di dispositivo °C
Equazione: A*5-40
Il valore era costante a 132 (132*5 -40 = 620) prima della combustione, poi costante a 121 (121*5-40 = 565). Il valore per "Temperatura catalizzatore (banco 1 sensore 2)" era 580 - 620°C durante la cottura. Devo tenere d'occhio una cosa.



*/

//int32_t v;
  //v = deviceELM327.processPID(SERVICE_22, 0x3278, 1, 1);
  //elm327ReadFloatData("0x3278", v, ""); // 53 km (Avg Dist between Regenerations)

  //v = deviceELM327.processPID(SERVICE_22, 0x3279, 1, 1, 5, -40);
  //elm327ReadFloatData("0x3279", v, ""); // 535 C°()

  //v = deviceELM327.processPID(SERVICE_22, 0x327A, 1, 1);
  //elm327ReadFloatData("0x327A", v, ""); // 509 s(Durata media di rigenerazione DPF)

  //v = deviceELM327.processPID(SERVICE_22, 0x3047, 1, 1);
  //elm327ReadFloatData("0x3047", v, ""); // non ritorna nulla (Contatore rigenerazioni interrotte DPF)