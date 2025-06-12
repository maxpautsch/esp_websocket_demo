#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Arduino.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>

#include "httpd.h"
#include "status.h"

WebServer httpd(80);
WebSocketsServer webSocket(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

    // send message to client
    websocket_update();
  }
  break;
  case WStype_TEXT:
  {
    Serial.printf("[%u] get Text: %s\n", num, payload);
    // hier könnte man auch direkt ein strcmp machen und gar kein json übertragen!

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
      Serial.println("Failed to parse JSON");
      return;
    }

    const char *command = doc["command"];
    if (!command)
    {
      Serial.println("No command found in JSON");
      return;
    }

    if (strcmp(command, "increment") == 0)
    {
      system_status.counter++;
      websocket_update();
    }
    else if (strcmp(command, "zero") == 0)
    {
      system_status.counter = 0;
      websocket_update();
    }
    else if (strcmp(command, "getJournal") == 0)
    {
      String journalData;
      File journalFile = SPIFFS.open("/journal.txt", "r");
      if (journalFile)
      {
        while (journalFile.available())
        {
          journalData += journalFile.readString();
        }
        journalFile.close();
      }
      else
      {
        journalData = "Failed to open journal file";
        Serial.println("Failed to open journal file");
      }
      journalData.replace("\n", "<br>");
      String reply = "{\"journal\": \"" + journalData + "\"}";
      webSocket.sendTXT(num, reply);
    }
    else
    {
      Serial.print("Unknown command: ");
      Serial.println(command);
    }

    break;
  }
  case WStype_BIN:
    // Serial.printf("[%u] get binary length: %u\n", num, length);
    // hexdump(payload, length);
    //  send message to client
    // webSocket.sendBIN(num, payload, length);
    break;
  default:
    break;
  }
}

void websocket_update()
{
  StaticJsonDocument<2000> doc;
  doc["automatik"] = system_status.auto_mode;
  doc["counter"] = system_status.counter;
  doc["number"] = system_status.number;

  doc["state"] = system_status.state.c_str();

  unsigned long uptimeMillis = millis();
  unsigned long uptimeSeconds = uptimeMillis / 1000;
  unsigned long uptimeMinutes = uptimeSeconds / 60;
  unsigned long uptimeHours = uptimeMinutes / 60;
  unsigned long uptimeDays = uptimeHours / 24;

  char uptimeString[50];
  snprintf(uptimeString, sizeof(uptimeString), "%lu d, %lu h, %lu m, %lu s",
           uptimeDays, uptimeHours % 24, uptimeMinutes % 60, uptimeSeconds % 60);
  doc["uptime"] = uptimeString;

  String jsonString;
  serializeJson(doc, jsonString);
  webSocket.broadcastTXT(jsonString);
}

void handleReset()
{
  httpd.send(200, "text/plain", "Rebooting...");
  Serial.println("reboot requested\n");
  delay(1000);
  ESP.restart();
}

void handleAdd()
{
  static int add_counter = 0;
  add_counter++;

  String output = "Add counter: " + String(add_counter);
  Serial.println(output); // Optional: Ausgabe im seriellen Monitor

  httpd.send(200, "text/plain", output);
}

void loop_httpd()
{
  httpd.handleClient();
  webSocket.loop();

  // Code um die Datenstruktur alle 3 Sekunden an alle Websocket-Clients zu verteilen
  static unsigned long websocket_update_previousMillis = 0;

  unsigned long currentMillis = millis();
  if (currentMillis - websocket_update_previousMillis >= websocket_update_interval)
  {
    websocket_update();
    websocket_update_previousMillis = currentMillis;
  }
}

String getContentType(String filename)
{
  if (httpd.hasArg("download"))
  {
    return "application/octet-stream";
  }
  else if (filename.endsWith(".htm"))
  {
    return "text/html";
  }
  else if (filename.endsWith(".html"))
  {
    return "text/html";
  }
  else if (filename.endsWith(".css"))
  {
    return "text/css";
  }
  else if (filename.endsWith(".js"))
  {
    return "application/javascript";
  }
  else if (filename.endsWith(".png"))
  {
    return "image/png";
  }
  else if (filename.endsWith(".gif"))
  {
    return "image/gif";
  }
  else if (filename.endsWith(".jpg"))
  {
    return "image/jpeg";
  }
  else if (filename.endsWith(".ico"))
  {
    return "image/x-icon";
  }
  else if (filename.endsWith(".xml"))
  {
    return "text/xml";
  }
  else if (filename.endsWith(".pdf"))
  {
    return "application/x-pdf";
  }
  else if (filename.endsWith(".zip"))
  {
    return "application/x-zip";
  }
  else if (filename.endsWith(".gz"))
  {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool exists(String path)
{
  bool yes = false;
  File file = SPIFFS.open(path, "r");
  if (!file.isDirectory())
  {
    yes = true;
  }
  file.close();
  return yes;
}

bool handleFileRead(String path)
{
  // Wenn keine explizite Datei angegeben ist: index.html verwenden
  // Hier könnten auch Unterverzeichnisse auf Dateien umgeleitet werden
  if (path.endsWith("/"))
  {
    path += "index.html";
  }
  String contentType = getContentType(path);
  if (exists(path))
  {
    File file = SPIFFS.open(path, "r");
    httpd.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void setup_httpd()
{
  // Start the web server

  // Direkte URL calls bekannt machen:
  httpd.on("/add", handleAdd);
  httpd.on("/reset", handleReset);

  httpd.onNotFound([]()
                   {
    Serial.printf("Not found: %s\n", httpd.uri().c_str());
    if (!handleFileRead(httpd.uri())) {
      httpd.send(404, "text/plain", "FileNotFound");
    } });
  httpd.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
