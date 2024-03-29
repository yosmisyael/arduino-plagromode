#define BLYNK_TEMPLATE_ID     ""
#define BLYNK_TEMPLATE_NAME   ""
#define BLYNK_AUTH_TOKEN      ""
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// set the default mode to auto
bool isAuto = true;
const char* SSID = "";
const char* PASS = "";
const u_int8_t PUMP_PIN = 16;
const u_int8_t BUILTIN_LED = 2;
hw_timer_t *machineTimer = NULL;
hw_timer_t *relayTimer = NULL;

void getNetworkConnection(const char* ssid, const char* pass) {
  digitalWrite(BUILTIN_LED, LOW);
  WiFi.begin(ssid, pass);
  Serial.print("connecting to network " + String(ssid));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(3000);
  }
  Serial.println("connected to network" + String(WiFi.localIP()));
  digitalWrite(BUILTIN_LED, HIGH);
}

void IRAM_ATTR handlerMachine() {
  digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  timerStart(relayTimer);
  timerStop(machineTimer);
}

void IRAM_ATTR handleRelay() {
  digitalWrite(BUILTIN_LED, LOW);
  timerStop(relayTimer);
  timerStart(machineTimer);
}

BLYNK_WRITE(V0) {
  int data = param.asInt();
  if (digitalRead(BUILTIN_LED) == 1) {
    digitalWrite(BUILTIN_LED, 0);
  }
  Blynk.virtualWrite(V1, 0);
  if (data == 1) {
    isAuto = true;
    timerStart(machineTimer);
  } else {
    isAuto = false;
    timerStop(machineTimer);
    timerStop(relayTimer);
    if (digitalRead(BUILTIN_LED) == 1) {
      digitalWrite(BUILTIN_LED, 0);
    }
  }
}

BLYNK_WRITE(V1) {
  int data = param.asInt();
  if (!isAuto) {
    digitalWrite(BUILTIN_LED, data ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(115200);
  getNetworkConnection(SSID, PASS);
  Blynk.begin(BLYNK_AUTH_TOKEN, SSID, PASS);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  Blynk.virtualWrite(V0, 1);
  Blynk.virtualWrite(V1, 0);

  // machine timer used to control the device auto mode active period 
  machineTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(machineTimer, &handlerMachine, true);
  timerAlarmWrite(machineTimer, 1000000*3, true);
  timerAlarmEnable(machineTimer);
  
  // relay timer used to control the relay active duration
  relayTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(relayTimer, &handleRelay, true);
  timerAlarmWrite(relayTimer, 1000000*5, true);
  timerAlarmEnable(relayTimer);
}

void loop(){
  if(WiFi.status() != WL_CONNECTED){
    getNetworkConnection(SSID, PASS);
  }
  isAuto ? Blynk.virtualWrite(0, 1) : Blynk.virtualWrite(0, 0);
  Blynk.run();
}
