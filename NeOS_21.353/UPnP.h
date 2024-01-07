//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define debug(...) Serial.print(__VA_ARGS__)
#define debugln(...) Serial.println(__VA_ARGS__)
#else
#define debug(...)
#define debugln(...)
#endif

#define UDP_TX_RESPONSE_MAX_SIZE 4192
#define UDP_TX_PACKET_MAX_SIZE 1000

WiFiUDP UDP;

//=====================================UPNP STRUCTURES==================================================

typedef struct _ssdpDevice {
  IPAddress host;
  int port;
  String path;
  _ssdpDevice* next;
} ssdpDevice;

typedef struct _ssdpList {
  _ssdpDevice* head;
  _ssdpDevice* tail;
  int getSize;
} ssdpList;

ssdpList* deviceList;
ssdpList* tempList;
//======================================UPNP VARAIBALES================================================
const char SSDP_PACKET_TEMPLATE[] =
  "M-SEARCH * HTTP/1.1\r\n"
  "HOST: 239.255.255.250:1900\r\n"
  "MAN: \"ssdp:discover\"\r\n"
  "MX: 1\r\n"
  "ST: ssdp:all\r\n"
  "USER-AGENT: neoLeaf/1.0 UPnP/2.0 TinyUPnP/1.0\r\n"
  "\r\n";

String DeviceUrl = "";
String manufacturerURL = "http://www.noxlabs.com";
String SerialNumber;
String modelURL = "http://www.modelURL.com";
String manufacturer = "NOX LABS";
char* SSDP_SCHEMA_TEMPLATE = (char*)malloc(sizeof(char) * 1500);

char* responseBuffer;
bool UPnP_BROADCAST = 0;
uint64_t UPnP_TIMER = 0;
uint16_t UPnP_TIMEOUT = 3000;

IPAddress ipMulti(239, 255, 255, 250);
IPAddress ipNull(0, 0, 0, 0);

HTTPClient http;

//==========================================FUNCTIONS====================================================

ssdpList* createList();
void addToList(ssdpList* list, ssdpDevice* device);
void printSSDPList(ssdpList* obj);
void deleteList(ssdpList* obj);
ssdpDevice* IF_VALID_NEOLEAF_DEVICE(ssdpDevice* device);

//====================================================================================================================================================

IPAddress getHost(String url) {
  IPAddress result(0, 0, 0, 0);
  if (url.indexOf(F("https://")) != -1) {
    url.replace("https://", "");
  }
  if (url.indexOf(F("http://")) != -1) {
    url.replace("http://", "");
  }
  int endIndex = url.indexOf('/');
  if (endIndex != -1) {
    url = url.substring(0, endIndex);
  }
  int colonsIndex = url.indexOf(':');
  if (colonsIndex != -1) {
    url = url.substring(0, colonsIndex);
  }
  result.fromString(url);
  return result;
}

//====================================================================================================================================================

int getPort(String url) {
  int port = -1;
  if (url.indexOf(F("https://")) != -1) {
    url.replace("https://", "");
  }
  if (url.indexOf(F("http://")) != -1) {
    url.replace("http://", "");
  }
  int portEndIndex = url.indexOf("/");
  if (portEndIndex == -1) {
    portEndIndex = url.length();
  }
  url = url.substring(0, portEndIndex);
  int colonsIndex = url.indexOf(":");
  if (colonsIndex != -1) {
    url = url.substring(colonsIndex + 1, portEndIndex);
    port = url.toInt();
  } else {
    port = 80;
  }
  return port;
}

//====================================================================================================================================================

String getPath(String url) {
  if (url.indexOf(F("https://")) != -1) {
    url.replace("https://", "");
  }
  if (url.indexOf(F("http://")) != -1) {
    url.replace("http://", "");
  }
  int firstSlashIndex = url.indexOf("/");
  if (firstSlashIndex == -1) {
    debug(F("ERROR: Cannot find path in url ["));
    debug(url);
    debugln(F("]"));
    return "";
  }
  return url.substring(firstSlashIndex, url.length());
}

//====================================================================================================================================================

void CONFIG_SSDP(uint16_t timeout) {
  if (timeout > 200) {
    UPnP_TIMEOUT = timeout;
  }
  server.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest * request) {
    SSDP.schema(SSDP_SCHEMA_TEMPLATE);
    request->send_P(200, "text/html", SSDP_SCHEMA_TEMPLATE);
  });
  
  SerialNumber = WiFi.macAddress();
  SerialNumber.replace(":", "");
  SerialNumber.toLowerCase();
  
  Serial.printf("Starting SSDP...\n");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName("Neo Leaf");
  SSDP.setSerialNumber(SerialNumber);
  SSDP.setURL(DeviceUrl);
  SSDP.setModelName("NL");
  SSDP.setModelNumber("101");
  SSDP.setModelURL(modelURL);
  SSDP.setManufacturer(manufacturer);
  SSDP.setManufacturerURL(manufacturerURL);
  SSDP.begin();

  deviceList = createList();
  tempList = createList();
}

