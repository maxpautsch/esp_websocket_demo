#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

#include "httpd.h"
#include "ota.h"
#define STATUS_INIT
#include "status.h"
#undef STATUS_INIT

// ----------------------- WIFI credentials -----------------------
// #ifndef wird gebraucht um SSID und passwort auch über Umgebungsvariablen zu setzen
#ifndef WIFI_SSID
#define WIFI_SSID "FallbackSSID"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "FallbackPassword"
#endif

// ----------------------- WIFI credentials -----------------------


struct status_t system_status;


void setup() {
  init_status();

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set hostname
  WiFi.setHostname("websocketdemo");

  // Set mDNS hostname
  if (!MDNS.begin("websocketdemo")) {
    Serial.println("Error setting mDNS hostname!");
  } else {
    Serial.println("mDNS hostname set to 'websocketdemo'");
  }

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

  setup_httpd();
  setup_ota();

}

void loop(){
  if(!ota_running()) {
    loop_httpd();
  }
}

