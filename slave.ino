/*******************************************************************************************
 **                                                                                       **
 ** Author: Jonas Burkhalter                                                              **
 ** Date: 30. March 2021                                                                  **
 ** Github: https://github.com/jonas-burkhalter/jsa.timemachine                           **
 ** Version: 0.1                                                                          **
 **                                                                                       **
 ** Libraries:                                                                            **
 ** CheapStepper: https://github.com/tyhenry/CheapStepper                                 **
 ** PubSubClient: https://github.com/knolleary/pubsubclient/                              **
 ******************************************************************************************/

///////////////////
// CONFIGURATION //
///////////////////
const int POWER = 1;
const char* TOPIC_NUMBER = "timemachine/number";

// Wifi //
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
const char* SSID = "timemachine";
const char* PASSWORD = "altisberg";
WiFiClient wiFiClient;

// MQTT //
#include <PubSubClient.h>
PubSubClient client(wiFiClient);
const char* MASTER = "192.168.4.1";
const char* CLIENT = "timemachine-slave-1";

// Sensor //
const int SENSOR_PIN = D0;

// Stepper //
#include <CheapStepper.h>
CheapStepper stepper = CheapStepper(D1,D2,D3,D4);
int offset = 3600;

///////////
// Setup //
///////////
void setup() {
  Serial.begin(115200);
  Serial.println("POWER: " + String(POWER));
  Serial.println("TOPIC_NUMBER: " + String(TOPIC_NUMBER));

  // Wifi //
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // MQTT //
  client.setServer(MASTER, 1883);
  client.setCallback(callback);

  // Sensor //
  pinMode(SENSOR_PIN, INPUT);

  // Stepper //
  calibrated();
}

//////////
// Loop //
//////////
void loop() {
  stepper.run();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

//////////
// MQTT //
//////////
void callback(char* topic, byte* payload, unsigned int length) {
  char data[length+1];
  os_memcpy(data, payload, length);
  data[length] = '\0';

  Serial.println("MQTT - callback topic: " + String(topic) + " data: " + String(data));
  
  if (String(topic) == TOPIC_NUMBER) {
    rotate(String(data).toInt());
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("MQTT.reconnect");
    if (client.connect(CLIENT)) {
      Serial.println("MQTT.connected");
      client.subscribe(TOPIC_NUMBER);
    } else {
      delay(1000);
    }
  }
}

/////////////
// Stepper //
/////////////
bool calibrated() {
  if (offset > 360){
    calibrate();
  }
  
  return (offset < 360); 
}

void calibrate() {
  Serial.println("Stepper.calibrate");
  
  for(int i = 0; i < offset; i++){
    stepper.moveToDegree(true, i); delay(1);

    if (digitalRead(SENSOR_PIN) == LOW){
      offset = i % 360; 
      Serial.println("Stepper.offset: " + String(offset));
    }
  }
}

void rotate(int number) {
  if(calibrated() && (stepper.getStepsLeft() <= 0)){    
    int digit = (number / int(pow(10, POWER))) % 10;     
    int degree = ((36 * digit) + offset) % 360;   
    stepper.newMoveToDegree(true, degree); 

    Serial.println("Stepper.rotate - digit: " + String(digit) + " degree: " + String(degree));
  }
}
