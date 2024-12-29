#include "Preferences.h"
#include "config.h"
#include "BluetoothManager.h"
#include "ELM327Manager.h"
#include "DisplayManager.h"
#include "OneButton.h"

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
bool isEngineStarted = false;
OneButton btnMain;

// === Hardware ===
#define BUTTON_MAIN  13

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

bool isLastPid(managed_pids pid) {
  return pid == (MANAGED_PIDS_COUNT - 1);
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

void setNextPid(bool enableRotationAtTheEnd) {

  bool increase = true;
  bool rotate = isDisplayPidsRotating;
  if (enableRotationAtTheEnd) {
    if (isLastPid(currentShowedPid) && !isDisplayPidsRotating) { // Last PID and is not rotating

      // Enable rotation
      rotate = true;

      // Don't increase PID index
      increase = false;

      // Prevent immediatly rotation on next loop
      lastPidRotationMs = millis();
    }
    else {
      rotate = false;
    }
  }

  int i = (int) currentShowedPid;

  if (increase) {
    i = (abs(i + 1)) % MANAGED_PIDS_COUNT;
  }

  setCurrentPidSettings((managed_pids)i, rotate, false, true);

  // PID CAT_TEMP_B1S2, CAT_TEMP_B2S1 and CAT_TEMP_B2S2 are grouped with CAT_TEMP_B1S1; skipping..
  if (i == CAT_TEMP_B1S2 || i == CAT_TEMP_B2S1 || i == CAT_TEMP_B2S2) {
    setNextPid(enableRotationAtTheEnd);
  }
}

void toggleDisplayPidsAuto() {

  setCurrentPidSettings(currentShowedPid, false, !isDisplayPidsAuto, true);
}

String getStringDataForPid(managed_pids pid, bool prefetchNext) {

  float value = elm327Manager.getDataForPID(pid, prefetchNext);
  String sValue = "- ";

  if (value != PID_NO_VALUE) {
    sValue = String(value, elm327Manager.getDecimalPointForPID(pid));
  }

  return sValue;
}

void displayDataForPid(managed_pids pid, bool prefetchNext) {

  String sValue = getStringDataForPid(pid, prefetchNext);

  if (pid == CAT_TEMP_B1S1 || pid == CAT_TEMP_B1S2 || 
      pid == CAT_TEMP_B2S1 || pid == CAT_TEMP_B2S2) {

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
  else if (pid == MANIFOLDPRESSURE) {
    displayManager.printGaugePID(elm327Manager.getNameForPID(pid), 
                                 sValue, 
                                 elm327Manager.getUnitForPID(pid),
                                 elm327Manager.getPercentageForPID(pid));
  }
  else {

    displayManager.printSinglePID(elm327Manager.getNameForPID(pid), 
                                  sValue, 
                                  elm327Manager.getUnitForPID(pid),
                                  elm327Manager.getPercentageForPID(pid));
  }
}

void displayDataForCurrentPid(bool prefetchNext) {

  displayDataForPid(currentShowedPid, prefetchNext);
}

void displayDataForCurrentPid() {

  displayDataForCurrentPid(true);
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

    if (!isEngineStarted) {
      // When engine started, doesn't fetch RPM anymore
      isEngineStarted = elm327Manager.getDataForPID(ENG_RPM, false) > 0;
    }
    
    if (!isEngineStarted || millis() <= AUTO_DISPLAY_BATTERY_ON_START_FOR_MILLIS) {

      // On engine not started or system uptime is less than 30 seconds, display battery PID
      displayDataForPid(BATTERY_VOLTAGE, false);
      return;
    }

    bool isCoolantCold = elm327Manager.getDataForPID(ENG_COOLANT_TEMP, false) < ENGINE_COOLANT_IS_COLD_THRESHOLD;
    if (isEngineStarted && millis() > AUTO_DISPLAY_BATTERY_ON_START_FOR_MILLIS && isCoolantCold) {
      
      // On engine started and system uptime is greater than 30 seconds and engine coolant is cold, display ENG_COOLANT_TEMP PID
      displayDataForPid(ENG_COOLANT_TEMP, false);
      return;
    }
    
    // Else display "currentPid" (= last manually selected PID) without prefetching next PID
    displayDataForCurrentPid(false);
    return;
  }
  
  displayDataForCurrentPid();
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

void handleButtonMainClick() {
  setNextPid(true);
}

void handleButtonMainLongPressStart() {
  toggleDisplayPidsAuto();
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

    btnMain.setup(BUTTON_MAIN, INPUT_PULLUP, true);
    btnMain.attachClick(handleButtonMainClick);
    btnMain.attachLongPressStart(handleButtonMainLongPressStart);
}

void loop() {

  unsigned long currentMillis = millis();
  btnMain.tick();

  if (bluetoothManager.isConnected() && elm327Manager.isInitialized() && !isLoading) {
    displayData();
  }

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