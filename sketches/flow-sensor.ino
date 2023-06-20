// Include project libraries
#include <ESP8266WiFi.h> // ESP8266 WiFi Library
#include <WiFiClientSecure.h> // Secure WiFi Client Library
#include <ESP8266HTTPClient.h> // HTTP Client Library
#include "time.h" // Time Library
#include <OneWire.h> // Single Wire Thermometer Library
#include <DallasTemperature.h> // Thermometer Data Processing Library

// API Certificate
const char IRG_Root_X1 [] PROGMEM = R"CERT("Certificate")CERT";

// List with the server certificate
X509List cert(IRG_Root_X1);

// Wi-Fi credentials
const char* ssid = ""; // SSID for the WiFi
const char* password = ""; // Password for the WiFi
const char* url = "URL"; // Server URL to Request

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
