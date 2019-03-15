#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

//DHT temp sensor
#include "DHT.h"

#ifndef STASSID
#define STASSID "archimedes"
#define STAPSK  "syncmaster923nw"
#endif

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


#define DEGREE '\xB0'

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

#define ESP12LED 2
#define NODEMCULED LED_BUILTIN

#define LED ESP12LED

float temp_celsius, humidity_percent;
float temp_index_celsius;

boolean read_temperature() {
  humidity_percent = dht.readHumidity();
  temp_celsius = dht.readTemperature();
  if (isnan(humidity_percent) || isnan(temp_celsius)) {
    // Check if any reads failed and exit early (to try again).
    return false;
  }
  const bool isFahrenheit = false;
  temp_index_celsius = dht.computeHeatIndex(temp_celsius , humidity_percent, isFahrenheit);
  return true;
}

void handleRoot() {
  digitalWrite(LED, LOW);
  boolean result = read_temperature();
  const int MAX_SIZE = 255;
  StaticJsonBuffer<MAX_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["t"] = temp_celsius;
  root["h"] = humidity_percent;
  root["ti"] = temp_index_celsius;

  char buffer[MAX_SIZE];
  root.printTo(buffer);
  Serial.println(buffer);
  server.send(200, "application/json", buffer);
  digitalWrite(LED, HIGH);
}

void postTemperature() {
  digitalWrite(LED, LOW);
  boolean result = read_temperature();
  const int MAX_SIZE = 255;
  StaticJsonBuffer<MAX_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = temp_celsius;
  root["humidity"] = humidity_percent;

  char buffer[MAX_SIZE];
  root.printTo(buffer);
  Serial.println(buffer);

  Serial.println("Connecting to thingsboard");
  HTTPClient http;
  http.begin("http://192.168.0.103:8080/api/v1/DHT11_DEMO_TOKEN/telemetry");
  Serial.println("Setting header");
  http.addHeader("Content-Type", "application/json");
  //send the payload
  Serial.println("POSTING buffer...");
  int httpCode = http.POST(buffer);
  Serial.print("Result HTTP Code: ");
  Serial.println(httpCode);
  http.end();
  //respond to caller
  digitalWrite(LED, HIGH);
}

void handleExport() {
  postTemperature();
  server.send(200, "text/plain", "OK");

}

void handleNotFound() {
  digitalWrite(LED, LOW);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED, HIGH);
}

void setup(void) {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/export", handleExport);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

const int delay_ms = 10000;
int lastMeasurement = 0;

void measureIfNeeded() {
  int currentTime = millis();
  if (currentTime >= (lastMeasurement + delay_ms ) || currentTime < lastMeasurement) {
    //delay expired or time rolled over - measure now
    lastMeasurement = currentTime;
    postTemperature();
  }
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  //handle a job every n seconds
  measureIfNeeded();
}
