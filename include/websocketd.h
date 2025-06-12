#pragma once

// websocket broadcast alle 3 Sekunden
const unsigned long websocket_update_interval = 3000;

void setup_websocketd();
void loop_websocketd();
void websocket_update();