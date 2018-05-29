#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include <MAX44009.h>
#define PUBLISH_DELAY 15
#define MEDICION_DELAY 150

// Update these with values suitable for your network.
byte mac[]    = {  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCF };
IPAddress server(192, 168, 1, 100);

int pirPin = 4;
int ledPin = 13;
int calibrationTime = 30;

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 8000, callback, ethClient);

long lastReconnectAttempt = 0;

ClosedCube_HDC1080 hdc1080;
MAX44009 light;

char tempbuffer[5];
char humidbuffer[5];
char lumibuffer[5];
char movbuffer[5];

double Temperatura = 0;
double Humedad = 0;
double Luminosidad = 0;
int Movimiento = 0;

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.

  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  client.publish("outTopic", p, length);
  // Free the memory
  free(p);
}

boolean reconnect() {
  if (client.connect("YinnSense")) {
    // Once connected, publish an announcement...
//    client.publish("outTopic","hello world");
    // ... and resubscribe
//    client.subscribe("inTopic");
  }
  return client.connected();
}

void setup()
{
  Serial.begin(9600);
  
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);  
  
  Ethernet.begin(mac);
  hdc1080.begin(0x40);
  light.begin();

  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device

  delay(1500);
  lastReconnectAttempt = 0;
  
//  if (client.connect("YinnSense")) {
    //client.publish("outTopic","hello world");
    //itoa(Temperatura, bytebuffer, 10);
    
    //client.publish("sensores_internos/temperatura", bytebuffer);

    //client.subscribe("inTopic");
//  }

  
  

  
}

void loop()
{
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected

    getTempHum(HDC1080_RESOLUTION_8BIT, HDC1080_RESOLUTION_8BIT);
    delay(MEDICION_DELAY);
    getLuminosidad();
    delay(MEDICION_DELAY);
    getMovimiento();
    delay(MEDICION_DELAY);
    publishAll();
    delay(MEDICION_DELAY);
    
    client.loop();
  }
}

void publishAll(){
  client.publish("sensores_internos/temperatura", tempbuffer);
  delay(PUBLISH_DELAY);
  client.publish("sensores_internos/humedad", humidbuffer);
  delay(PUBLISH_DELAY);
  client.publish("sensores_internos/luminosidad", lumibuffer);
  delay(PUBLISH_DELAY);
  client.publish("sensores_internos/movimiento", movbuffer);
  delay(PUBLISH_DELAY);
}

void getTempHum(HDC1080_MeasurementResolution humidity, HDC1080_MeasurementResolution temperature) {
  hdc1080.setResolution(humidity, temperature);
  HDC1080_Registers reg = hdc1080.readRegister();
  //printRegister(reg);
  
  Temperatura = hdc1080.readTemperature();
  int intTemperatura = (int) Temperatura;
  snprintf(tempbuffer, 5, "%d", intTemperatura);
  Humedad = hdc1080.readHumidity();
  int intHumedad = (int) Humedad;
  snprintf(humidbuffer, 5, "%d", intHumedad);

  Serial.print("Temperatura: ");
  Serial.print(tempbuffer);
  Serial.println(" °C"); 
  
  Serial.print("Humedad: ");
  Serial.print(humidbuffer);
  Serial.println(" %"); 
}

void getLuminosidad() {
  int Luminosidad = (int) light.get_lux();
  int intLuminosidad = (int) Luminosidad;
  snprintf(lumibuffer, 5, "%d", intLuminosidad);
  Serial.print("Flujo luminoso: ");
  Serial.print(lumibuffer);
  Serial.println(" lux");
}

void getMovimiento(){

     Movimiento = digitalRead(pirPin);
//     int mov = 0;

             

//     if(Movimiento == 0){
//      mov = 1;
//     }
//  
//     if(Movimiento == 1){ 
//      mov = 0;   
//     }

     Serial.println("Movimiento: ");
     Serial.println(Movimiento);

     snprintf(movbuffer, 5, "%d", Movimiento);
   
}
