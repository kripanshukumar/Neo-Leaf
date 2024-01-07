//====================================================================================================================================
boolean connectWiFi() {
  LINEDASH;                                                   // Dashed line
  WiFi.disconnect();                                          // Disconnect from all WiFi modes
  delay(100);                                                 // Delay to allow the system acheive stable state
  if (WiFi_SSID.length() > 0) {                               // If SSID length is greater than 0, proceed connecting to SSID
    outlnF("Connecting to: "); outln(WiFi_SSID);              // Print the SSID to serial monitor
    WiFi.begin(WiFi_SSID.c_str(), WiFi_PASS.c_str());         // Initialize the WiFi connection using given parameters
    int i = 0;                                                // counts seconds passed from the last connection request
    while (WiFi.status() != WL_CONNECTED) {                   // Run the "While" loop till the connection is established or timeout is reached
      delay(1000);                                            // 1 second delay
      out('.');                                               // to show the connecting attempt on the serial monitor
      if (i >= CONNECTING_DURATION) {                         // If time passed since last connection attemp > allowed duration break out of while loop
        outlnF("\nUnable to Connect to the WiFi network");    // Print the unsuccessful attemp to serial monitor
        networkStatus = 0;                                    // Turn the network flag as 0
        break;                                                // Break out of while loop
      }
      i++;                                                    // Increment the time counter variable
    }
  }
  if (WiFi.status() == WL_CONNECTED) {                        // If the connection is established
    networkStatus = 1;                                        // Change the network flag to 1
    outlnF("\nWIFI CONNECTED");                               // Update the status on serial monitor
    outF("IP address: ");                                     // Print the IP address on the serial monitor
    outln(WiFi.localIP());                                    //'''''''''''''''''''''''''''''''''''''''''''

    return true;
  }
  else {                                                      // If connection attemp failed then
    return false;
  }
  LINEDASH;                                                   // Dashed line
}

//------------------------------------------------------------------------------------------------------------------------------------

