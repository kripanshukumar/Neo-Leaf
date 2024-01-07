//====================================================================================================================================
void initializeParameters() {
  // Derive the device ID from the MAC address
  DEVICE_ID = DEVICE_MAC;                                         // Copy the MAC address to the ID string
  DEVICE_ID.replace(":", "");                                     // Remove the colons from the ID string
  DEVICE_ID.remove(0, 6);                                         // Keep the last six unique identifying characters

  // Basic string combination to create a unique, device specific AP SSID
  AP_SSID = String(ProdShortcode) + "_" + DEVICE_ID;              // Create a unique AP SSID from "NEO" and the Device ID

  // Turn on built-in LED (by pulling it LOW) to indicate Neo power ON
  pinMode(LED_BUILTIN, OUTPUT);                                   // Initialize the LED_BUILTIN pin as an output
  SysLEDOn;                                                       // Turn ON Built-In LED

  //Dump general information to serial monitor
  DLINEDASH;                                                      // Print dashed line
  outF("Welcome to "); outln(FW_Version);  // Print welcome message and firmware version to serial output
  outF("Serial Debugging: ");                                     // Print serial debugger status
#ifdef ENABLE_DEBUG                                             // If deugging is enabled print "ACTIVE" else "DISABLED"
  outlnF("ENABLED");                                              //..
#else                                                           //..
  outlnF("DISABLED");                                             //..
#endif                                                          //..
  outF("Device ID: ");                                            // Print Device ID to serial output
  outln(DEVICE_ID);                                               // Print Device ID to serial output
  outF("Device MAC: ");                                           // Print Device MAC to serial output
  outln(DEVICE_MAC);                                               // Print Device MAC to serial output

  CONFIG_SSDP(2000);

  LINEDASH;                                                       // Print dashed line
}

//------------------------------------------------------------------------------------------------------------------------------------

void FirstRunCheck() {                                              // START OF FIRSTRUNCHECK() FUNCTION
  // This function checks to see if this is Neo's first run

  // FirstRun = 0 if this device is new or has been hard reset
  FirstRun = checkFirstRun();              // Load FirstRun value from SPIFFS. FirstRun = 0 on a true first run or post hard reset.
  outF("First Run: "); outln(FirstRun);    // DEBUG: Used to determine value of FirstRun for debugging

  if (FirstRun == 0) {
    outlnF("This device has not been set up yet.");
    outlnF("Setup will begin momentarily...");
    run_AP_Rountine();
  }
  else if (FirstRun == 1) {
    //getCredentials();
    outlnF("This device has completed set up.");
    outlnF("Continuing NeOS initialization...");
    if (connectWiFi() == false) {
      run_AP_Rountine();                                        // Initialize the WIFI as Access Point
    }
  }
  else {
    outlnF("ERROR A01: FirstRun Value Out Of Range");
    outlnF("Error may indicate data corruption.");
    LINEBREAK;

    commintFirstRun(0);
    outlnF("Value has been reset. Device will now restart...");
    delay(2500);
    ESP.restart();
  }
}

void update_power_consumptions() {
  self.state = deviceState;
  self.powerConsumption = localPowerConsumption;
  self.netPowerConsumption = netPowerConsumption;
}

//------------------------------------------------------------------------------------------------------------------------------------

networkInfo* get_Network_Pointer(String networkName) {
  networkInfo* netwrk = avl_networks->head;
  while (netwrk != NULL) {
    int j = 0;
    for (j = 0; j < networkName.length(); j++) {
      if (networkName[j] != netwrk->Name[j]) {
        break;
      }
    }
    if(j >= networkName.length()){
      return netwrk;
    }
    netwrk = netwrk->next;
  }
  return NULL;
}

String hasParam(const char* param, String lookup[][2]){
  for(int i = 0; i<20; i++){
    if(lookup[i][0] == param){
      return lookup[i][1];
    }
  }
  return "";
}
  //------------------------------------------------------------------------------------------------------------------------------------
