#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

RF24 radio(9, 10);               // nRF24L01 (CE,CSN)
//RF24 radio(7,8); //
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 01;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t mcu = 00;    

//D2 is pin #5 I think
const uint16_t reed_pin = 4;  
bool delayRunning = false;
int delayStart;

void setup() {
  //for debugging
  Serial.begin(9600);

   
  pinMode(reed_pin,INPUT_PULLUP);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  delayStart = millis();   // start delay
  delayRunning = true; // not finished yet
}
void loop() {
  
  //idk how costly this instruction is, but we may not need it(this arduino does not use anything from the network  
  network.update();

  
  //if delay is false, then we just sent our status so reinit everything
  if(delayRunning == false) {
    delayStart = millis();
    delayRunning = true;
  }

  //if 1 minute(60000 ms) has passed...  use 6000(6 seconds) for debugging
  if(delayRunning && ((millis() - delayStart) >= 1000)) {
    //if this happened then timer has ended

    //get the reed pin's state
    unsigned long sensor_status = digitalRead(reed_pin);
    
    //Debugging: log the sensor status here
    Serial.print(sensor_status);
  
    //then after the timer is finished, call send_status
    send_status(sensor_status);

    //reinit the timer using this
    delayRunning = false;
  }
}

void send_status(unsigned long sensor_status) {
  
  RF24NetworkHeader header(mcu);     // (Address where the data is going)
  bool ok = network.write(header, &sensor_status, sizeof(sensor_status)); // Send the data
}