void reconnectWiFi() {                                               // This function check for the disconnection or new credential update and reinitialize the WiFI
  if (newCredentials == 1) {                    // If new credentials are set and the first run  equals to 1
    if (connectWiFi() == true) {                                     // Try connecting to the new WIFI SSID with given parameters
      outlnF("Connected to the new WiFi Network...");
      // Run CommitWiFi() To Commit WiFi Credentials To EEPROM
      commintWiFiCredentials(WiFi_SSID, WiFi_PASS);
    }
    else {
      outlnF("Unable to connect to new SSID. Restoring to the previous credentials...");
      getCredentials();                                              // Get the new credentials
    }
    newCredentials = 0;                                              // Change the newCredential flag to 0.
  }
  if (WiFi.status() != WL_CONNECTED && (FirstRun == 1)) {            // If WiFi STA is not connected and first run is 1
    if (WiFi_SSID.length() > 0) {                                    // Check if the SSID name is valid
      if (millis() - lastRecoonectAttempt >= RECONNECT_DURATION) {   // If time laspsed till the last reconnect attemp made is equal or longer than min reconnect duration
        outlnF("\nConnecting to: "); outln(WiFi_SSID);                 // Print SSID on the serial monitor
        WiFi.begin(WiFi_SSID.c_str(), WiFi_PASS.c_str());            // Connect to the given SSID and PASSWORD
        int i = 0;                                                   // timer for timeout on connection attempt
        while (WiFi.status() != WL_CONNECTED) {                      // while the connection is not established
          delay(1000);                                               // delay 1 second on every loop execution
          out('.');                                                  // Show the attempt on the serial monitor
          if (i >= CONNECTING_DURATION) {                            // If connection attempt take longer than connecting duration then
            outlnF("\nUnable to Connect to the WiFi network");        // Print failed attempt to the serial monitor
            networkStatus = 0;                                       // Change the network flag to 0
            break;                                                   // Break out of while loop
          }
          i++;                                                       // Increment the timer value
        }
        if (WiFi.status() == WL_CONNECTED) {                        // If the connection is established
          networkStatus = 1;                                        // Change the network flag to 1
          outlnF("\nWIFI CONNECTED");                                 // Update the status on serial monitor
          outF("IP address: ");                                     // Print the IP address on the serial monitor
          outln(WiFi.localIP());                                    //'''''''''''''''''''''''''''''''''''''''''''
          WiFi.softAPdisconnect(true);                              // Turn off WiFi AP
        }
        lastRecoonectAttempt = millis();                            // Reset the last reconnect Attemp timer
      }
    }
  }
  if (Stop_AP == 1) {
    WiFi.softAPdisconnect(true);                                    // Turn off WiFi AP
    Stop_AP = 0;
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

boolean StartAP() {
  LINEDASH;
  WiFi.disconnect();                                                    // Disconnecte any WiFi connection (if existing)
  delay(10);
  outln("Initialzing the WiFi Access Point...");                        // Initialize the Access Point with given Parameters.
  AP_ACTIVE =  WiFi.softAP(AP_SSID.c_str(), AP_PASS);
  if (AP_ACTIVE == true) {
    LINEBREAK;                                                          // Blank line break
    outln("Access Point Is Active");                                    // The access point has now been initialized.
    AP_IP = WiFi.softAPIP().toString();                                 // Pass device's AP IP as string to AP_IP variable
    outF("AP SSID: ");                                                  // Display access point's device specific SSID via serial
    outln(AP_SSID);
    outF("AP_PASS: ");                                                  // Display generic AP password
    outln(AP_PASS);
    return true;                                                        // return TRUE as WIFI AP is initialized successfully.
  }
  else if (AP_ACTIVE == false) {
    LINEBREAK;                                                          // Blank line break
    outlnF("ERROR B01: WIFI_AP Activation Failed");                     // The access point has failed to initialized.
    return false;                                                       // Return FALSE when AP initialization failed.
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

void run_AP_Rountine() {

  byte failCount = 0;

  LINEBREAK;
  outlnF("Running WIFI AP routine...");

StartAP_L1:                                                 // StartAP() Label 2

  StartAP();                                                // "Begin" access point and pass status to AP_Active

  // Waiting for a device to connect via Neo's WiFi AP
  LINEBREAK;                                                // Blank line break
  Serial.print(FPSTR("Waiting For WiFi Connection"));       // The Neo will now wait for a WiFi connection.

  byte loopCount = 0;

  while (WiFi.softAPgetStationNum() == 0) {
    SysLEDToggle;                                           // TOGGLE Built-In LED State
    outF(".");                                              // Add a periods to indicate that the Neo is waiting
    delay(1000);                                            // Delay loop for 1 second, or 1000 milliseconds
    ++loopCount;                                            // Increments loopCount
    if (loopCount  >= 60) {
      break;
    }
  }

  if (loopCount  >= 60) {
    ++failCount;                                            // Increments the variable counting timeout failures
    if (failCount == 3) {
      LINEBREAK;
      outlnF("ERROR B03: WiFi AP Connection Time Out Limit");
      outlnF("Device Has Timed Out 3 Times During AP Connection.");
      outlnF("Please Check To Ensure All Devices Are Functional.");
      LINEBREAK;
      outlnF("This Neo Will Now Reset...");
      delay(2500);
      outlnF("DEBUG: RESET COMMAND INVOKED!");
      //ESP.restart();
    }
    else {
      // Turns on the built-in LED and makes solid
      SysLEDOff;                                                  // Turn OFF Built-In LED

      // Inform user that the WiFi AP connection attempt has timed out
      LINEBREAK;
      outlnF("ERROR B02: WiFi AP Connection Timed Out");         // Inform user of failure/timeout
      outlnF("Please Check Your Devices And Try Again");         // Error resolution suggestion

      // Cycles the WiFi AP OFF to be turned back on after StartAP_L1 label
      LINEBREAK;
      outlnF("Resetting WiFi Access Point");                     // Debug message informing user of AP reset
      WiFi.softAPdisconnect(true);                                // Turn off WiFi AP
      bool AP_Active = false;                                     // Resets bool WiFi AP activity indicator
      SysLEDOff;                                                  // Turn OFF Built-In LED

      // Add a delay to ensure the Neo completely cycles its WiFi AP OFF
      byte failLoop = 0;                                          // failLoop counts the number of loops performed
      while (failLoop <= 9) {                                     // 1 Loop = 1 second; failLoop runs for 10 seconds
        outF(".");                                 // Print a "." for visual confirmation of activity
        delay(1000);                                              // Delay loop 1 second, or 1000 milliseconds
        ++failLoop;                                               // Increment failLoop variable with each loop
      }

      // Turns on the built-in LED and makes solid
      SysLEDOn;                                                   // Turn ON Built-In LED

      // Returns to the beginning of the StartAP() function to attempt WiFi AP connection again
      goto StartAP_L1;
    }
  }
  else if (WiFi.softAPgetStationNum() >= 1) {
    // Turns on the built-in LED and makes solid
    SysLEDOn;                                                     // Turn ON Built-In LED

    // Inform user of WiFi AP status and give instructions for procedure
    LINEBREAK;
    outlnF("A WiFi AP Connection Has Been Established");
    delay(2500);
  }

}

//------------------------------------------------------------------------------------------------------------------------------------

void ServerSetup() {
  LINEBREAK;
  Serial.println(FPSTR("Initializing Server System..."));

  WiFiSetup = 0;                                                          // (Re)Set the WiFiSetup flag to 0 (FALSE)
  newCredentials = 0;

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Send a web page to client at <AP_IP>
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", "Root");
  });

  // Send web page with WiFi network input fields to client at <AP_IP>/WiFiConfig
  server.on("/WiFiConfig", HTTP_GET, [](AsyncWebServerRequest * request) {

    request->send_P(200, "text/html", HTML_WiFiConfig);

  });

  server.on("/networkSelect", HTTP_GET, [](AsyncWebServerRequest * request) {

    request->send_P(200, "text/html", HTML_NetworkSelect);

  });

  // Send a web page response to client after submitting data to <AP_IP>/WiFiConfig
  server.on("/WiFiConfig", HTTP_POST, [] (AsyncWebServerRequest * request) {

    // GET Values From Web Form
    int paramNumber = request->params();
    for (int i = 0; i < paramNumber; i++) {
      AsyncWebParameter* p = request->getParam(i);

      if (p->name() == "SSIDBox") {
        WiFi_SSID = p->value();
      }

      if (p->name() == "PSKBox") {
        WiFi_PASS = p->value();
      }
    }

    outlnF("\nTesting Provided WiFi Credentials...");

    // Print the new WiFi credentials to the serial monitor
    // (For debugging only; should not be a part of the release)
    LINEBREAK;
    outlnF("The Provided WiFi Network Credentials Are:");
    outF("SSID: ");  outln(WiFi_SSID);
    outF("PASS: ");  outln(WiFi_PASS);
    LINEBREAK;

    newCredentials = 1;

    // Send a confirmation response to the browser UI
    if (FirstRun == 0)  {
      // Sends User A Confirmation With Request To Proceed to /LeafConfig
      request->send_P(200, "text/html", HTML_WiFiConfirmFR);
    }
    else if (FirstRun == 1) {
      // Send User A Confirmation And Returns Them To /WiFiConfig
      request->send_P(200, "text/html", HTML_WiFiConfirmFR);
      request->redirect("/WiFiConfig");
    }

  });

  // Send web page with Leaf Config input fields to client at <AP_IP>/LeafConfig
  server.on("/LeafConfig", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", (const uint8_t*)&HTML_LeafConfig[0], sizeof(HTML_LeafConfig));
  });

  // Send web page with Leaf Config input fields to client at <AP_IP>/LeafConfig
  server.on("/LeafConfig", HTTP_POST, [](AsyncWebServerRequest * request) {
    // GET Values From Web Form

    int paramNumber = request->params();                                   //Get the number of parameters received in POST requrest
    for (int i = 0; i < paramNumber; i++) {                                //Iterate for all available parameters
      String temp;
      AsyncWebParameter* p = request->getParam(i);                         //Parameter handler "AsyncWebParameter* p"
      if (p->name() == "AttApplianceMenu") {                               //Check for the Parameter's name, and store the value in appropriate variable
        ApplianceType = p->value();
        temp = ApplianceType + "_" + DEVICE_ID;
        memcpy(&self.appliance, temp.c_str(), 11);
        self.appliance[10] = '\0';
      }
      if (p->name() == "AIVBox") {                                         //"
        ApplianceVin = p->value().toInt();
        self.operatingVoltage = ApplianceVin;
      }
      if (p->name() == "ApplPriorityMenu") {                               //"
        AppliancePriority = p->value().toInt();
        self.priority = AppliancePriority;
      }
      if (p->name() == "UtilityCoMenu") {                                  //"
        UtilityCompany = p->value();
        temp = UtilityCompany + "_" + DEVICE_ID;
        memcpy(&self.utilityCompany, temp.c_str(), 11);
        self.utilityCompany[10] = '\0';
      }
      if (p->name() == "RatePlanMenu") {                                   //"
        RatePlan = p->value();
        temp = RatePlan + "_" + DEVICE_ID;
        memcpy(&self.planRate, temp.c_str(), 11);
        self.planRate[10] = '\0';
      }
      if (p->name() == "PPDLBox") {                                        //"
        PeakDemandPlanLim = p->value().toInt();
        self.planLimit = PeakDemandPlanLim;
      }
      if (p->name() == "UPDTBox") {                                        //"
        PeakDemandUserLim = p->value().toInt();
        self.userLimit = PeakDemandUserLim;
      }
    }

    LINEDASH;
    outlnF("Values received:\n");                                     //Print the received value on serial monitor
    outF("AttApplianceMenu-> "); outln(self.appliance);
    outF("AIVBox-----------> "); outln(self.operatingVoltage);
    outF("ApplPriorityMenu-> "); outln(self.priority);
    outF("UtilityCoMenu----> "); outln(self.utilityCompany);
    outF("RatePlanMenu-----> "); outln(self.planRate);
    outF("PPDLBox----------> "); outln(self.planLimit);
    outF("UPDTBox----------> "); outln(self.userLimit);
    LINEDASH;
    request->send_P(200, "text/html", HTML_LeafConfig);

    commit_device_parameters();                                       // Store the self device parameters to flash

    //As the device has been configured flip the First Run flage to 1
    if (FirstRun != 1) {                                              // Check if FirstRun is already 1 else proceed
      commintFirstRun(1);                                             // Commint the first run value to the file in flash
      FirstRun = 1;                                                   // Set the first run flag as 1
      Stop_AP = 1;
    }
  });


  // Send the WiFi Config page when the reset button is pressed on the Leaf Config page
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", HTML_WiFiConfig);
    resetParameters();
    ESP.restart();
  });

  server.on("/isConnected", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (WiFi.status() == WL_CONNECTED) {
      String payload = "1,http://" + String(WiFi.localIP().toString()) + "/networkSelect," + WiFi_SSID + ',' ;
      request->send_P(200, "text/html", (const uint8_t*)payload.c_str(), payload.length());
    }
    else {
      request->send_P(200, "text/html", "0");
    }
    if (CONNECTION_TIMEOUT == 1) {
      request->send_P(200, "text/html", "-1");
    }
  });

  server.on("/selectedNetwork", HTTP_GET, [](AsyncWebServerRequest * request) {
    String networkParam;
    networkParam +=  String(Active_Network.Name) + ',';
    networkParam +=  String(Active_Network.operatingVoltage) + ',';
    networkParam +=  String(Active_Network.utilityCompany) + ',';
    networkParam +=  String(Active_Network.planRate) + ',';
    networkParam +=  String(Active_Network.planLimit) + ',';
    networkParam +=  String(Active_Network.userLimit) + ',';
    request->send_P(200, "text/html", (const uint8_t*)networkParam.c_str(), networkParam.length());
  });

  server.on("/selectedNetwork", HTTP_POST, [](AsyncWebServerRequest * request) {
    int paramNumber = request->params();                                   //Get the number of parameters received in POST requrest
    int parsed = 0;
    int flag = -1;
    for (int i = 0; i < paramNumber; i++) {                                //Iterate for all available parameters
      AsyncWebParameter* p = request->getParam(i);
      String temp = p->value();

      Serial.print(p->name()); Serial.print('\t');
      Serial.println(temp);
      if (p->name() == "flag") {
        flag = temp.toInt();
      }
      if (p->name() == "selectedNetwork") {
        if (flag == 0) {
          outlnF("Copying structure data to active network..");
          int useNTWpar = Active_Network.useNetParam;
          memcpy(&Active_Network, (void*)get_Network_Pointer(temp), sizeof(networkInfo));
          Active_Network.useNetParam = useNTWpar;
          outlnF("DONE...");
        }
        else {
          memcpy(&Active_Network.Name, temp.c_str(), temp.length());
        }
        parsed = 1;
      }

      if (p->name() == "networkBox") {
        memcpy(&Active_Network.Name, temp.c_str(), temp.length());
        parsed = 1;
      }

      if (p->name() == "AIVBox") {                                         //"
        Active_Network.operatingVoltage = temp.toInt();
      }

      if (p->name() == "UtilityCoMenu") {                                  //"
        memcpy(&Active_Network.utilityCompany, temp.c_str(), 3);
        Active_Network.utilityCompany[3] = '\0';
      }

      if (p->name() == "RatePlanMenu") {                                   //"
        memcpy(&Active_Network.planRate, temp.c_str(), 3);
        Active_Network.planRate[3] = '\0';
      }

      if (p->name() == "PPDLBox") {                                        //"
        Active_Network.planLimit = temp.toInt();
      }

      if (p->name() == "UPDTBox") {                                        //"
        Active_Network.userLimit = temp.toInt();
      }

      if (p->name() == "useNTWpar") {
        Active_Network.useNetParam = temp.toInt();
      }
    }
    memcpy(&self.Name, &Active_Network.Name, 21);
    if (Active_Network.useNetParam == 1) {
      Serial.println("The Parameters are being saved");
      String temp;
      self.operatingVoltage = Active_Network.operatingVoltage;

      temp = String(Active_Network.utilityCompany) + "_" + DEVICE_ID;
      Serial.println(temp);
      memcpy(&self.utilityCompany, temp.c_str(), 11);

      temp = String(Active_Network.planRate) + "_" + DEVICE_ID;
      Serial.println(temp);
      memcpy(&self.planRate, temp.c_str(), 11);

      self.planLimit = Active_Network.planLimit;

      self.userLimit = Active_Network.userLimit;
    }


    LINEDASH;
    outlnF("Values received:\n");                                     //Print the received value on serial monitor
    outF("Network----------> "); outln(Active_Network.Name);
    outF("AIVBox-----------> "); outln(Active_Network.operatingVoltage);
    outF("UtilityCoMenu----> "); outln(Active_Network.utilityCompany);
    outF("RatePlanMenu-----> "); outln(Active_Network.planRate);
    outF("PPDLBox----------> "); outln(Active_Network.planLimit);
    outF("UPDTBox----------> "); outln(Active_Network.userLimit);
    outF("USE NETWORK PARM-> "); outln(Active_Network.useNetParam);
    LINEDASH;
    if (parsed == 1) {
      request->send_P(200, "text/html", "SUCCESS");
      commit_network_parameters();
      if (FirstRun == 1) {
        commit_device_parameters();
      }
    }
    else {
      request->send_P(200, "text/html", "FAILED");
    }
  });

  server.on("/getNetwork", HTTP_GET, [](AsyncWebServerRequest * request) {
    String networkList = "<option class=\"option\" value=\"NONE\">NONE SELECTED (" + String(avl_networks->size) + " available)</option>\n";
    networkInfo* temp = avl_networks->head;
    const int number = avl_networks->size;
    String payload[number];                                              // Temporary string variable used to store the appliance names
    int pos[number + 1];
    for (int i = 0; i < number; i++) {
      payload[i] = temp->Name;
      temp = temp->next;
    }
    String tempName = "";
    int subPos = 1;
    for (int i = 0; i < number; i++ ) {
      for (int j = i + 1; j < number - i; j++) {
        if (payload[i][0] > payload[j][0]) {
          tempName = payload[i];
          payload[i] = payload[j];
          payload[j] = tempName;
        }
        else if (payload[i][0] == payload[j][0]) {
          subPos = 0;
          while (payload[i][subPos] == payload[j][subPos]) {
            subPos++;
            if (subPos > 8) {
              break;
            }
          }
          if (payload[i][0] > payload[j][0]) {
            tempName = payload[i];
            payload[i] = payload[j];
            payload[j] = tempName;
          }
        }
      }
    }
    for (int i = 0 ; i < avl_networks->size; i++) {
      if (payload[i] == String(Active_Network.Name)) {
        networkList += "<option style=\"background-color:YELLOW\" class=\"option\" value=\"" + String(payload[i]) + "\">" + String(payload[i]) + " (Active)</option>\n";
      }
      else {
        networkList += "<option class=\"option\" value=\"" + String(payload[i]) + "\">" + String(payload[i]) + "</option>\n";
      }
      temp = temp->next;
    }
    request->send_P(200, "text/html", (const uint8_t*)networkList.c_str(), networkList.length());
  });

  server.on("/getNetwork", HTTP_POST, [](AsyncWebServerRequest * request) {
    int paramNumber = request->params();                                   //Get the number of parameters received in POST requrest

    String networkParam;
    for (int i = 0; i < paramNumber; i++) {                                //Iterate for all available parameters
      AsyncWebParameter* p = request->getParam(i);
      String temp = p->value();
      Serial.print(p->name()); Serial.print('\t');
      Serial.println(temp);
      if (p->name() == "NETWORK") {
        networkInfo* netwrk;
        if (temp == "_SELF_") {
          netwrk = &Active_Network;
        }
        else {
          netwrk = get_Network_Pointer(temp);

        }
        networkParam +=  String(netwrk->Name) + ',';
        networkParam +=  String(netwrk->operatingVoltage) + ',';
        networkParam +=  String(netwrk->utilityCompany) + ',';
        networkParam +=  String(netwrk->planRate) + ',';
        networkParam +=  String(netwrk->planLimit) + ',';
        networkParam +=  String(netwrk->userLimit) + ',';
      }
    }
    request->send_P(200, "text/html", (const uint8_t*)networkParam.c_str(), networkParam.length());
  });


  server.on("/getParams", HTTP_GET, [](AsyncWebServerRequest * request) {
    String payload;
    payload += String(self.appliance).substring(0, 3) + ',';
    payload += String(self.operatingVoltage) + ',';
    payload += String(self.utilityCompany).substring(0, 3) + ',';
    payload += String(self.planRate).substring(0, 3) + ',';
    payload += String(self.planLimit) + ',';
    payload += String(self.userLimit) + ',';
    payload += String(self.priority) + ',';

    request->send_P(200, "text/html", (const uint8_t*)payload.c_str(), payload.length());
  });
  // Page to show other online/connected Neo Leaf devices
  server.on("/devices", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", devices_html);
  });


  // AJAX response for updating the online devices on the "/devices" webpage
  server.on("/readDevices", HTTP_GET, [](AsyncWebServerRequest * request) {

    neoDevice* temp = devicesOnNetwork;                               // Temporary pointer to neo device List's head
    String nameList = "";

    int number = 0;
    while (temp != NULL) {                                            // While loop which traverse through the neo leaf device list
      number += 1;
      temp = temp->next;                                              // Go to the next Neo Leaf device in the list
    }
    if (number > 1) {
      const int asd = number;
      String payload[asd];                                              // Temporary string variable used to store the appliance names
      int pos[asd + 1];
      temp = devicesOnNetwork;
      for (int i = 0; i < number; i++) {
        payload[i] = temp->appliance;
        temp = temp->next;
      }
      String tempName = "";
      int subPos = 1;
      for (int i = 0; i < number; i++ ) {
        for (int j = i + 1; j < number - i; j++) {
          if (payload[i][0] > payload[j][0]) {
            tempName = payload[i];
            payload[i] = payload[j];
            payload[j] = tempName;
          }
          else if (payload[i][0] == payload[j][0]) {
            subPos = 0;
            while (payload[i][subPos] == payload[j][subPos]) {
              subPos++;
              if (subPos > 8) {
                break;
              }
            }
            if (payload[i][0] > payload[j][0]) {
              tempName = payload[i];
              payload[i] = payload[j];
              payload[j] = tempName;
            }
          }
        }

      }
      for (int i = 0; i < number; i++) {
        nameList += "<li style=\"list-style: none\">"                    // Add the list item html tag to the payload
                    + payload[i]                                         // Add the Neo Leaf device name to the markup
                    + "</li>\n";                                         // Add the closing list item tag with newline
        //Serial.print(i); Serial.print(". ");
        //Serial.println(payload[i]);
      }
    }
    else {
      if (number == 1) {
        nameList += "<li style=\"list-style: none\">"                    // Add the list item html tag to the payload
                    + String(devicesOnNetwork->appliance)                                         // Add the Neo Leaf device name to the markup
                    + "</li>\n";                                         // Add the closing list item tag with newline
      }
    }
    request->send_P(200, "text/html", nameList.c_str());               // Send the prepared payload as the ajax response
  });

  

  // This end is for the inter Neo Leaf device communication.
  server.on(
    "/comm",
    HTTP_POST,
  [](AsyncWebServerRequest * request) {},
  NULL,
  [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    Serial.println("In the event handler ");
    String param[20][2] = {""};
    int j = 0;
    int l = 0;
    for (int i = 0; i < len; i++) {
      if (char(data[i]) == '&') {
        param[j][l].concat('\0');
        j++;
        l = 0;
        continue;
      }
      if (char(data[i]) == '=') {
        param[j][l].concat('\0');
        l = 1;
        continue;
      }
      param[j][l].concat(char(data[i]));
    }
    for (int i = 0; i <= j; i++) {
      Serial.print(i); Serial.print(' '); Serial.print(param[i][0]);
      Serial.print(' '); Serial.println(param[i][1]);
    }

    String message;
    if (hasParam("command", param))
    {
      //Serial.print("Found Param with name command->");
      String Command = hasParam("command", param);
      if (Command == "basicInfo") {
        command = BASIC_INFO;
        //Serial.print("BASIC_INFO->");
      }
      else {
        if (Command == "request_on") {
          command = REQUEST_ON;
          //Serial.print("REQUEST_ON->");
        }
        else {
          if (Command == "notify") {
            command = NOTIFY_CONSUMPTION;
            //Serial.print("NOTIFY_CONSUMPTION->");
          }
          else {
            if (Command == "check_parameters") {
              command = CHECK_PARAMETERS;
              //Serial.print("CHECK_PARAMETERS->");
            }
          }
        }
      }
      if (command == BASIC_INFO) {
        //Serial.print("Case->Basic_Info->");
        message = "SUCCESS,";
        message += String(self.Name) + ',';
        message += String(self.planRate) + ',';                               // Add plan rate to the data
        message += String(self.appliance) + ',';                              // Add the appliance type to the data
        message += String(self.utilityCompany) + ',';                         // Add the utility company to the data
        message += String(self.operatingVoltage) + ',';                       // Add the operating voltage to the data
        message += String(self.planLimit) + ',';                              // Add the plan Limit to the data
        message += String(self.userLimit) + ',';                              // Add the user limit to the data
        message += String(self.priority) + ',';                               // Add the device priority to the data
        message += String(self.state) + ',';                                  // Add the device state to the data
        message += String(self.powerConsumption) + ',';                       // Add the self power consumption to the Data
        message += String(self.netPowerConsumption) + ',';                    // Add the net power consumption calculated by the device to the data
        //Serial.println(params);

        //LINEDASH;
        //outlnF("Values received:\n");                                     //Print the received value on serial monitor
        //outF("AttApplianceMenu-> "); outln(self.appliance);
        //outF("AIVBox-----------> "); outln(self.operatingVoltage);
        //outF("ApplPriorityMenu-> "); outln(self.priority);
        //outF("UtilityCoMenu----> "); outln(self.utilityCompany);
        //outF("RatePlanMenu-----> "); outln(self.planRate);
        //outF("PPDLBox----------> "); outln(self.planLimit);
        //outF("UPDTBox----------> "); outln(self.userLimit);
        //outF("State------------> "); outln(self.state);
        //outF("PowerConsumption-> "); outln(self.powerConsumption);
        //outF("NPowerConsumption> "); outln(self.netPowerConsumption);
        //LINEDASH;
      }
      else {
        neoDevice* temp = NULL;
        if (hasParam("deviceName", param).length()) {
          //Serial.print("deviceName->");
          message = hasParam("deviceName", param);
          temp = findNeoDevice(message);
          if (temp == NULL) {
            message = "ERROR:DEVICE_NOT_FOUND";
            //Serial.print("ERROR:DEVICE_NOT_FOUND->");
          }
          else {
            switch (command) {
              case REQUEST_ON: {
                  //Serial.print("Case->REQUEST_ON->");
                  message = "SUCCESS,";
                  if (self.netPowerConsumption < CONSUMPTION_THRESHOLD) {
                    message += "permission=GRANTED";
                  }
                  else {
                    message += "permission=DENIED";
                  }
                  break;
                }

              case NOTIFY_CONSUMPTION: {
                  //Serial.print("Case->NOTIFY_CONSUMPTION->");
                  int numberofParams = 0;
                  if (hasParam("state", param).length()) {
                    message = hasParam("state",param);
                    temp->state = message.toInt();
                    numberofParams += 1;
                  }
                  if (hasParam("powerConsumption", param).length()) {
                    message = hasParam("powerConsumption",param);
                    temp->powerConsumption = message.toInt();
                    numberofParams += 1;
                  }
                  if (hasParam("netPowerConsumption", param).length()) {
                    message = hasParam("netPowerConsumption",param);
                    temp->netPowerConsumption = message.toInt();
                    numberofParams += 1;
                  }
                  if (numberofParams == 3) {
                    message = "SUCCESS=200";
                  }
                  else {
                    message = "ERROR=PARAMS_MISSING(" + String(3 - numberofParams) + ')';
                  }
                  break;
                }
              case CHECK_PARAMETERS: {
                  //Serial.print("Case->CHECK_PARAMETERS->");
                  message = "SUCCESS,";
                  message += String(temp->Name) + ',';
                  message += String(temp->planRate) + ',';                               // Add plan rate to the data
                  message += String(temp->appliance) + ',';                              // Add the appliance type to the data
                  message += String(temp->utilityCompany) + ',';                         // Add the utility company to the data
                  message += String(temp->operatingVoltage) + ',';                       // Add the operating voltage to the data
                  message += String(temp->planLimit) + ',';                              // Add the plan Limit to the data
                  message += String(temp->userLimit) + ',';                              // Add the user limit to the data
                  message += String(temp->priority) + ',';                               // Add the device priority to the data
                  message += String(temp->state) + ',';                                  // Add the device state to the data
                  message += String(temp->powerConsumption) + ',';                       // Add the self power consumption to the Data
                  message += String(temp->netPowerConsumption) + ',';                    // Add the net power consumption calculated by the device to the data
                  break;
                }
            }
          }
        }
        else {
          message = "ERROR:DEVICE_PARAM_NOT_FOUND";
        }
      }
      //Serial.print("Message prepared: "); Serial.println(message);
      request->send_P(200, "text/html", (const uint8_t*)message.c_str(), message.length());                  // Send the prepared data string as HTTP_POST response
    }
    else {
      request->send_P(200, "text/html", "ERROR:NO_COMMAND_GIVEN");
    }
  });

  // Starts the server.
  server.onNotFound(HTML_404);
  server.begin();

  delay(1000);

  // Debugging message to indicate the end of this function
  LINEBREAK;
  outlnF("END: ServerSetup()");
}

//------------------------------------------------------------------------------------------------------------------------------------
// Handles 404 Errors
void HTML_404(AsyncWebServerRequest * request) {
  request->send(404, "text/plain", "404: NOT FOUND");
}

//------------------------------------------------------------------------------------------------------------------------------------

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  } else if (type == WS_EVT_ERROR) {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if (type == WS_EVT_PONG) {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len) {
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if (info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0) {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len) {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if (info->final) {
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
          if (info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------
