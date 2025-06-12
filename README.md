# ESP und websocket demo
## Toolchain
### vscode / platformIO
Wir verwenden im Folgenden [VSCode](https://code.visualstudio.com/) und [platformIO](https://platformio.org) als Tools. Für die installation zuerst vscode installieren und dann über den extention manager platformIO nachinstallieren wie [hier](https://platformio.org/install/ide?install=vscode) beschrieben.

Der Vorteil von platformIO ist, dass die toolchain sowie alle libs versioniert angegeben werden können. [fill here]
### git
Dieses Projekt ist unter ... abgelegt. Um es direkt auf den Rechner zu clonen kann folgendes gemacht werden: `git clone ...`Alternativ kann man auf der Seite das Repository auch als zip file runterladen und entpacken.

## Projekt-Verzeichnise und Dateien
- `data/` Alles in diesem Ordner wird 1:1 in das Dateisystem des ESP übernommen
- `include/` [fill here]
-  `lib/` [fill here]
- `src/` [fill here]
- `platformio.ini` [fill here]
- `.gitignore` [fill here]
-  `README.md` [fill here]

## Begriffe
- OTA = over the air update [fill here]
- webserver (httpd) = 
- websocket = 

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