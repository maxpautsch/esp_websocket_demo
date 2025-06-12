#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#include "websocketd.h"
#include "status.h"

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
        else if (strcmp(command, "invert_mode") == 0)
        {
            system_status.bool_mode = !system_status.bool_mode;
            websocket_update();
        }
        else if (strcmp(command, "set_status") == 0)
        {
            const char *status = doc["status"];
            if (status)
            {
                system_status.state = status;
                websocket_update();
            }
            else
            {
                Serial.println("No status provided for set_status command");
            }
        }
        else if (strcmp(command, "set_number") == 0)
        {
            if (doc.containsKey("number"))
            {
                system_status.number = doc["number"].as<int>();
                websocket_update();
            }
            else
            {
                Serial.println("No number provided for set_number command");
            }
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
    doc["bool_mode"] = system_status.bool_mode;
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

void setup_websocketd()
{
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop_websocketd()
{
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