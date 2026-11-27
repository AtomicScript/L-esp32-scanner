#include <Arduino.h>
#include "WiFi.h"

#define ALERT_LED 2

// Timing 
const uint16_t STRIKE_MS     = 40;  
const uint16_t PAUSE_MS      = 80;
const uint32_t SCAN_INTERVAL = 10000;


void setup() {
  Serial.begin(115200);
  pinMode(ALERT_LED, OUTPUT);
  digitalWrite(ALERT_LED, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.STA.begin();

  Serial.println("Survey Corps WiFi Protocol initiated.");
  Serial.println("Tch. Let's see how filthy the air is.");

}

// Sharp 
void alert(){
  for (uint8_t i = 0; i < 3; i++){
    digitalWrite(ALERT_LED, HIGH);  // change state of the LED by setting the pin to the HIGH voltage level
    delay(STRIKE_MS);                      // wait for a second
    digitalWrite(ALERT_LED, LOW);   // change state of the LED by setting the pin to the LOW voltage level
    delay(PAUSE_MS);                      // wait for a second
  }
}


void ScanWiFi() {
  Serial.println("\n--- Scan commencing ---");

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  int open_count = 0; 

  if (n == 0) {
    Serial.println("No networks. Clean air. Acceptable.");
    return;
  }

  Serial.printf("%d networks detected.\n", n);
  Serial.println("No. | SSID                             | RSSI | CH | Enc");

    
  for (int i = 0; i < n; ++i) {
    Serial.printf("%2d  | ", i + 1);
    Serial.printf("%-32.32s | ", WiFi.SSID(i).c_str());
    Serial.printf("%4d | ", WiFi.RSSI(i));
    Serial.printf("%2d | ", WiFi.channel(i));
      
    switch (WiFi.encryptionType(i)) {
      case WIFI_AUTH_OPEN:            Serial.print("OPEN — FILTH"); open_count++ ; break;
      case WIFI_AUTH_WEP:             Serial.print("WEP — WEAK"); break;
      case WIFI_AUTH_WPA_PSK:         Serial.print("WPA — OUTDATED"); break;
      case WIFI_AUTH_WPA2_PSK:        Serial.print("[WPA2]"); break;
      case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
      case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
      case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
      case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
      case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
      default:                        Serial.print("unknown");
    }
      Serial.println();
    }
    
    Serial.printf("\nOpen networks: %d\n", open_count);

    if (open_count > 0) {
    Serial.println("Tch. Open networks. Disgusting.");
    for (int i = 0; i < open_count; i++) {
      alert();
      delay(300);   // short pause between reports
    }
    } else {
    Serial.println("No open networks. Barely acceptable.");
    }

  WiFi.scanDelete();
  Serial.println("Scan complete. Memory cleared.");
}




void loop() {
  Serial.println("\n=====================================");
  Serial.println("Default band scan:");
  Serial.println("=====================================");

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 2)
  WiFi.setBandMode(WIFI_BAND_MODE_AUTO);
#endif
  ScanWiFi();

#if CONFIG_SOC_WIFI_SUPPORT_5G
  delay(1000);
  Serial.println("\n=====================================");
  Serial.println("2.4 GHz only:");
  Serial.println("=====================================");
  WiFi.setBandMode(WIFI_BAND_MODE_2G_ONLY);
  ScanWiFi();

  delay(1000);
  Serial.println("\n=====================================");
  Serial.println("5 GHz only:");
  Serial.println("=====================================");
  WiFi.setBandMode(WIFI_BAND_MODE_5G_ONLY);
  ScanWiFi();
#endif

  Serial.println("\nStand by. Next survey in 10 seconds.");
  delay(SCAN_INTERVAL);
}