//====================================================================================================================================================

ssdpDevice* LOOK_FOR_SSDP_RESPONSE() {
  int packetSize = UDP.parsePacket();

  // only continue if a packet is available
  if (packetSize <= 0) {
    return NULL;
  }

  IPAddress remoteIP = UDP.remoteIP();
  IPAddress gatewayIP = WiFi.gatewayIP();
  // only continue if the packet was received from the gateway router
  // for SSDP discovery we continue anyway


  debug(F("Received packet of size ["));
  debug(String(packetSize));
  debug(F("]"));
  debug(F(" ip ["));
  for (int i = 0; i < 4; i++) {
    debug(String(remoteIP[i]));  // Decimal
    if (i < 3) {
      debug(F("."));
    }
  }
  debug(F("] port ["));
  debug(String(UDP.remotePort()));
  debugln(F("]"));

  // sanity check
  if (packetSize > UDP_TX_RESPONSE_MAX_SIZE) {
    debug(F("Received packet with size larged than the response buffer, cannot proceed."));
    return NULL;
  }

  int idx = 0;
  while (idx < packetSize) {
    responseBuffer = (char*)malloc(sizeof(char)*(packetSize + 1));
    if(responseBuffer == 0){
      debugln("Buffer creation failed...");
      return 0;
    }
    memset(responseBuffer, 0, packetSize + 1);
    int len = UDP.read(responseBuffer, packetSize );
    //int len = 0;
    if (len <= 0) {
      break;
    }
    debug(F("UDP packet read bytes ["));
    debug(String(len));
    debug(F("] out of ["));
    debug(String(packetSize));
    debugln(F("]"));
    //memcpy(responseBuffer + idx, packetBuffer, len);
    idx += len;
  }
  responseBuffer[idx] = '\0';

  debugln(F("Gateway packet content:"));
  debugln(responseBuffer);

  String location = "";
  char* location_indexStart = strstr(responseBuffer, "location:");
  if (location_indexStart == NULL) {
    location_indexStart = strstr(responseBuffer, "Location:");
  }
  if (location_indexStart == NULL) {
    location_indexStart = strstr(responseBuffer, "LOCATION:");
  }
  if (location_indexStart != NULL) {
    location_indexStart += 9;  // "location:".length()
    char* location_indexEnd = strstr(location_indexStart, "\r\n");
    if (location_indexEnd != NULL) {
      int urlLength = location_indexEnd - location_indexStart;
      int arrLength = urlLength + 1;  // + 1 for '\0'
      // converting the start index to be inside the packetBuffer rather than responseBuffer
      char locationCharArr[arrLength];
      memcpy(locationCharArr, location_indexStart, urlLength);
      locationCharArr[arrLength - 1] = '\0';
      location = String(locationCharArr);
      location.trim();
    } else {
      debugln(F("ERROR: could not extract value from LOCATION param"));
      free(responseBuffer);
      return NULL;
    }
  } else {
    debugln(F("ERROR: LOCATION param was not found"));
    free(responseBuffer);
    return NULL;
  }

  debug(F("Device location found ["));
  debug(location);
  debugln(F("]"));

  IPAddress host = getHost(location);
  int port = getPort(location);
  String path = getPath(location);

  ssdpDevice* newDevice = new ssdpDevice;
  newDevice->host = host;
  newDevice->port = port;
  newDevice->path = path;
  newDevice->next = NULL;
  free(responseBuffer);
  return newDevice;
}

//====================================================================================================================================================

void print_SSDP_List(ssdpList* obj) {
  if (obj->getSize > 0) {
    Serial.println("Devices in the List are: ");
    ssdpDevice* temp = obj->head;
    for (int i = 0; i < obj->getSize; i++) {
      Serial.print(i);
      Serial.print(F(". IP Address: ")); Serial.print(temp->host);
      Serial.print(F(" PORT: ")); Serial.print(temp->port);
      Serial.print(F(" PATH: ")); Serial.println(temp->path);
      temp = temp->next;
    }
  }
  else {
    Serial.println("The List is Empty...");
  }
}

ssdpList* createList() {
  ssdpList* temp = new ssdpList;
  temp->head = NULL;
  temp->tail = NULL;
  temp->getSize = 0;
  return temp;
}

void deleteListElements(ssdpList* obj) {
  if (obj->getSize > 0) {
    ssdpDevice* toDelete = obj->head;
    ssdpDevice* temp;// = toDelete->next;
    for (int i = 0; i < obj->getSize; i++) {
      temp = toDelete->next;
      delete toDelete;
      toDelete = temp;
    }
    obj->head = NULL;
    obj->tail = NULL;
    obj->getSize = 0;
  }
}

