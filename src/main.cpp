#include "Preferences.h"
#include "config.h"
#include "BluetoothManager.h"
#include "ELM327Manager.h"
#include "DisplayManager.h"

// === Preferences ===
Preferences preferences;
#define PREF_CURRENT_SHOWED_PID "ShowedPid"
#define PREF_DISPLAY_PIDS_ROTATING "PidsRotating"
#define PREF_DISPLAY_PIDS_AUTO "PidsAuto"

// === Bluetooth ===
BluetoothManager bluetoothManager;

// === ELM327 ===
ELM327Manager elm327Manager;

// === Display ===
DisplayManager displayManager;

// === Tasks ===
TaskHandle_t taskLoadingAnimation = NULL;
TaskHandle_t taskReadDataFromELM327 = NULL;

// === Other ===
unsigned long lastEndLoop = 0;
bool isLoading = true;
bool isStoppingLoadingAnimation = false;
managed_pids currentShowedPid = BATTERY_VOLTAGE;
bool isDisplayPidsRotating = false;
unsigned long lastPidRotationMs = 0;
bool isDisplayPidsAuto = false;

// === Hardware ===
#define BUTTON_PREV  13
#define BUTTON_NEXT  14

void savePreferences() {
  preferences.putInt(PREF_CURRENT_SHOWED_PID, currentShowedPid);
  preferences.putBool(PREF_DISPLAY_PIDS_ROTATING, isDisplayPidsRotating);
  preferences.putBool(PREF_DISPLAY_PIDS_AUTO, isDisplayPidsAuto);
}

void setCurrentPidSettings(managed_pids pid, bool pidsRotating, bool pidsAuto, bool savePreference) {
  currentShowedPid = pid;
  isDisplayPidsRotating = pidsRotating;
  isDisplayPidsAuto = pidsAuto;
  displayManager.setLoopIndicator(isDisplayPidsRotating);
  displayManager.setAutoIndicator(isDisplayPidsAuto);

  if (savePreference) {
    savePreferences();
  }
}

void loadPreferences() {
  managed_pids pid = (managed_pids)preferences.getInt(PREF_CURRENT_SHOWED_PID);
  bool pidsRotating = preferences.getBool(PREF_DISPLAY_PIDS_ROTATING);
  bool pidsAuto = preferences.getBool(PREF_DISPLAY_PIDS_AUTO);

  setCurrentPidSettings(pid, pidsRotating, pidsAuto, false);
}

void setPrevPid() {

  int i = (int) currentShowedPid;

  if (i == 0) {
    i = MANAGED_PIDS_COUNT - 1;
  }
  else {
    i = (abs(i - 1)) % MANAGED_PIDS_COUNT;
  }

  setCurrentPidSettings((managed_pids)i, isDisplayPidsRotating, isDisplayPidsAuto, true);

  // PID CAT_TEMP_B1S2, CAT_TEMP_B2S1 and CAT_TEMP_B2S2 are grouped with CAT_TEMP_B1S1; skipping..
  if (i == CAT_TEMP_B1S2 || i == CAT_TEMP_B2S1 || i == CAT_TEMP_B2S2) {
    setPrevPid();
  }
}

void setNextPid(bool enableRotationAndAutoAtTheEnd) {

  bool increase = true;
  bool rotate = isDisplayPidsRotating;
  bool automatic = isDisplayPidsAuto;
  if (enableRotationAndAutoAtTheEnd) {
    if (currentShowedPid == (MANAGED_PIDS_COUNT - 1) && !isDisplayPidsRotating) { // Last PID and not rotating
      
      // Enable rotation
      rotate = true;

      // Don't increase PID index
      increase = false;

      // Prevent immediatly rotation on next loop
      lastPidRotationMs = millis();
    }
    else if (!isDisplayPidsAuto) { // Last PID, not rotating and not auto

      // Enable automatic mode
      automatic = true;

      // Don't increase PID index
      increase = false;
    }
    else {
      rotate = false;
      automatic = false;
    }
  }

  int i = (int) currentShowedPid;

  if (increase) {
    i = (abs(i + 1)) % MANAGED_PIDS_COUNT;
  }

  setCurrentPidSettings((managed_pids)i, rotate, automatic, true);

  // PID CAT_TEMP_B1S2, CAT_TEMP_B2S1 and CAT_TEMP_B2S2 are grouped with CAT_TEMP_B1S1; skipping..
  if (i == CAT_TEMP_B1S2 || i == CAT_TEMP_B2S1 || i == CAT_TEMP_B2S2) {
    setNextPid(enableRotationAndAutoAtTheEnd);
  }
}

String getStringDataForPid(managed_pids pid, bool prefetcNext) {

  float value = elm327Manager.getDataForPID(pid, prefetcNext);
  String sValue = "- ";

  if (value != PID_NO_VALUE) {
    sValue = String(value, elm327Manager.getDecimalPointForPID(currentShowedPid));
  }

  return sValue;
}

