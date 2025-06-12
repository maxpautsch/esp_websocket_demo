#pragma once

void setup_httpd();
void loop_httpd();
void websocket_update();

const unsigned long websocket_update_interval = 3000;
