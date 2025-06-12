### WIFI credentials:
Wifi credentials können über die #defines `WIFI_SSDI`und `WIFI_PASSWORDWIFI_PASSWOR` in `main.cpp` vorgegeben werden. 
#### Credentials über Umgebungsvariable
Bei öffentlichen Projekten will man seine WiFi Credentials natürlich nicht direkt im Source Code haben, sie aber auch nicht jedes mal nach einem `git pull` oder vor einem `git commit` ersetzen. Um das zu umgeben, können mit Compile-Flags in `platformio.ini` die defines auf die entsprechenden System-Umgebungsvariablen gesetzt werden: 
```
    -DWIFI_SSID=\"${sysenv.WIFI_SSID}\"
    -DWIFI_PASSWORD=\"${sysenv.WIFI_PASSWORD}\"
```
Diese Umgebungsvariablen können beispielsweise in der `.bashrc` so gesetzt werden:
```bash
export WIFI_SSID="DeinNetzwerkName"
export WIFI_PASSWORD="DeinPasswort"
``` 