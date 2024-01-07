//==========================////////////LIBRARIES///////////==================================
#include <FS.h>
#include <Arduino.h>
#include "lwip/tcp_impl.h"
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

//==========================//////////GLOBAL MACROS//////////==================================
// Debugging
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define debug(...) Serial.print(__VA_ARGS__)
#define debugln(...) Serial.println(__VA_ARGS__)
#else
#define debug(...)
#define debugln(...)
#endif

#define outF(...) Serial.print(F(__VA_ARGS__))
#define outlnF(...) Serial.println(F(__VA_ARGS__))

#define out(...) Serial.print(__VA_ARGS__)
#define outln(...) Serial.println(__VA_ARGS__)

//GENERAL
#define LINEBREAK Serial.println(FPSTR(""))       // Blank line break macro allows easy line breaks by a clear command
#define DLINEDASH Serial.println(F("\n============================================================================="));
#define LINEDASH Serial.println(FPSTR("\n-----------------------------------------------------------------------------"));
#define SysLEDOn digitalWrite(LED_BUILTIN, 0)
#define SysLEDOff digitalWrite(LED_BUILTIN, 1)
#define SysLEDToggle digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN))

//THRESHOLD
#define CONSUMPTION_THRESHOLD 50

//WiFi Macros
#define CONNECTING_DURATION 30
#define RECONNECT_DURATION 30000

//FILE- STORAGE PARAMETERS
#define FORMAT_SPIFFS_IF_FAILED true
boolean storageMount = false;
const char* firstRunFile = "/firstrun.txt";
const char* parametersFile = "/parameter.txt";
const char* credentialsFile = "/credentials.txt";
const char* networkInfoFile = "/network.txt";
const char* networkDevices = "/networkDevices.txt";
const char* files[] = {firstRunFile,parametersFile,credentialsFile,networkInfoFile,networkDevices};

// GLOBAL CONSTANTS
const String DEVICE_MAC = WiFi.macAddress();      // Constant for storing DEVICE MAC ADDRESS
const char*  ProductName = "Neo Leaf";     // Constant for storing the full PRODUCT NAME
const char*  ProdShortcode = "NEO";        // Constant for storing product's programmatic SHORTCODE
const char*  HW_Version = "v21.144 RC1";   // Constant for storing latest compatible HARDWARE VERSION
const char*  FW_Version = "NeOS v21.127a"; // Constant for storing the FIRMWARE VERSION
const char*  AP_PASS = "password";         // Constant for storing the ACCESS POINT PASSWORD

// Declare web server constant
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
//==========================///////////////VARIABLES//////////////================================
//WiFi Variables
String WiFi_SSID = "";
String WiFi_PASS = "";
String AP_SSID;
String AP_IP;
String DEVICE_ID;
boolean WiFiSetup;
boolean newCredentials;
boolean AP_ACTIVE = false;
uint32_t lastRecoonectAttempt = 0;
int networkStatus = 0;
boolean CONNECTION_TIMEOUT = 0;
int Stop_AP = 0;

//First Run
boolean FirstRun;                                               // Byte as boolean to indicate if device's FIRST RUN


//Parameters
String RatePlan;
String ApplianceType;
String UtilityCompany;
byte AppliancePriority;
byte PeakDemandPlanLim;
byte PeakDemandUserLim;
int ApplianceVin;

//SSDP parameters:
uint64_t t = 0;

//Other Neo Leaf devices Parameters and variables..
typedef struct _neoDevices{
  char planRate[11];
  char appliance[11];
  char Name[22];
  char utilityCompany[11];
  int operatingVoltage;
  byte planLimit;
  byte userLimit;
  byte priority;
  boolean state;
  int powerConsumption;
  int netPowerConsumption;
  struct _neoDevices* next;
} neoDevice;

const char* communicationAddress = "/comm";
neoDevice self {"","","",0,0,0,0,0,0,0,NULL};
neoDevice* devicesOnNetwork = NULL;


//Network Parameters
typedef struct _networkInfo{
  char utilityCompany[4];
  char planRate[4];
  char Name[21];
  byte useNetParam;
  byte userLimit;
  byte planLimit;
  int operatingVoltage;
  struct _networkInfo* next;
}networkInfo;

networkInfo Active_Network;
//networkInfo** networkList;
const char* netInfoAddress = "/getNetwork";

// Power Usage Variables:
int localPowerConsumption = 0;
int netPowerConsumption = 0;
boolean deviceState = false;

//Enumerations
enum commFlag{BASIC_INFO, REQUEST_ON, NOTIFY_CONSUMPTION, CHECK_PARAMETERS};

enum commFlag command;

//HTTP UPDATES variables:
//const char* versionCheckAddress = "http://192.
//==========================///////////////FUNCTIONS//////////////=================================
//Wireless.h
boolean connectWiFi();
boolean StartAP();
void run_AP_Rountine();
void ServerSetup();
void reconnectWiFi();
void HTML_404(AsyncWebServerRequest *request);
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

//SPI Flash system
void initStorage();
void checkFiles();
byte checkFirstRun();
void commintWiFiCredentials(String id, String password);
void getCredentials();
void resetParameters();
void formatStorage();
void commit_device_parameters();
void load_device_parameters();
void commit_network_parameters();
void load_network_parameters();

// General Functions
void FirstRunCheck();
void taskHandler();
networkInfo* get_Network_Pointer(String networkName);
String hasParam(const char* param, String lookup[][2]);

// UPnP Functions
void CONFIG_SSDP(uint16_t timeout);

//NeoDevices Functions
neoDevice* parseData(String data);
void update_from_ssdp_list();
neoDevice* findNeoDevice(String devName);
