#include <SPI.h>
#include <Ethernet.h>
#include <MQTT.h>

#define PIN_CONNECT 4

EthernetClient net;
MQTTClient client;

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xFE };
unsigned long lastMillis = 0;
IPAddress ip(192, 168, 8, 124);

void connect() {
//  Serial.print("conectando red...");
  Ethernet.begin(mac,ip);

//  Serial.print("\nconectando mqtt...");
  while (!client.connect("YinnLight", "intentando", "intentando")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nmqtt conectado!");

  client.subscribe("actuadores/bombilla");
  client.publish("dispositivos_activos/YinnLight", "true");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("mensaje: " + topic + " - " + payload);
  if(payload == "1"){
    digitalWrite(PIN_CONNECT, HIGH);
  }
  else{
    digitalWrite(PIN_CONNECT, LOW);
  }
}

void setup() {
  Serial.begin(9600);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin("192.168.8.150", 8000, net);
  client.onMessage(messageReceived);

  pinMode(PIN_CONNECT, OUTPUT);
  digitalWrite(PIN_CONNECT, LOW);

  connect();
}

void loop() {
  
  client.loop();

  if (!client.connected()) {
    connect();
  }
  
//  if (millis() - lastMillis > 1000) {
//    lastMillis = millis(); 
//    client.publish("actuadores/ventilador", "1");
//  }


}
