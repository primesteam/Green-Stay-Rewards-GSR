// Include all needed libraries
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include "time.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// API Certificate
const char IRG_Root_X1 [] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
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
6yDUjDrT
-----END CERTIFICATE-----

)CERT";

// Create a list with the server certificate
X509List cert(IRG_Root_X1);

// Wi-Fi credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Server URL to Request
const char* url = "https://gsr.labaki.gr/api/gsr.php";

// Define Esp-01 code
const String code = "JFFW66FKIX";

// Define Thermometer
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Define Flow Sensor
const int flowPin = 0;

// Call setup() function
void setup() {
  // Set flowPin to Input
  pinMode(flowPin, INPUT);

  // Begin Serial Monitor
  Serial.begin(115200);
  Serial.println();
  Serial.println("=====Green-Stay-Rewards-GSR=====");

  // Call connectWiFiNTP() function to connect to WiFi and NTP server
  connectWiFiNTP();

  // Begin thermometer sensor
  sensors.begin();
}

// Call loop() function
void loop() {
  // Get data from flow sensor
  int flow = pulseIn(flowPin, HIGH);
  float flowRate = calculateFlowRate(flow);
  delay(500);

  // Get data from thermometer sensor
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  delay(500);

  // Show data to the serial monitor
  Serial.println("================================");
  Serial.print("Flow: ");
  Serial.print(flowRate);
  Serial.println(" L/min");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  // Create data and make request to the server
  String data = String("&waterFlow=" + String(flowRate) + "&waterTemp=" + temp);
  httpsRequest(data);

  Serial.println("Waiting 5 seconds...");
  delay(5000);
}

// Create calculateFlowRate() function to get flow rate
float calculateFlowRate(int flow) {
  // Convert pulse duration to flow rate
  float flowRate = flow / 7.5;
  return flowRate;
}

// Create httpsRequest() function to send data to the api
void httpsRequest(String data) {
  // Create WiFi client
  WiFiClientSecure client;

  // Wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    // Apply certificate
    client.setTrustAnchors(&cert);
    // Create HTTPS client
    HTTPClient https;

    // Begin HTTPS
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(client, url + String("?code=") + code + data)) {
      Serial.print("[HTTPS] GET...\n");
      // Start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // File found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      // End request
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}

// Create connectWiFiNTP() function to connect to the WiFi and set Esp01 time via NTP server
void connectWiFiNTP() {
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("================================");

  // Set time via NTP
  configTime(3 * 3600, 0, "gr.pool.ntp.org", "ntp.grnet.gr");
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}
