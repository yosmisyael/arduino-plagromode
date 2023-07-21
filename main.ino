#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// constants
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID      "TMPL••••••••"
#define BLYNK_TEMPLATE_NAME    "Test"
#define BLYNK_AUTH_TOKEN       "••••••••••••••••••••••••"
#define DHT_TYPE DHT22
bool autoMode = false;
bool manualPumpMode = false;
const char* SSID = ""; // write SSID here
const char* PASS = ""; // write PASS here
const int DHT_PIN = 15;
const int LCD_COLUMNS = 16;
const int LCD_ROWS = 2;
const int oneWirePin = 4;
const int ANALOG_PIN = 36;
const int LAMP_PIN = 5;
const int PUMP_PIN = 16;
const int MIN_VALUE = 310;
const int MAX_VALUE = 620;
const int WET_VALUE = 80;
const int DRY_VALUE = 40;

// instances 
DHT sensor_DHT(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C LCD(0x27, LCD_COLUMNS, LCD_ROWS);
OneWire oneWire(oneWirePin);
DallasTemperature sensor_DS18B20(&oneWire);

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LAMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, HIGH);
  digitalWrite(LAMP_PIN, HIGH);
  connectNetwork();
  Blynk.begin(BLYNK_AUTH_TOKEN, SSID, PASS);
  LCD.init();
  LCD.backlight();
  sensor_DHT.begin();
  sensor_DS18B20.begin();
}

void loop() {
  Blynk.run();

  // read sensor
  float humidity = sensor_DHT.readHumidity();
  sensor_DS18B20.requestTemperatures();
  float temperature = sensor_DS18B20.getTempCByIndex(0);
  int soilMoistureValue = getSoilMoisture();
  if (isnan(humidity)) {
    Serial.println("failed to read dht sensor");
  }

  // DHT22 read result
  Serial.println("Hm=" + String(humidity) + "%");
  LCD.setCursor(0, 0);
  LCD.print(!isnan(humidity) ? "Hm=" + String(humidity, 1) + "%" : "Hm=Err");
  Blynk.virtualWrite(V4, humidity); // virtual pin for humidity
  
  // DS18B20 read result
  Serial.println("T=" + String(temperature) + "°C");
  LCD.setCursor(9, 0);
  LCD.print("T=" + String(temperature, 1) + "C");
  Blynk.virtualWrite(V3, temperature); // virtual pin for temperature
  
  // soil capacitative read result
  Serial.println("Ms=" + String(soilMoistureValue) + "%");
  LCD.setCursor(0, 1);
  LCD.print("Ms=" + String(soilMoistureValue) + "%");  
  Blynk.virtualWrite(V5, soilMoistureValue); // virtual pin for soil moisture  
  
  // pump indicator
  LCD.setCursor(8, 1);
  LCD.print("Pump=off");
  LCD.setCursor(13, 1);
  LCD.print(digitalRead(PUMP_PIN) ? "off" : "on ");

  // automatic mode logic 
  if (autoMode && !manualPumpMode) {
    digitalWrite(PUMP_PIN, soilMoistureValue < DRY_VALUE ? LOW : HIGH);
    Blynk.virtualWrite(V2, digitalRead(PUMP_PIN) ? 0 : 1);
  } else if (!autoMode && !manualPumpMode) {
    digitalWrite(PUMP_PIN, HIGH);
    Blynk.virtualWrite(V2, digitalRead(PUMP_PIN) ? 0 : 1);
  }
  delay(3000);
}

// functions
void connectNetwork() {
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("connecting to network...");
    delay(3000);
  }
  Serial.println("connected to network");
}

int getSoilMoisture() {
  int soilMoisture = analogRead(ANALOG_PIN);
  Serial.println(soilMoisture);
  soilMoisture = map(soilMoisture, MIN_VALUE, MAX_VALUE, 0, 100);
  if (soilMoisture >= 100) {
    soilMoisture = 100;
  } else if (soilMoisture <= 0) {
    soilMoisture = 0;
  }
  return soilMoisture;
}

// virtual pin from blynk
BLYNK_WRITE(V0) {
  autoMode = param.asInt();
  if (!manualPumpMode) {
    Serial.println("auto mode = " + String(autoMode ? "true" : "false"));
  }
}

BLYNK_WRITE(V1) { 
  digitalWrite(LAMP_PIN, !param.asInt() ? HIGH : LOW);
}

BLYNK_WRITE(V2) {
  manualPumpMode = param.asInt();
  if (!autoMode) {
    digitalWrite(PUMP_PIN, manualPumpMode ? LOW : HIGH);
    Serial.println(manualPumpMode ? "water pump is on" : "water pump is off");
  }
}