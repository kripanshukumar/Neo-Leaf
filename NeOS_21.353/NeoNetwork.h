typedef struct _netList {
  int size;
  struct _networkInfo* head;
} netList;

netList* avl_networks = new netList{0, NULL};
netList* temp_netList = new netList{0, NULL};

#define networkLookupTimer 2000

uint32_t networkTimer = 0;
//-------------------------------------------------------------------------------------------------------------------------------
//functions
void add_network(netList* lst, networkInfo* obj);
void delete_network_list(netList* lst);
//-------------------------------------------------------------------------------------------------------------------------------

boolean checkExisting(String networkName) {
  networkInfo* netwrk = temp_netList->head;
  while (netwrk != NULL) {
    int j = 0;
    for (j = 0; j < networkName.length(); j++) {
      if (networkName[j] != netwrk->Name[j]) {
        break;
      }
    }
    if (j >= networkName.length()) {
      return 1;
    }
    netwrk = netwrk->next;
  }
  return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------
void get_online_networks(ssdpList* devlist) {
  if(devlist == NULL){
    return;
  }
  if (millis() - networkTimer > networkLookupTimer) {
    if (networkStatus == 1) {
      String payload = "";
      ssdpDevice* device = devlist->head;
      while (device != NULL) {
        String address = "http://" + device->host.toString() + netInfoAddress ;
        debugln("\n-----------------------------------------------------------------------------");
        debug("Reaching out to: "); debugln(address);
        Serial.println("Running..");
        http.begin(address.c_str());
        http.addHeader("Cache-Control", "max-age=0");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.addHeader("Connection", "close");
        int httpCode = http.POST("NETWORK=_SELF_");

        if (httpCode > 0) {
          // Serial.printf("[HTTP] GET... code: %d\n", httpCode);
          // file found at server
          if (httpCode == HTTP_CODE_OK) {
            payload = http.getString();
            int commaIndex[6] = {0};
            commaIndex[0] = payload.indexOf(',');
            for (int i = 1; i < 6; i++) {
              commaIndex[i] = payload.indexOf(',', commaIndex[i - 1] + 1);
            }
            if (checkExisting(payload.substring(0, commaIndex[0]))) {
              device = device->next;
              continue;
            }
            networkInfo* temp = new networkInfo{'\0', '\0', '\0', 0, 0, 0, 0, NULL};
            memcpy(&temp->Name, payload.c_str(), commaIndex[0]);
            temp->Name[commaIndex[0]] = '\0';
            temp->operatingVoltage = (payload.substring(commaIndex[0] + 1, commaIndex[2])).toInt();
            memcpy(&temp->utilityCompany, &payload[commaIndex[1] + 1], 3);
            temp->utilityCompany[3] = '\0';
            memcpy(&temp->planRate, &payload[commaIndex[2] + 1], 3);
            temp->planRate[3] = '\0';
            temp->planLimit = (payload.substring(commaIndex[3] + 1, commaIndex[4])).toInt();
            temp->userLimit = (payload.substring(commaIndex[4] + 1, commaIndex[5])).toInt();
            temp->useNetParam = 0;
            //            LINEDASH;
            //            outlnF("Values received:\n");                                     //Print the received value on serial monitor
            //            outF("Network----------> "); outln(temp->Name);
            //            outF("AIVBox-----------> "); outln(temp->operatingVoltage);
            //            outF("UtilityCoMenu----> "); outln(temp->utilityCompany);
            //            outF("RatePlanMenu-----> "); outln(temp->planRate);
            //            outF("PPDLBox----------> "); outln(temp->planLimit);
            //            outF("UPDTBox----------> "); outln(temp->userLimit);
            //            outF("USE NETWORK PARM-> "); outln(temp->useNetParam);
            //            LINEDASH;

            add_network(temp_netList, temp);
          }
        } else {
          Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        while (tcp_tw_pcbs != NULL) {
          tcp_abort(tcp_tw_pcbs);
        }
        device = device->next;
        delay(10);
      }
      String temp = String(Active_Network.Name);
      if (checkExisting(temp) == 0) {
        networkInfo* temp = new networkInfo{'\0', '\0', '\0', 0, 0, 0, 0, NULL};
        memcpy(temp,&Active_Network,sizeof(networkInfo));
        add_network(temp_netList, temp);
      }
      //(Active_Network)
      delete_network_list(avl_networks);
      avl_networks->head = temp_netList->head;
      avl_networks->size = temp_netList->size;

      temp_netList->size = 0;
      temp_netList->head = NULL;
    }
    networkTimer = millis();
  }
}

void add_network(netList* lst, networkInfo* obj) {
  //Serial.print(lst->size); Serial.print('\t'); Serial.println(int(lst->head));
  if (lst->size >= 0) {
    networkInfo* temp;
    if (lst->size > 0) {
      temp = lst->head;
      for (int i = 1; i < lst->size; i++) {
        temp = temp->next;
      }
      temp->next = obj;
    }
    else {
      if (lst->size == 0) {
        lst->head = obj;
      }
    }
    lst->size += 1;
  }
}

void delete_network_list(netList* lst) {
  if (lst->size > 0) {
    networkInfo* temp = lst->head;
    networkInfo* toDelete = temp;
    for (int i = 0; i < lst->size; i++) {
      //Serial.print(i); Serial.print('\t'); Serial.println(int(temp));
      temp = temp->next;
      delete toDelete;
      toDelete = temp;
    }
    //delete lst;
    lst->size = 0;
    lst->head = NULL;
  }
}
