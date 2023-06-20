// Include project libraries
#include <ESP8266WiFi.h> // ESP8266 WiFi Library
#include <WiFiClientSecure.h> // Secure WiFi Client Library
#include <ESP8266HTTPClient.h> // HTTP Client Library
#include "time.h" // Time Library
#include <Wire.h> // I2C Library
#include <DFRobot_MLX90614.h> // IR Sensor Library

// API Certificate
const char IRG_Root_X1 [] PROGMEM = R"CERT("Certificate")CERT";

// List with the server certificate
X509List cert(IRG_Root_X1);

// Wi-Fi credentials
const char* ssid = ""; // SSID for the WiFi
const char* password = ""; // Password for the WiFi
const char* url = "URL"; // Server URL to Request

// Define Esp-01 code
const String code = "Q3BAIPZUAV";

// Define IR Thermometer
#define MLX90614_I2C_ADDR 0x5A
DFRobot_MLX90614_I2C mlx(MLX90614_I2C_ADDR, &Wire);

// Call setup() function
void setup() {
  // Begin I2C
  Wire.begin(2,0);

  // Begin Serial Monitor
  Serial.begin(115200);
  Serial.println();
  Serial.println("=====Green-Stay-Rewards-GSR=====");

  // Call connectWiFiNTP() function to connect to WiFi and NTP server
  connectWiFiNTP();

  // Begin IR Thermometer
  mlx.begin();
}

// Call loop() function
void loop() {
  // Get temperatures
  float objectTemp = mlx.getObjectTempCelsius();
  float roomTemp = mlx.getAmbientTempCelsius();
  delay(500);

  // Show data to the serial monitor
  Serial.println("================================");
  Serial.print("Object Temp: ");
  Serial.print(objectTemp);
  Serial.println(" °C");
  Serial.print("Room Temp: ");
  Serial.print(roomTemp);
  Serial.println(" °C");

  // Create data and make request to the server
  String data = String("&objectTemp=" + String(objectTemp) + "&roomTemp=" + String(roomTemp));
  httpsRequest(data);

  Serial.println("Waiting 5 seconds...");
  delay(5000);
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