void displayCurrentPidData() {

  String sValue = getStringDataForPid(currentShowedPid, true);

  if (currentShowedPid == CAT_TEMP_B1S1 || currentShowedPid == CAT_TEMP_B1S2 || 
      currentShowedPid == CAT_TEMP_B2S1 || currentShowedPid == CAT_TEMP_B2S2) {

    displayManager.print4PID("Catalyst",
                              elm327Manager.getNameForPID(CAT_TEMP_B1S1), 
                              getStringDataForPid(CAT_TEMP_B1S1, true),
                              elm327Manager.getUnitForPID(CAT_TEMP_B1S1),
                              elm327Manager.getNameForPID(CAT_TEMP_B1S2), 
                              getStringDataForPid(CAT_TEMP_B1S2, true),
                              elm327Manager.getUnitForPID(CAT_TEMP_B1S2),
                              elm327Manager.getNameForPID(CAT_TEMP_B2S1), 
                              getStringDataForPid(CAT_TEMP_B2S1, true),
                              elm327Manager.getUnitForPID(CAT_TEMP_B2S1),
                              elm327Manager.getNameForPID(CAT_TEMP_B2S2), 
                              getStringDataForPid(CAT_TEMP_B2S2, true),
                              elm327Manager.getUnitForPID(CAT_TEMP_B2S2));
  }
  else if (currentShowedPid == MANIFOLDPRESSURE) {
    displayManager.printGaugePID(elm327Manager.getNameForPID(currentShowedPid), 
                                 sValue, 
                                 elm327Manager.getUnitForPID(currentShowedPid),
                                 elm327Manager.getPercentageForPID(currentShowedPid));
  }
  else {

    displayManager.printSinglePID(elm327Manager.getNameForPID(currentShowedPid), 
                                  sValue, 
                                  elm327Manager.getUnitForPID(currentShowedPid),
                                  elm327Manager.getPercentageForPID(currentShowedPid));
  }
}

void displayData() {

  bool isRegeneratingDPF = elm327Manager.getDataForPID(DPF_REGEN_STATUS, false) > 0;
  if (isRegeneratingDPF) {

    // Set display PID config without saving data
    setCurrentPidSettings(DPF_REGEN_STATUS, isDisplayPidsRotating, isDisplayPidsAuto, false);
    displayManager.printSinglePIDWithWarning(
                                elm327Manager.getNameForPID(currentShowedPid), 
                                String(elm327Manager.getDataForPID(currentShowedPid, false)), 
                                elm327Manager.getUnitForPID(currentShowedPid),
                                elm327Manager.getPercentageForPID(currentShowedPid), "DPF", "CLEANING!");
    return;
  }

  if (isDisplayPidsRotating) {

    unsigned long currentMs = millis();
    if ((currentMs - lastPidRotationMs) > DISPLAY_PIDS_ROTATION_DELAY) {
      lastPidRotationMs = millis();
      setNextPid(false);
    }
  }

  if (isDisplayPidsAuto) {

    bool isEngineStarted = elm327Manager.getDataForPID(ENG_LOAD, false) > 0;
    if (!isEngineStarted || millis() <= AUTO_DISPLAY_BATTERY_ON_START_FOR_MILLIS) {

      // On engine not started or system uptime is less than 30 seconds, display battery PID
      displayManager.printSinglePID(elm327Manager.getNameForPID(BATTERY_VOLTAGE), 
                              getStringDataForPid(BATTERY_VOLTAGE, false), 
                              elm327Manager.getUnitForPID(BATTERY_VOLTAGE),
                              elm327Manager.getPercentageForPID(BATTERY_VOLTAGE));
      
      return;
    }

    bool isOilCold = elm327Manager.getDataForPID(OIL_TEMP, false) > OIL_COLD_THRESHOLD;
    if (isEngineStarted && millis() > AUTO_DISPLAY_BATTERY_ON_START_FOR_MILLIS && isOilCold) {
      
      // On engine started and system uptime is greater than 30 seconds and Oil is cold, display OIL_TEMP PID
      displayManager.printSinglePID(elm327Manager.getNameForPID(OIL_TEMP), 
                              getStringDataForPid(OIL_TEMP, false), 
                              elm327Manager.getUnitForPID(OIL_TEMP),
                              elm327Manager.getPercentageForPID(OIL_TEMP));
      
      return;
    }
    
    // Else display current PID
  }
  
  displayCurrentPidData();
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

    elm327Manager.readAllData();
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

    // Init NVS memory for preferences
    preferences.begin("OBD2CarMonitor", false);
    loadPreferences();

    pinMode(BUTTON_PREV, INPUT_PULLUP);
    pinMode(BUTTON_NEXT, INPUT_PULLUP);
}

void loop() {

  unsigned long currentMillis = millis();

  if (bluetoothManager.isConnected() && elm327Manager.isInitialized() && !isLoading) {
    displayData();
  }


  if ((currentMillis - lastEndLoop) < LOOP_DELAY) {

    if (DEBUG_MODE) {
      Serial.println("loop() is skipping for delay...");
    }
    return;
  }

  bool prevButtonPressed = digitalRead(BUTTON_PREV) == LOW;
  bool nextButtonPressed = digitalRead(BUTTON_NEXT) == LOW;

  if (prevButtonPressed) {
    setPrevPid();
  }

  if (nextButtonPressed) {
    setNextPid(true);
  }

  if (!bluetoothManager.isConnected()) {

    isLoading = true;

    stopReadDataFromELM327Async();
    startLoadingAnimationAsync();

    // If BT is not connected, forse EML327 init
    elm327Manager.resetInitState();
  }

  bluetoothManager.checkOrConnect();
  
  if (bluetoothManager.isConnected()) {

    elm327Manager.checkOrInit(bluetoothManager.getBtSerial());

    if (elm327Manager.isInitialized()) {

      if (isLoading) {
        isLoading = false;
        stopLoadingAnimationAsync();
        startReadDataFromELM327Async();
      }
    }
  }
  
  lastEndLoop = millis();
}