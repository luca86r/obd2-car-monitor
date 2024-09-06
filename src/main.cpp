#include "Preferences.h"
#include "config.h"
#include "BluetoothManager.h"
#include "ELM327Manager.h"
#include "DisplayManager.h"

// === Preferences ===
Preferences preferences;

// === Bluetooth ===
BluetoothManager bluetoothManager;

// === ELM327 ===
ELM327Manager ELM327Manager;

// === Display ===
DisplayManager displayManager;

void oledPrintData() {

  String lines = "";

  lines.concat("battery: ");
  lines.concat(ELM327Manager.getBatteryVoltage());
  lines.concat(" v");
  lines.concat("\n");
  
  lines.concat("EGR cmd: ");
  lines.concat(ELM327Manager.getCommandedEGR());
  lines.concat(" %");
  lines.concat("\n");
  
  lines.concat("egrError: ");
  lines.concat(ELM327Manager.getEgrError());
  lines.concat(" %");
  lines.concat("\n");
  
  lines.concat("manifoldPressure: ");
  lines.concat(ELM327Manager.getManifoldPressure());
  lines.concat(" kPa");
  lines.concat("\n");
  
  lines.concat("dpfDirtLevel: ");
  lines.concat(ELM327Manager.getDpfDirtLevel());
  lines.concat(" %");
  lines.concat("\n");

  lines.concat("kmsSinceDpf: ");
  lines.concat(ELM327Manager.getKmsSinceDpf());
  lines.concat(" km");
  lines.concat("\n");

  lines.concat("regenerationStatus: ");
  lines.concat(ELM327Manager.getRegenerationStatus());
  lines.concat(" %");

  displayManager.printText(lines);
}

void readAndPrintAllData() {

  ELM327Manager.readAllData();
  oledPrintData();
}

void setup()
{
    Serial.begin(115200);

    // Bluetooth init
    bluetoothManager.init();

    // Display init
    displayManager.init();
}

void loop() {

  if (!bluetoothManager.isConnected()) {

    displayManager.loadingAnimation();

    // If BT is not connected, forse EML327 init
    ELM327Manager.resetInitState();
  }

  bluetoothManager.checkOrConnect();
  
  if (bluetoothManager.isConnected()) {
    ELM327Manager.checkOrInit(bluetoothManager.getBtSerial());

    if (ELM327Manager.isInitialized()) {

      readAndPrintAllData();
    }
  }
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