#include <WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"


const long channelNumber = 3551173;            
const char* apiKey       = "MY_API_KEY"; 
const char* fullName     = "Jana_Jawabreh";
const char* uniID        = "12324675";


const char* WIFI_Name = "Wokwi-GUEST";
const char* WIFI_Pass = "";

#define DHTPIN 15
#define DHTTYPE DHT22
#define LED_PIN 23
#define TEMP_THRESHOLD 30.0

DHT dht(DHTPIN, DHTTYPE);
WiFiClient clientObject;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_Name, WIFI_Pass, 6);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Status: Not Connected");
    delay(1000);
  }
  Serial.println("Status: Connected");

  ThingSpeak.begin(clientObject);
}

void loop() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  
  if (temp > TEMP_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, hum);

  int stat = ThingSpeak.writeFields(channelNumber, apiKey);

  if (stat == 200) {
    Serial.printf("[%s | ID: %s] Temp: %.1f C, Hum: %.1f %% -> Data Sent to ThingSpeak\n",
                  fullName, uniID, temp, hum);
  } else {
    Serial.printf("Send failed, HTTP code: %d\n", stat);
  }

  delay(20000);
}