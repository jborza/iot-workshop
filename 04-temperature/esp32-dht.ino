//https://thingsboard.io/docs/samples/esp32/gpio-control-pico-kit-dht22-sensor/

#include <DHTesp.h>         // DHT for ESP32 library

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD    115200

// DHT object
DHTesp dht;
// ESP32 pin used to query DHT11
#define DHT_PIN 33

// Main application loop delay
int quant = 20;

// Period of sending a temperature/humidity data.
int send_delay = 2000;

// Time passed after temperature/humidity data was sent, milliseconds.
int send_passed = 0;

// Setup an application
void setup() {
  // Initialize serial for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);

  // Initialize temperature sensor
  dht.setup(DHT_PIN, DHTesp::DHT11);
}

// Main application loop
void loop() {
  delay(quant);

  send_passed += quant;

  // Check if it is a time to send DHT11 temperature and humidity
  if (send_passed > send_delay) {

    
    TempAndHumidity lastValues = dht.getTempAndHumidity();    
    if (isnan(lastValues.humidity) || isnan(lastValues.temperature)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      // Values can be observed in the Serial plotter if separated by a comma
      Serial.print( lastValues.temperature);
      Serial.print(",");
      Serial.print(lastValues.humidity);
    }

    send_passed = 0;
  }
}