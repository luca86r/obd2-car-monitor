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

// === Tasks ===
TaskHandle_t taskLoadingAnimation = NULL;
TaskHandle_t taskReadDataFromELM327 = NULL;

// === Other ===
unsigned long lastEndLoop = 0;
const unsigned long LOOP_DELAY = 500;
const unsigned long READ_ELM327_DATA_DELAY = 50;
bool isLoading = true;
bool isStoppingLoadingAnimation = false;
managed_pids currentShowedPid = BATTERY_VOLTAGE;

void oledPrintCurrentPidData() {

  String sValue = String(ELM327Manager.getDataForPID(currentShowedPid), 
                         ELM327Manager.getDecimalPointForPID(currentShowedPid));

  displayManager.printSinglePID(ELM327Manager.getNameForPID(currentShowedPid), 
                                sValue, 
                                ELM327Manager.getUnitForPID(currentShowedPid));
}

void taskReadDataFromELM327Func( void * parameter) {

  unsigned long lastLoopMillis = 0;
  unsigned long currentMillis = 0;

  for(;;) {

    currentMillis = millis();
 
    if ((currentMillis - lastLoopMillis) < READ_ELM327_DATA_DELAY) {

      if (DEBUG_MODE) {
        Serial.println("taskReadDataFromELM327Func() is skipping for delay...");
      }
      continue;
    }

    ELM327Manager.readAllData();
    lastLoopMillis = millis();
  }
}

void startReadDataFromELM327Async() {

  if (taskLoadingAnimation != NULL) {
    Serial.println("Task taskReadDataFromELM327 already running? Skipping start...");
    return;
  }

  Serial.println("Starting task taskReadDataFromELM327...");

  xTaskCreatePinnedToCore(
      taskReadDataFromELM327Func, /* Function to implement the task */
      "taskReadDataFromELM327", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &taskReadDataFromELM327,  /* Task handle. */
      0); /* Core where the task should run */

  Serial.println("Started task taskReadDataFromELM327!");
}

void stopReadDataFromELM327Async() {

  if (taskReadDataFromELM327 != NULL) {

    Serial.println("Stopping task taskReadDataFromELM327...");
    vTaskDelete(taskReadDataFromELM327);
    taskReadDataFromELM327 = NULL;
    Serial.println("Stopped task taskReadDataFromELM327!");

    return;
  }

  Serial.println("Task taskReadDataFromELM327 already stopped? Skipping stop...");
}

void taskLoadingAnimationFunc( void * parameter) {
  while (!isStoppingLoadingAnimation) {
    displayManager.loadingAnimation();
  }
  while (true) {};
}

void startLoadingAnimationAsync() {

  if (taskLoadingAnimation != NULL) {
    Serial.println("Task taskLoadingAnimation already running? Skipping start...");
    return;
  }

  Serial.println("Starting task taskLoadingAnimation...");
  isStoppingLoadingAnimation = false;

  xTaskCreatePinnedToCore(
      taskLoadingAnimationFunc, /* Function to implement the task */
      "taskLoadingAnimation", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &taskLoadingAnimation,  /* Task handle. */
      0); /* Core where the task should run */

  Serial.println("Started task taskLoadingAnimation!");
}

void stopLoadingAnimationAsync() {

  if (taskLoadingAnimation != NULL) {
    
    Serial.println("Stopping task taskLoadingAnimation...");
    isStoppingLoadingAnimation = true;
    delay(1000);
    vTaskDelete(taskLoadingAnimation);
    taskLoadingAnimation = NULL;
    Serial.println("Stopped task taskLoadingAnimation!");
    return;
  }

  Serial.println("Task taskLoadingAnimation already stopped? Skipping stop...");
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

  unsigned long currentMillis = millis();
  /*
  Serial.print("currentMillis: ");
  Serial.println(currentMillis);

  unsigned long diff = (currentMillis - lastEndLoop);
  Serial.print("lastEndLoop: ");
  Serial.println(lastEndLoop);
  Serial.print("diff: ");
  Serial.println(diff);
  */
  if ((currentMillis - lastEndLoop) < LOOP_DELAY) {

    if (DEBUG_MODE) {
      Serial.println("loop() is skipping for delay...");
    }
    return;
  }

  if (!bluetoothManager.isConnected()) {

    isLoading = true;

    stopReadDataFromELM327Async();
    startLoadingAnimationAsync();

    // If BT is not connected, forse EML327 init
    ELM327Manager.resetInitState();
  }

  bluetoothManager.checkOrConnect();
  
  if (bluetoothManager.isConnected()) {

    ELM327Manager.checkOrInit(bluetoothManager.getBtSerial());

    if (ELM327Manager.isInitialized()) {

      if (isLoading) {
        isLoading = false;
        stopLoadingAnimationAsync();
        startReadDataFromELM327Async();
      }
      else {
        oledPrintCurrentPidData();
      }
    }
  }

  lastEndLoop = millis();
}