#define BLYNK_TEMPLATE_ID     "TMPL6u-CbO2a6"
#define BLYNK_TEMPLATE_NAME   "SMEKSAPLOS AUTOMATIC WATERING"
#define BLYNK_AUTH_TOKEN      "W6xYp4CGgOaI-I829DpHKzLw-AWHg2tx"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

bool autoMode = false;
bool manualPumpMode = false;
const char* SSID = "PS1";
const char* PASS = "@ps1labkom";
const u_int8_t PUMP_PIN = 16;
const u_int8_t WET_VALUE = 74;
const u_int8_t DRY_VALUE = 47;
const u_int8_t BUILTIN_LED = 2;
hw_timer_t *timer = NULL;

void getNetworkConnection() {
  WiFi.begin(SSID, PASS);
  Serial.print("connecting to network");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(3000);
  }
  Serial.println("connected to network");
}

void IRAM_ATTR onAlarm(){
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
}

BLYNK_WRITE(V0) {
  int data = param.asInt();
  if (manualPumpMode) manualPumpMode = false;
  autoMode = data ? true : false;
  autoMode == true ? timerStart(timer) : timerStop(timer);
  digitalWrite(BUILTIN_LED, LOW);
}

BLYNK_WRITE(V1) {
  int data = param.asInt();
  manualPumpMode = data ? true : false;
  if (!autoMode) {
    digitalWrite(BUILTIN_LED, manualPumpMode ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, SSID, PASS);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  getNetworkConnection();
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onAlarm, true);
  timerAlarmWrite(timer, 3600*1000000*2, true);
  timerAlarmEnable(timer);
}

void loop(){
  if(WiFi.status() != WL_CONNECTED){
    getNetworkConnection();
  }
  Blynk.run();
}