# Code Explanation

## Files

#### Arduino Ino
* <a href="#flow-sensor">flow-sensor.ino</a>
* <a href="#ir-temperature-sensor">ir-temperature-sensor.ino</a>
#### Functions
* <a href="#calculateFlowRate">calculateFlowRate()</a>
* <a href="#gsr">httpsRequest()</a>
* <a href="#gsr">connectWiFiNTP()</a>

## Flow Sensor

### _Include Project Libraries_

```cpp
#include <ESP8266WiFi.h> // ESP8266 WiFi Library
#include <WiFiClientSecure.h> // Secure WiFi Client Library
#include <ESP8266HTTPClient.h> // HTTP Client Library
#include "time.h" // Time Library
#include <OneWire.h> // Single Wire Thermometer Library
#include <DallasTemperature.h> // Thermometer Data Processing Library
```

### _Define Certificate_

```cpp
const char IRG_Root_X1 [] PROGMEM = R"CERT("Certificate")CERT"; // API Certificate

X509List cert(IRG_Root_X1); // List with the server certificate
```

### _Define WiFi Settings_

```cpp
// Wi-Fi credentials
const char* ssid = ""; // SSID for the WiFi
const char* password = ""; // Password for the WiFi
const char* url = "URL"; // Server URL to Request
```

### _Define Sensor ID_

```cpp
const String code = "JFFW66FKIX"; // Define Esp-01 code
```

### _Define Thermometer_

```cpp
// Define Thermometer
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
```

### _Define Flow Sensor Pin_

```cpp
// Define Flow Sensor
const int flowPin = 0;
```

### _Open Setup Function_

```cpp
void setup() {
```
#### Set flowPin to Input

```cpp
  pinMode(flowPin, INPUT);
```

#### Begin Serial

```cpp
  Serial.begin(115200);
  Serial.println();
  Serial.println("=====Green-Stay-Rewards-GSR=====");
```

#### Call <a href="#connectWiFiNTP">connectWiFiNTP()</a> function

```cpp
  // Connect to WiFi and NTP server
  connectWiFiNTP();
```

#### Begin Thermometer sensor

```cpp
  sensors.begin();
}
```

### _Open Loop Function_

```cpp
void loop() {
```

#### Get data from flow sensor <a href="#calculateFlowRate">calculateFlowRate()</a>

```cpp
  int flow = pulseIn(flowPin, HIGH);
  float flowRate = calculateFlowRate(flow);
  delay(500);
```

#### Get data from thermometer sensor

```cpp
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  delay(500);
```

#### Show data to the serial monitor

```cpp
  Serial.println("================================");
  Serial.print("Flow: ");
  Serial.print(flowRate);
  Serial.println(" L/min");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");
```

#### Create data

```cpp
  String data = String("&waterFlow=" + String(flowRate) + "&waterTemp=" + temp);
```

#### Make request to the server <a href="#httpsRequest">httpsRequest()</a>

```cpp
  httpsRequest(data);
```

#### Wait 5 seconds

```cpp
  Serial.println("Waiting 5 seconds...");
  delay(5000);
}
```

## IR Thermometer Sensor

### _Include Project Libraries_

```cpp
#include <ESP8266WiFi.h> // ESP8266 WiFi Library
#include <WiFiClientSecure.h> // Secure WiFi Client Library
#include <ESP8266HTTPClient.h> // HTTP Client Library
#include "time.h" // Time Library
#include <Wire.h> // I2C Library
#include <DFRobot_MLX90614.h> // IR Sensor Library
```

### _Define Certificate_

```cpp
const char IRG_Root_X1 [] PROGMEM = R"CERT("Certificate")CERT"; // API Certificate

X509List cert(IRG_Root_X1); // List with the server certificate
```

### _Define WiFi Settings_

```cpp
// Wi-Fi credentials
const char* ssid = ""; // SSID for the WiFi
const char* password = ""; // Password for the WiFi
const char* url = "URL"; // Server URL to Request
```

### _Define Sensor ID_

```cpp
const String code = "JFFW66FKIX"; // Define Esp-01 code
```

### _Define IR Thermometer_

```cpp
#define MLX90614_I2C_ADDR 0x5A
DFRobot_MLX90614_I2C mlx(MLX90614_I2C_ADDR, &Wire);
```

### _Open Setup Function_

```cpp
void setup() {
```
#### Begin I2C

```cpp
  Wire.begin(2,0);
```

#### Begin Serial

```cpp
  Serial.begin(115200);
  Serial.println();
  Serial.println("=====Green-Stay-Rewards-GSR=====");
```

#### Call <a href="#connectWiFiNTP">connectWiFiNTP()</a> function

```cpp
  // Connect to WiFi and NTP server
  connectWiFiNTP();
```

#### Begin IR Thermometer

```cpp
  mlx.begin();
}
```

### _Open Loop Function_

```cpp
void loop() {
```

#### Get temperatures

```cpp
  float objectTemp = mlx.getObjectTempCelsius();
  float roomTemp = mlx.getAmbientTempCelsius();
  delay(500);
```

#### Show data to the serial monitor

```cpp
  Serial.println("================================");
  Serial.print("Object Temp: ");
  Serial.print(objectTemp);
  Serial.println(" °C");
  Serial.print("Room Temp: ");
  Serial.print(roomTemp);
  Serial.println(" °C");
```

#### Create data

```cpp
  String data = String("&objectTemp=" + String(objectTemp) + "&roomTemp=" + String(roomTemp));
```

#### Make request to the server <a href="#httpsRequest">httpsRequest()</a>

```cpp
  httpsRequest(data);
```

#### Wait 5 seconds

```cpp
  Serial.println("Waiting 5 seconds...");
  delay(5000);
}
```

## Functions

## calculateFlowRate Function

### _Create Function_

```cpp
float calculateFlowRate(int flow) {
```

### _Convert pulse duration to flow rate_

```cpp
  float flowRate = flow / 7.5;
  return flowRate;
}
```

## httpsRequest Function

### _Create Function_

```cpp
void httpsRequest(String data) {
```

### _Create Wi-Fi Client_

```cpp
  WiFiClientSecure client;
```

### _Check for Wi-Fi_

```cpp
    if (WiFi.status() == WL_CONNECTED) {
```

### _Apply certificate_

```cpp
      client.setTrustAnchors(&cert);
```

### _Create HTTPS client_

```cpp
      HTTPClient https;
```

### _Begin HTTPS_

```cpp
      Serial.print("[HTTPS] begin...\n");
    if (https.begin(client, url + String("?code=") + code + data)) {
```

### _Start connection and send HTTP header_

```cpp
      Serial.print("[HTTPS] GET...\n");
      int httpCode = https.GET();
```

### _Check for Error_

```cpp
      if (httpCode > 0) {
```

### _Print Error_

```cpp
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

    // File found at server
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
       String payload = https.getString();
       Serial.println(payload);
    }
```

### _Error Connecting to Server_

```cpp
  } else {
    Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
  }
```

### _End request_

```cpp
   https.end();
```

### _Error Connecting to Wi-Fi_

```cpp
   } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}
```

## connectWiFiNTP Function

### _Create Function_

```cpp
void connectWiFiNTP() {
```

### _Connect to Wi-Fi_

```cpp
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
```
### _Set time via NTP_

```cpp
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
```
