//
// ce347-2 microprocessor systems project 2
// security system: door/window sensors
//
// hardware: microcontroller w/ SPI (Arduino Uno, Arduino
// Nano IOT 33, Elegoo Uno R3), nRF24L01 transceiver, reed switch
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
// Read the status of the reed_pin sensor and send a signal 
// through the window_door node (aka nRF24L01 transceiver) to
// the main control unit indicating whether the reed_pin is 
// open (HIGH) or closed (LOW).


#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>


RF24 radio(9, 10);                // nRF24L01 (CE,CSN)
RF24Network network(radio);       // Include the radio in the network

const uint16_t this_node = 01;    // Address of this node in Octal format ( 04,031, etc)
const uint16_t mcu = 00;          // Address of main control unit (mcu)

int reed_pin = 4;
int delayStart;
bool delayRunning = false;


void setup() {
  // Debugging
//  Serial.begin(9600);
  
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);   // (channel, node address)
  
  delayStart = millis();          // start delay timer
  delayRunning = true;            // status has not yet been sent
     
  pinMode(reed_pin,INPUT_PULLUP);
}


void loop() {
  
  // May not need this line because the arduino does not use anything from the network  
  network.update();
 
  // If delay is false, then we have just sent our status and should reinit everything
  if(delayRunning == false) {
    delayStart = millis();
    delayRunning = true;
  }

  // Send sensor status every 1 minute (60000 ms)
  // Debugging: use 1000 (1 seconds)
  if(delayRunning && ((millis() - delayStart) >= 1000)) {
    // If this happened then timer has ended

    // Get the reed pin's state
    unsigned long sensor_status = digitalRead(reed_pin);
    
    // Debugging: log the sensor status
//    Serial.print(sensor_status);
  
    // After the timer is finished, call send_status
    send_status(sensor_status);

    // Reinit the timer
    delayRunning = false;
  }
}


// Send status through the nRF transciever to the main control unit
void send_status(unsigned long sensor_status) {
  RF24NetworkHeader header(mcu);     // (Address where the data is going)
  bool ok = network.write(header, &sensor_status, sizeof(sensor_status)); // Send the data
}
