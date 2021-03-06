#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

RF24 radio(7, 8);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t window_door_one = 01;
//D2 is pin #5 I think
const uint16_t alarm_pin = 2;


void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node); //(channel, node address)
  pinMode(alarm_pin,OUTPUT);
}

void loop() {
  
  network.update();
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    
    //incoming data from any door/window sensors
    unsigned long incomingData;
    network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
    if(header.from_node == 01) {
      //this node is a door/window sensor
      if(incomingData == HIGH) {
        sendTriggerMessage(1);
      }
      else {
        sendTriggerMessage(0);
      }
    }
  }
}

void turnAlarmOn() {
  digitalWrite(alarm_pin,HIGH);
  
}
void turnAlarmOff() {
  digitalWrite(alarm_pin,LOW);
}

void sendMalfunctionMessage(int status) {
  
}

void sendTriggerMessage(int status) {
//Talk to blynk app somehow
  
}