void addToList(ssdpList* list, ssdpDevice* device) {
  if(device == NULL){
    return;
  }
  if (list->getSize == 0) {
    list->head = device;
    device->next = NULL;
    list->tail = device;
    list->getSize += 1;
  }
  else {
    ssdpDevice* temp = list->tail;
    temp->next = device;
    device->next = NULL;
    list->tail = device;
    list->getSize += 1;
  }
}

void add_with_new_node(ssdpList* list, ssdpDevice* device){
  if(device == NULL){
    return;
  }
  ssdpDevice* toAdd = new ssdpDevice;
  toAdd->host =  device->host;
  toAdd->port =  device->port;
  toAdd->path =  device->path;
  toAdd->next =  device->next;
  
  addToList(list,toAdd);
}

//=================================================================================================================================================

ssdpList* SSDP_UPDATE_LIST() {

  if (UPnP_BROADCAST == 0) {
    while (!UDP.beginMulticast(WiFi.localIP(), ipMulti, 0)) {
      delay(5);
    }
    debugln(F("\n[SSDP] Multicast Group Joined Successfully!"));
    debugln(F("[SSDP} Sending M-SERCH Packet..."));
    UDP.beginPacketMulticast(ipMulti, 1900, WiFi.localIP());
    UDP.print(SSDP_PACKET_TEMPLATE);
    int endPck = UDP.endPacket();
    debug(F("[SSDP] Packet Send Status: [")); debug(endPck); debugln("]");
    debugln(SSDP_PACKET_TEMPLATE);
    UPnP_BROADCAST = 1;
  }
  else {

    if ((millis() - UPnP_TIMER <= UPnP_TIMEOUT) && (UPnP_BROADCAST == 1)) {
      ssdpDevice* newDevice = LOOK_FOR_SSDP_RESPONSE();
      if (newDevice != NULL) {
        ssdpDevice* temp = tempList->head;
        int exist = 0;
        for(int i = 0; i<tempList->getSize; i++){
          if(temp->host == newDevice->host){
            debugln("Device already exists...");
            exist = 1;
          }
          temp = temp->next;
        }
        if(exist == 0){
          addToList(tempList, newDevice);
        }
      }
    }
    else {
      deleteListElements(deviceList);
      ssdpDevice* newDevice = tempList->head;
      ssdpDevice* temp;
      for (int i = 0; i < tempList->getSize; i++) {
        temp = IF_VALID_NEOLEAF_DEVICE(newDevice);
        if (temp != NULL) {
           add_with_new_node(deviceList, temp);
        }
        newDevice = newDevice->next;
        if (newDevice == NULL) {
          break;
        }
        delay(10);
      }
      
      deleteListElements(tempList); 
      tempList->head = NULL;
      tempList->tail = NULL;
      tempList->getSize = 0;

      print_SSDP_List(deviceList);
      UPnP_BROADCAST = 0;
      UPnP_TIMER = millis();
      UDP.stop();
    }
  }
  return deviceList;
}

ssdpDevice* IF_VALID_NEOLEAF_DEVICE(ssdpDevice* device) {
  if (device == NULL) {
    return NULL;
  }
  String payload;

  String address = "http://" + device->host.toString() + ":" + String(device->port) + String(device->path) ;
  debugln("\n-----------------------------------------------------------------------------");
  debug("Reaching out to: "); debugln(address);

  http.begin(address.c_str());
  http.setTimeout(500);

  int httpCode = http.GET();

  if (httpCode > 0) {
    // Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
      debugln(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  int indices[5] {0};
  indices[0] = 14 + payload.indexOf("<friendlyName>", 0);
  indices[1] = payload.indexOf("</friendlyName>", indices[0]);
  indices[2] = payload.indexOf("<serialNumber>", indices[1] );
  indices[3] = payload.indexOf("</serialNumber>", indices[2]);
  indices[4] = payload.indexOf("</UDN>", indices[3]);
  int found = 0;
  if ((indices[0] > 0) && (indices[1] > 0) && (indices[2] > 0) && (indices[3] > 0) && (indices[4] > 0)) {
    String fName = payload.substring(indices[0], indices[1]);
    String serialNumber = payload.substring(indices[3] - 6, indices[3]);
    String uuid = payload.substring(indices[4] - 6, indices[4]);
    debug("Friendly Name: "); debugln(fName);
    debug("Serial Number: "); debugln(serialNumber);
    debug("UUID : "); debugln(uuid);
    if (fName == String("Neo Leaf")) {
      if (serialNumber == uuid) {
        debugln("Valid Neo Leaf device found...");
        found = 1;
      }
    }
  }
  debugln("\n-----------------------------------------------------------------------------");
  if (found) {
    return device;
  }
  else {
    return NULL;
  }
}
