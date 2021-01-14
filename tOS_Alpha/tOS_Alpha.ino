#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include "Settings.h"

const char* ssid = STASSID;
const char* password = STAPSK;
const char* mqtt_server = MQQTIP;

const int sensorCount = sizeof(dallasSensorList)/sizeof(DeviceAddress*);
const int dhtCount = sizeof(dhtSensorList)/sizeof(DHT);
float tempArray[sensorCount][bufferSize];
float humTempArray[dhtCount*2][bufferSize];
int bufferIndex = 0;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  connectWifi();
  handleOTA();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  Serial.println("dinit");
  dallasInit();
  Serial.println("done");
  Serial.println("dhtinit");
  dhtInit();
  Serial.println("done");
}

void loop() {
  ArduinoOTA.handle();
  sensors.requestTemperatures();
  mqttHandle();
}

void connectWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Booting");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}

void dallasInit(){
  sensors.begin();
  bufferIndex = 0;
  delay(1000);
  for(int i = 0; i < sensorCount; i++){
    sensors.setResolution(dallasSensorList[i], TEMPERATURE_PRECISION);
    delay(sensorDelay);
  }
  while(bufferIndex < bufferSize){
    for(int i = 0; i < sensorCount; i++){
      float t = sensors.getTempC(dallasSensorList[i]);
      delay(sensorDelay);
      while(t < -20){
        t = sensors.getTempC(dallasSensorList[i]);
        delay(sensorDelay);
      }
      tempArray[i][bufferIndex] = t;
    }
    bufferIndex++;
  }
}

void dhtInit(){
  bufferIndex = 0;
  for(int i = 0; i < dhtCount; i++){
    dhtSensorList[i].begin();
    delay(sensorDelay);
  }
  while(bufferIndex < bufferSize){
    int k = 0;
    for(int i = 0; i < dhtCount; i++){
      float t = dhtSensorList[i].readTemperature();
      float h = dhtSensorList[i].readHumidity();
      while(t < -20){
        t = dhtSensorList[i].readTemperature();
      }
      while(h < -20){
        h = dhtSensorList[i].readHumidity();
      }
      humTempArray[k][bufferIndex] = t;
      humTempArray[k+1][bufferIndex] = h;
      k = k + 2;
    }
    bufferIndex++;
  }
}

void tempUpdate(){
  Serial.println("ds");
  if(bufferIndex >= bufferSize) bufferIndex = 0;
  for(int i = 0; i < sensorCount; i++){
    float t = sensors.getTempC(dallasSensorList[i]);
    delay(sensorDelay);
    while(t < -20){
      t = sensors.getTempC(dallasSensorList[i]);
      delay(sensorDelay);
    }
    tempArray[i][bufferIndex] = t;
  }
  Serial.println("done");
  Serial.println("dht");
  int k = 0;
  for(int i = 0; i < dhtCount; i++){
      float t = dhtSensorList[i].readTemperature();
      float h = dhtSensorList[i].readHumidity();
      while(t < -20){
        t = dhtSensorList[i].readTemperature();
        delay(sensorDelay);
      }
      while(h < -20){
        h = dhtSensorList[i].readHumidity();
        delay(sensorDelay);
      }
      humTempArray[k][bufferIndex] = t;
      humTempArray[k+1][bufferIndex] = h;
      k = k + 2;
    }
    Serial.println("done");
  bufferIndex++;
}
  

void mqttHandle(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  if (now - lastMsg > updateDelay) {
    debug(String(dhtSensorList[0].readTemperature()).c_str());
    debug(String(dhtSensorList[1].readTemperature()).c_str());
    debug(String(dhtSensorList[2].readTemperature()).c_str());
    lastMsg = now;
    tempUpdate();
    for(int i = 0; i < sensorCount; i++){
      float t = 0;
      for(int j = 0; j < bufferSize; j++){
        t += tempArray[i][j];
      }
      t /= bufferSize;
      snprintf (msg, MSG_BUFFER_SIZE, String(t).c_str());
      client.publish(dallasTopicList[i], msg);
    }
    int k = 0;
    for(int i = 0; i < dhtCount; i++){
      float t = 0;
      float h = 0;
      for(int j = 0; j < bufferSize; j++){
        t += humTempArray[k][j];
        h += humTempArray[k+1][j];
      }
      t /= bufferSize;
      h /= bufferSize;
      snprintf (msg, MSG_BUFFER_SIZE, String(t).c_str());
      client.publish(dhtTopicList[k], msg);
      snprintf (msg, MSG_BUFFER_SIZE, String(h).c_str());
      client.publish(dhtTopicList[k+1], msg);
      k = k + 2;
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = cID;
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mUser, mPass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void handleOTA(){
  ArduinoOTA.setHostname(cID);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void debug(const char* s){
  snprintf (msg, MSG_BUFFER_SIZE, s);
  client.publish("debug" , msg);
}
