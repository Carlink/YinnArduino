void setup(){
 Serial.begin(9600);
 pinMode(8, INPUT);//pin DO en el sensor
 //CONECTAR TAMBIEN A0 EN EL SENSOR AL A0 EN EL ARDUINO 
}
void loop(){

Serial.println(analogRead(0));
// NO LLUEVE (SECO ) ES 1023 MOJADO ES MENOR
// //digital output
 if(digitalRead(8) == HIGH) Serial.println("No Rain Detected");
 else Serial.println("Rain Detected");

 delay(250);
}
