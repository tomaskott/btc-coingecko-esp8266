#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>  // 📌 Nová knihovna pro HTTPS
#include <LiquidCrystal.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "secrets.h"

// 🔧 Nastavení WiFi
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// ✅ CoinGecko API (HTTPS verze)
const char* host = "api.coingecko.com";
const char* apiPath = "/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

// ⏳ NTP konfigurace (časová synchronizace)
const char* ntpServer = "pool.ntp.org";
const int timeZone = 1;  // UTC+2 (Česká republika)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, timeZone * 3600);

// 🖥️ Inicializace LCD displeje
#define D9 2  // GPIO2 = Arduino D9 (Wemos D1 R1)
LiquidCrystal lcd(D8, D9, D4, D5, D6, D7);
#define lcdSvit 10  // Pin pro podsvícení LCD

// 🔐 Použití WiFiClientSecure pro HTTPS
WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n🔄 Spouštění...");

  // 🌍 Připojení k WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("🌐 Připojování k WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi připojeno!");
  Serial.print("IP adresa: ");
  Serial.println(WiFi.localIP());

  // 📟 Inicializace LCD
  lcd.begin(16, 2);
  pinMode(lcdSvit, OUTPUT);
  
  // ⏳ Připojení k NTP serveru
  timeClient.begin();

  // 🛠️ ⚠️ Důležité: vypnutí ověřování SSL certifikátu
  client.setInsecure();
}

void loop() {
  getData();  // 🔄 Získání ceny Bitcoinu
  delay(10000);  // ⏳ Aktualizace každých 10 sekund
}

void getData() {
  Serial.println("\n🌍 Připojuji se k CoinGecko API...");

  if (!client.connect(host, 443)) {
    Serial.println("❌ Chyba: Nelze se připojit k API!");
    return;
  }

  // 📩 Odeslání HTTP požadavku
  client.print(String("GET ") + apiPath + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  delay(500);
  
  // 📥 Načtení odpovědi do proměnné
  String response = "";
  while (client.available()) {
    response += char(client.read());
  }
  client.stop();
  
  Serial.println("📩 Odpověď API:");
  Serial.println(response);

  // 🛠️ Najdeme začátek JSON dat
  int jsonStart = response.indexOf('{');
  if (jsonStart == -1) {
    Serial.println("❌ Chyba: JSON nezačíná znakem { !");
    return;
  }

  String jsonData = response.substring(jsonStart);
  Serial.println("📄 Extrahovaný JSON:");
  Serial.println(jsonData);

  // 📊 Parsování JSON
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, jsonData);
  
  if (error) {
    Serial.print("❌ Chyba při parsování JSON: ");
    Serial.println(error.f_str());
    return;
  }

  // 💰 Získání ceny Bitcoinu
  //float btcPrice = doc["bitcoin"]["usd"];
  int btcPrice = doc["bitcoin"]["usd"].as<int>();
  Serial.print("💰 BTC Price: $");
  Serial.println(btcPrice);

  // ⏳ Aktualizace NTP času
  timeClient.update();
  String currentTime = timeClient.getFormattedTime();
  
  // 📟 Výpis na LCD
  digitalWrite(lcdSvit, HIGH);  // Zapnutí podsvícení LCD
  lcd.setCursor(0, 0);
  lcd.print("BTC:  $");
  lcd.print(btcPrice);
  lcd.setCursor(0, 1);
  lcd.print("TIME: ");
  lcd.print(currentTime);
  digitalWrite(lcdSvit, LOW);  // Vypnutí podsvícení LCD

  delay(60000);
}
