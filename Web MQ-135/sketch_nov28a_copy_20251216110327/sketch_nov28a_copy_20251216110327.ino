#include <WiFi.h>
#include <HTTPClient.h>

/* ===== WIFI ===== */
const char* ssid = "ALFATIH";
const char* password = "1234abcd";

/* ===== FIREBASE (ASIA REGION) ===== */
const char* FIREBASE_URL =
"https://mq-135-b46df-default-rtdb.asia-southeast1.firebasedatabase.app/mq135.json";

/* ===== MQ-135 ===== */
#define MQ135_PIN 34

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== ESP32 MQ-135 START ===");

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi CONNECTED");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi FAILED");
  }
}

void loop() {
  int raw = analogRead(MQ135_PIN);

  // estimasi sederhana (BUKAN ilmiah)
  int ppm = map(raw, 0, 4095, 0, 1000);

  String status;
  if (ppm < 300) status = "BAIK";
  else if (ppm < 600) status = "SEDANG";
  else status = "BURUK";

  Serial.println("----- MQ-135 DATA -----");
  Serial.print("Raw ADC   : "); Serial.println(raw);
  Serial.print("PPM Est   : "); Serial.println(ppm);
  Serial.print("Status    : "); Serial.println(status);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(FIREBASE_URL);
    http.addHeader("Content-Type", "application/json");

    String json =
      "{"
      "\"raw\":" + String(raw) + "," +
      "\"ppm\":" + String(ppm) + "," +
      "\"status\":\"" + status + "\"," +
      "\"waktu_ms\":" + String(millis()) +
      "}";

    int httpCode = http.PUT(json);
    Serial.print("Firebase code: ");
    Serial.println(httpCode);

    http.end();
  }

  Serial.println("-----------------------\n");
  delay(3000);
}
