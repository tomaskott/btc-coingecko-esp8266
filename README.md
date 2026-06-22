# ESP8266 Bitcoin Price Display

Arduino sketch for a LOLIN (WeMos) D1 R1 that displays the current Bitcoin
price from the CoinGecko API and the current NTP time on a 16x2 LCD.

## Requirements

- LOLIN (WeMos) D1 R1 / ESP8266
- ArduinoJson
- LiquidCrystal
- NTPClient

## Wi-Fi setup

Copy `secrets.example.h` to `secrets.h` and enter your Wi-Fi credentials:

```cpp
#pragma once

#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

The real `secrets.h` is ignored by Git and must not be committed.

## Build

Select **LOLIN (WeMos) D1 R1** in the Arduino IDE and compile
`btc_coingecco_working.ino`.
