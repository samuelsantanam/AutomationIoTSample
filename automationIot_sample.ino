#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "DHT.h"
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#define DHTPIN D5                                       //pin D5 on NodeMCU
#define DHTTYPE DHT22                                   //Type sensor

const char* ssid = "";                                  //your wifi ssid
const char* password = "";                              //your pw for wifi
const char* passwdota = "";                             //Password for use ArduinoOTA: On The Air

DHT dht(DHTPIN, DHTTYPE);
ESP8266WiFiMulti wifiMulti;
WiFiClient wifiClient;

//MQTT Server
const char* BROKER_MQTT = "";                            //URL of broker MQTT
int BROKER_PORT = ;                                      //Broker port
#define mqtt_user ""                                     //Mqtt user 
#define mqtt_password ""                                 //Mqtt password
#define ID_MQTT "NodeMCU"                                //Name of device send to MQTT


//Topic publish
#define TOPIC_PUBLISH_TEMP "home/bedroom/temperature"
#define TOPIC_PUBLISH_HUM  "home/bedroom/humidity"
PubSubClient MQTT(wifiClient);                           //Instance mqtt client, send wifi client

//Call Functions
void keepConnect();                                     //Keep connect in MQTT Broker
void connectWiFi();                                     //Wifi connect
void ConnectMQTT();                                     //Broker connect

void setup() {
  pinMode(DHTPIN, INPUT_PULLUP);                        //Temp sensor DHT22
  pinMode(LED_BUILTIN, OUTPUT);                         //NodeMCU Led
  Serial.begin(115200);
  connectWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);

  //OTA config
  Serial.println("Booting");
  wifiMulti.addAP(ssid, password);
  Serial.println("Connecting OTA on wifi...");

  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(250);
    Serial.print('.');
  }

  ArduinoOTA.setHostname("NODEMCU_8266");
  ArduinoOTA.setPassword(passwdota);
  ArduinoOTA.onStart([]() {
    startOTA();
  });
  ArduinoOTA.onEnd([]() {                   
    endOTA();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    progressOTA(progress, total);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    errorOTA(error);
  });

  ArduinoOTA.begin();

  Serial.println("OTA Ready");
  Serial.print("IP address OTA: ");
  Serial.println(WiFi.localIP());
}

void startOTA() {
  String type;

  if (ArduinoOTA.getCommand() == U_FLASH)
    type = "flash";
  else
    type = "filesystem"; // U_SPIFFS
  Serial.println("Start updating " + type);
}

void endOTA()
{
  Serial.println("\nEnd");
}

void progressOTA(unsigned int progress, unsigned int total)
{
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void errorOTA(ota_error_t error){
  Serial.printf("Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR)
    Serial.println("Auth Failed");
  else if (error == OTA_BEGIN_ERROR)
    Serial.println("Begin Failed");
  else if (error == OTA_CONNECT_ERROR)
    Serial.println("Connect Failed");
  else if (error == OTA_RECEIVE_ERROR)
    Serial.println("Receive Failed");
  else if (error == OTA_END_ERROR)
    Serial.println("End Failed");
}

void loop() {
  keepConnect();
  sendTemp();
  ArduinoOTA.handle();                                                                
  MQTT.loop();
}

void keepConnect() {
  if (!MQTT.connected()) {
    ConnectMQTT();
  }
  connectWiFi();
}

//*********************************************** wifi connect ****************************************************//
void connectWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.print("Connect to: ");
  Serial.print(ssid);
  Serial.println("...Wait!");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connect sucess, on: ");
  Serial.print(ssid);
  Serial.print("  IP: ");
  Serial.println(WiFi.localIP());
}

//*********************************************** MQTT connect ****************************************************//
void ConnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Attempting MQTT connection...");
    Serial.print("Connecting to ");
    Serial.print(BROKER_MQTT);
    Serial.print(" as ");
    Serial.println(ID_MQTT);
                                                                                //if you not have a password use: if (client.connect((char*) clientName.c_str())) {
    if (MQTT.connect(ID_MQTT, mqtt_user, mqtt_password)) {                      // If you do not want to use a username and password, change next line to
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTT.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//************************************************* Temp sensor *************************************************//
void sendTemp() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t, 0);
  Serial.println("°C");
  MQTT.publish(TOPIC_PUBLISH_TEMP, String(t).c_str(), true);

  Serial.print("Humidity: ");
  Serial.print(h, 0);
  Serial.println("%");
  MQTT.publish(TOPIC_PUBLISH_HUM, String(h).c_str(), true);
  delay(900000);    //Delay: 15 min
}
