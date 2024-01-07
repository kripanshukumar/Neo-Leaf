//====================================================================================================================================
String fetch_from_url(ssdpDevice* device) {
  if (networkStatus == 1) {
    String payload = "";
    String address = "http://" + device->host.toString() + communicationAddress ;
    debugln("\n-----------------------------------------------------------------------------");
    debug("Reaching out to: "); Serial.println(address);

    http.begin(address.c_str());
    http.addHeader("Cache-Control", "max-age=0");
    http.addHeader("Content-Type", "application/javascript");
    http.addHeader("Connection", "keep-alive");
    int httpCode = http.POST("command=basicInfo&");

    if (httpCode > 0) {
      // Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        //Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    int firstIndex = payload.indexOf(',', 0);
    //Serial.print(payload.substring(firstIndex + 1, payload.indexOf(',', firstIndex + 1))); Serial.print('\t'); Serial.println(String(self.Name));
    if (payload.substring(firstIndex + 1, payload.indexOf(',', firstIndex + 1)) == String(self.Name)) {
      Serial.println(payload);
      return payload;
    }
    else {
      return "";
    }
  }
  return "";
}

//------------------------------------------------------------------------------------------------------------------------------------

neoDevice* parseData(String data) {
  int comma_index[20] {0};
  comma_index[0] = data.indexOf(',', 0);
  int i = 1;

  while (comma_index[i - 1] != -1) {
    comma_index[i] = data.indexOf(',', comma_index[i - 1] + 1);
    i++;
  }

  neoDevice* temp = new neoDevice;
  temp->next = NULL;
  if (data.substring(0, comma_index[0]) == "SUCCESS") {
    memcpy(&temp->Name[0], data.substring(comma_index[0] + 1, comma_index[1]).c_str(),  comma_index[1] -  comma_index[0] - 1);
    temp->Name[comma_index[1] -  comma_index[0] - 1] = '\0';
    memcpy(&temp->planRate[0], data.substring(comma_index[1] + 1, comma_index[2]).c_str(),  11);
    temp->planRate[10] = '\0';
    memcpy(&temp->appliance[0], data.substring(comma_index[2] + 1, comma_index[3]).c_str(),  11);
    temp->appliance[10] = '\0';
    memcpy(&temp->utilityCompany[0], data.substring(comma_index[3] + 1, comma_index[4]).c_str(),  11);
    temp->utilityCompany[10] = '\0';
    temp->operatingVoltage = data.substring(comma_index[4] + 1, comma_index[5]).toInt();
    temp->planLimit = data.substring(comma_index[5] + 1, comma_index[6]).toInt();
    temp->userLimit = data.substring(comma_index[6] + 1, comma_index[7]).toInt();
    temp->priority = data.substring(comma_index[7] + 1, comma_index[8]).toInt();
    temp->state = data.substring(comma_index[8] + 1, comma_index[9]).toInt();
    temp->powerConsumption = data.substring(comma_index[9] + 1, comma_index[10]).toInt();;
    temp->netPowerConsumption = data.substring(comma_index[10] + 1, comma_index[11]).toInt();;
    temp->next = NULL;
  }

  //    outlnF("\n-----------------------------------------------------------------------------");
  //    outF("temp->Name----------------->");outln(temp->Name);
  //    outF("temp->planRate------------->");outln(temp->planRate);
  //    outF("temp->appliance------------>");outln(temp->appliance);
  //    outF("temp->utilityCompany------->");outln(temp->utilityCompany);
  //    outF("temp->operatingVoltage----->");outln(temp->operatingVoltage);
  //    outF("temp->planLimit------------>");outln(temp->planLimit);
  //    outF("temp->userLimit------------>");outln(temp->userLimit);
  //    outF("temp->priority------------->");outln(temp->priority);
  //    outF("temp->state---------------->");outln(temp->state);
  //    outF("temp->powerConsumption----->");outln(temp->powerConsumption);
  //    outF("temp->netPowerConsumption-->");outln(temp->netPowerConsumption);
  //    outlnF("\n-----------------------------------------------------------------------------");

  return temp;
}

//------------------------------------------------------------------------------------------------------------------------------------

void update_from_ssdp_list() {
  if (deviceList->getSize <= 0) {
    return;
  }
  outlnF("Connecting devices from the SSDP list...");
  ssdpDevice * temp;
  temp = deviceList->head;

  String data = "";

  neoDevice* dev;
  while (devicesOnNetwork != NULL) {
    dev = devicesOnNetwork;
    devicesOnNetwork = devicesOnNetwork->next;
    delete dev;
  }

  if (temp != NULL) {
    data = fetch_from_url(temp);
    devicesOnNetwork = parseData(data);
    dev = devicesOnNetwork;
    temp = temp->next;
  }

  while (temp != NULL) {
    data = fetch_from_url(temp);
    dev->next = parseData(data);
    dev = dev->next;
    temp = temp->next;
    delay(5);
  }
}

//------------------------------------------------------------------------------------------------------------------------------------
neoDevice* findNeoDevice(String devName) {
  neoDevice* dev;
  dev = devicesOnNetwork;
  //Serial.println("Looking for the device>>");
  while (dev != NULL) {
//    Serial.print(devName); Serial.println('.');
//    Serial.print(String(dev->appliance)); Serial.println('.');
    int brk = 0;
    for(int i = 0; i<devName.length(); i++){
      if(devName[i] != dev->appliance[i]){
        brk = 1;
        break;
      }
    }
    if(brk != 1){
      Serial.println("Device Found..");
      return dev;
    }
    dev = dev->next;
  }
  return NULL;
}

//------------------------------------------------------------------------------------------------------------------------------------
