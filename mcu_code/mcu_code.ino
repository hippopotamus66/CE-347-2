//
// ce347-2 microprocessor systems project 2
// security system: main control unit
//
// hardware: Elegoo UNO R3, nRF24L01 transceiver
//
// +---------------------------+
// | 1 2                       |
// | 3 4     component side    |
// | 5 6        nRF24L01       |
// | 7 8                       |
// +---------------------------+ 
// 
// 1 - GND (boxed)
// 2 - VCC (3.3V)
// 3 - CE
// 4 - CSN
// 5 - SCK
// 6 - MOSI
// 7 - MISO
// 8 - IRQ
//
// Read the incoming data from the window_door nodes and 
// send a signal to the alarm_pins indicating whether 
// each reed_pin sensor is open (HIGH) or closed (LOW).


#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

             
RF24 radio(9, 10);                    // nRF24L01 (CE, CSN)
RF24Network network(radio);           // Include the radio in the network

const uint16_t this_node = 00;        // Address of this node in Octal format ( 04,031, etc)
const uint16_t window_door_one = 01;  // Address of node 01 (window_door_one)
const uint16_t window_door_two = 02;  // Address of node 02 (window_door_two)

int alarm_pin_1 = 5;
int alarm_pin_2 = 4;


void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);       // (channel, node address)
  
  pinMode(alarm_pin_1, OUTPUT);
  pinMode(alarm_pin_2, OUTPUT);
}

void loop() {
  
  network.update();
  while (network.available()) {       // Is there any incoming data?
    RF24NetworkHeader header;
    
    // Read incoming data from any door/window nodes
    unsigned long incomingData;
    network.read(header, &incomingData, sizeof(incomingData)); // Read incoming data

    // Toggle incoming door/window node's alarm_pin
    if(header.from_node == window_door_one) {
      if(incomingData == HIGH) {
        turnAlarmOn(alarm_pin_1);
      } else {
        turnAlarmOff(alarm_pin_1);
      }
    }
    if(header.from_node == window_door_two) {
      if(incomingData == HIGH) {
        turnAlarmOn(alarm_pin_2);
      } else {
        turnAlarmOff(alarm_pin_2);
      }
    }
  }
}


// Toggle the alarm pins
void turnAlarmOn(int alarm_pin) {
    digitalWrite(alarm_pin,HIGH); 
}
void turnAlarmOff(int alarm_pin) {
    digitalWrite(alarm_pin,LOW);
}
