//======================================================================================================================================================
void initStorage() {                                      // Function to initialize the flash storage
  if (!SPIFFS.begin()) {                                  // Initialize the SPI file system
    outlnF("SPIFFS Mount Failed");                        // If initialization unsuccessful the format the SPI file system
    formatStorage();                                      // Perform the format operation
  }
  else {                                                  // If the initialization was successful
    storageMount = true;                                  // Switch the storageMount flag to true
    outlnF("SPIFFS Mount Successfull........");           // Serial print the Mount successful message
  }
  LINEDASH;                                               // Single line dash
  checkFiles();                                           // Check the availability of the required files in the SPI files system
  LINEDASH;                                               // Single line dash
  getCredentials();                                       // Fetch the wifi credentials from the file stored in the flash
  FirstRun = checkFirstRun();                             // Fetch the first run variable from the file in flash
  load_device_parameters();                               // Fetch the device parameters from the file in flash
  //load_network_parameters();
}

//------------------------------------------------------------------------------------------------------------------------------------

void checkFiles() {                                             // Function to check for the availability of required files
  if (storageMount == true) {                                   // If the storage is mounted then
    outlnF("Check for all the required files in the flash:");   // Print the message to serial terminal
    int sizeofArray = sizeof(files);                            // To calculate the total number of files which we will be looking in the flash
    int numberOfFiles = sizeofArray / sizeof(const char*);      // To calculate the total number of files which we will be looking in the flash
    // "files[]" variable is the char pointer array to the files names
    for (int i = 0; i < numberOfFiles; i++) {                   // Iterate to check the files in the flash
      File file = SPIFFS.open(files[i], "r");                   // Open the file from the flash (Load data from flash to ram)
      if (file) {                                               // If file opened successfully then
        outln("Found: " + String(files[i]));                    // Serial print the file name with the "found" tag
        file.close();                                           // Close the file
      }
      else {                                                    // If the file was not opened then probabbly it is not in flash
        outln("Creating: " + String(files[i]));                 // Serial print the filename with "creating" tag showing the creation of the file
        file = SPIFFS.open(files[i], "w");                      // Create the file with required name
        if (file) {                                             // If file created then
          outlnF("File created successfully...");               // Serial print the successful message
          file.close();                                         // Close the file after the creation
        }
        else {                                                  // If file was not created then
          outlnF("Unable to create the file...");               // Serial print the message
        }
      }
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

byte checkFirstRun() {                                          // Function to fetch the first run variable
  byte val = 0;                                                 // temporary variable to store the value retrived from the file
  if (storageMount == true) {                                   // If the storage is mounted
    File file = SPIFFS.open(files[0], "r");                     // Open the file in reading mode
    if (file) {                                                 // If file is opened then
      String temp = file.readString();                          // copy the file data into a local string variable
      val = temp.toInt();                                       // convert the string value to the int
      file.close();                                             // Close the file to release the resources
    }
    else {                                                      // If file was not opened then
      outlnF("Unable to open: "); Serial.println(files[0]);     // Serial print the message
    }
    return val;                                                 // return the temporary variable
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

void commintFirstRun(byte stat) {                               // Function to sotre the First run value to the firstrun.txt
  if (storageMount == true) {                                   // Check if the storage is mounted
    if (stat > 0) {
      stat = 1;
    }
    else {
      stat = 0;
    }
    File file = SPIFFS.open(files[0], "w");                     // Open the firstrun.txt to write the firstrun value
    if (file) {
      file.println(String(stat));
      file.close();
      outlnF("commit: FirstRun");
    }
    else {
      outlnF("Commint Failed..");
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

void commintWiFiCredentials(String id, String password) {      // Function to store the wifi credentials in the flash storage
  if (storageMount == true) {                                  // Check if the storage is mounted
    File file = SPIFFS.open(files[2], "w");                    // Open the credentials.txt to write the data
    if (file) {                                                // If the file is opened successfully
      file.println(id);                                        // Write the WiFi SSID file
      file.println(password);                                  // Write the password to the file
      file.close();                                            // Close the file after writing
      outlnF("commit: WiFi Credentials");
    }
    else {
      outlnF("Commint Failed..");
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

void getCredentials() {                                            // Function to retrive the wifi credentials from the file in flash
  if (storageMount == true) {                                      // Check if the storage is already mounted
    File file = SPIFFS.open(files[2], "r");                        // Open the credentials.txt in reading mode
    if (file) {                                                    // If file opened successfully
      WiFi_SSID = file.readStringUntil('\n');                      // Read the SSID from the file
      WiFi_SSID = WiFi_SSID.substring(0, WiFi_SSID.length() - 1);  // remove the last redundant character from the end
      WiFi_PASS = file.readStringUntil('\n');                      // Read the password from the file
      WiFi_PASS = WiFi_PASS.substring(0, WiFi_PASS.length() - 1);  // remove the last redundant character from the end
      file.close();                                                // Close the file after reading
      debugln(F("Credentials obtained from file: "));              // Debugging messages
      debug(F("WIFI SSID: ")); debugln(WiFi_SSID);                 // Debugging messages
      debug(F("WIFI_PASSWORD: ")); debugln(WiFi_PASS);             // Debugging messages
    }
    else {                                                         // If file was not opened
      outlnF("Unable to open the file for reading Credentials.");
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

void commit_device_parameters() {                                 // Function to store the device self parameters in the flash
  if (storageMount == true) {                                     // Check if the storage is already mounted
    File file = SPIFFS.open(parametersFile, "w");                 // Open the parameters file in writing mode
    if (file) {                                                   // If the file is opened
      uint8_t* temp = (uint8_t*)&self;                            // Kind of char/ascii pointer to the the device parameter structure
      for (int i = 0; i < sizeof(_neoDevices); i++) {             // Write the data one char/ascii at a time
        file.write(*temp);                                        // Write operation
        temp++;                                                   // increment the pointer variable
      }
      file.close();                                               // close the file
      outlnF("COMMIT PARAMETERS: SUCCESSFUL");
    }
    else {                                                        // If file was not opened
      outF("Error opening: "); outln(parametersFile);
    }
  }
}

void load_device_parameters() {                                   // Function to load the device self parameters from the file in flash
  if ((storageMount == true) && (FirstRun == 1)) {                // Check if the storage is mounted and first run is set
    File file = SPIFFS.open(parametersFile, "r");                 // Open the file in reading mode
    if (file) {                                                   // If the file is opened
      uint8_t* temp = (uint8_t*)&self;                            // Pointer to the self parameter structure's begning
      while (file.available()) {                                  // run while file is available
        *temp = file.read();                                      // store the date at the location pointed by the pointer
        temp++;                                                   // Increment the pointer
      }
      file.close();                                               // Close the file
      outlnF("PARAMETERS LOADED FROM FLASH");
    }
    else {                                                        // If the file was not opened
      outF("Error opening: "); outln(parametersFile);
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

void resetParameters() {                                           // This function is executed when the reset button is pressed from the web interface
  if (storageMount == true) {
    outlnF("Erasing the data of all files in the flash:");
    int sizeofArray = sizeof(files);
    int numberOfFiles = sizeofArray / sizeof(const char*);
    for (int i = 0; i < numberOfFiles; i++) {
      File file = SPIFFS.open(files[i], "w");
      if (file) {
        outln("Erased: " + String(files[i]));
        file.close();
      }
      else {
        outln("Creating: " + String(files[i]));
        file = SPIFFS.open(files[i], "w");
        if (file) {
          outlnF("File created successfully...");
          file.close();
        }
        else {
          outlnF("Unable to create the file...");
        }
      }
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------

void commit_network_parameters() {
  if (storageMount == true) {                                     // Check if the storage is already mounted
    File file = SPIFFS.open(networkInfoFile, "w");                 // Open the parameters file in writing mode
    if (file) {                                                   // If the file is opened
      uint8_t* temp = (uint8_t*)&Active_Network;                            // Kind of char/ascii pointer to the the device parameter structure
      for (int i = 0; i < sizeof(_networkInfo); i++) {             // Write the data one char/ascii at a time
        file.write(*temp);                                        // Write operation
        temp++;                                                   // increment the pointer variable
      }
      file.close();                                               // close the file
      outlnF("COMMIT NETWORK PARAMETERS: SUCCESSFUL");
    }
    else {                                                        // If file was not opened
      outF("Error opening: ");outln(networkInfoFile);
    }
  }
}

void load_network_parameters() {                                   // Function to load the device self parameters from the file in flash
  if ((storageMount == true) && (FirstRun == 1)) {                // Check if the storage is mounted and first run is set
    File file = SPIFFS.open(networkInfoFile, "r");                    // Open the file in reading mode
    if (file) {                                                   // If the file is opened
      uint8_t* temp = (uint8_t*)&Active_Network;                            // Pointer to the self parameter structure's begning
      while (file.available()) {                                  // run while file is available
        *temp = file.read();                                      // store the date at the location pointed by the pointer
        temp++;                                                   // Increment the pointer
      }
      file.close();                                               // Close the file
      outlnF("NETWORK PARAMETERS LOADED FROM FLASH");
    }
    else {                                                        // If the file was not opened
      outF("Error opening: "); outln(networkInfoFile);
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------

void storenetworkDevices(neoDevice* dev) {
  neoDevice* temp = dev;
  File file = SPIFFS.open(networkDevices, "w");
  if (file) {
    Serial.println("File opened successfully");
    while (temp != NULL) {
      if (file.write((const uint8_t *)temp, sizeof(neoDevice))) {
        Serial.println("The device param saved successfully");
      }
      else {
        Serial.println("Failed to write the devices params to file");
      }
      temp = temp->next;
    }
  }
  else {
    Serial.println("Failed to open the file");
  }
  file.close();
}

neoDevice* retrive_network_devices() {
  File file = SPIFFS.open(networkDevices, "r");
  if (file) {
    int iteration = 0;
    if (file.size() > sizeof(neoDevice)) {
      neoDevice* head = (neoDevice *)calloc(1, sizeof(neoDevice));
      neoDevice* temp = head;
      for (int i = 0; i < file.size() / sizeof(neoDevice); i++) {
        file.read((uint8_t *)temp, sizeof(neoDevice));
        if (temp->next != NULL) {
          temp->next = (neoDevice *)calloc(1, sizeof(neoDevice));
          temp = temp->next;
        }
        else {
          return head;
        }
      }
    }

    Serial.println();
  }
  else {
    Serial.println("Failed to open the file for reading");
  }
  file.close();
  return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------

void formatStorage() {                                                // Function to format the SPI file system
  if (SPIFFS.format())
  {
    outlnF("File system Formatted");
  }
  else
  {
    outlnF("File system formatting error");
  }
}

//------------------------------------------------------------------------------------------------------------------------------------
