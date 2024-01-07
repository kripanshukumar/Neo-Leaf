//void checkforUpdates() {
//  if (networkStatus == true) {
//    String payload;
//    debugln("\n-----------------------------------------------------------------------------");
//    //debug("Reaching out to: "); debugln(versionCheckAddress);
//
//    //http.begin(versionCheckAddress);
//
//     http.addHeader("Content-Type", "text/plain");
//
//    int httpCode = http.POST("basic");
//
//    if (httpCode > 0) {
//      // Serial.printf("[HTTP] GET... code: %d\n", httpCode);
//      // file found at server
//      if (httpCode == HTTP_CODE_OK) {
//        payload = http.getString();
//        //debugln(payload);
//      }
//    } else {
//      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//    }
//
//    http.end();
//  }
//}

void runUpdate(){
  if ((WiFi.status() == WL_CONNECTED)) {

    t_httpUpdate_return ret = ESPhttpUpdate.update("http://192.168.18.196:1234/storage/emulated/0/espLocalUpdate/NeOS_21.299.ino.nodemcu.bin");
    //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
}
