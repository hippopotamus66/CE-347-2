#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

//#include <WiFi.h>



// To send Email using Gmail use port 465 (SSL) and SMTP Server smtp.gmail.com
// YOU MUST ENABLE less secure app option https://myaccount.google.com/lesssecureapps?pli=1
//#define emailSenderAccount    ""    
//#define emailSenderPassword   ""
//#define emailRecipient        ""
//#define smtpServer            "smtp.gmail.com"
//#define smtpServerPort         465 //587 //465
//#define emailSubject          "door_window_open"
//SMTPData smtpData;



//RF24 radio(7, 8);               // nRF24L01 (CE,CSN)
RF24 radio(9, 10);
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t window_door_one = 01;
const uint16_t window_door_two = 02;
//D2 is pin #5 I think
const uint16_t alarm_pin_1 = 5;
const uint16_t alarm_pin_2 = 4;


void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node); //(channel, node address)
  pinMode(alarm_pin_1, OUTPUT);
  pinMode(alarm_pin_2, OUTPUT);
}

void loop() {
  
  network.update();
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    
    //incoming data from any door/window sensors
    unsigned long incomingData;
    network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
    if(header.from_node == window_door_one) {
      //this node is a door/window sensor
      if(incomingData == HIGH) {
        turnAlarmOn1();
      }
      else {
        turnAlarmOff1();
      }
    }
    if(header.from_node == window_door_two) {
      //this node is a door/window sensor
      if(incomingData == HIGH) {
        turnAlarmOn2();
      }
      else {
        turnAlarmOff2();
      }
    }
  }
}

////////////////////////////////////////////////////
// Alarm 1
////////////////////////////////////////////////////
void turnAlarmOn1() {
    digitalWrite(alarm_pin_1,HIGH); 
}
void turnAlarmOff1() {
    digitalWrite(alarm_pin_1,LOW);
}

////////////////////////////////////////////////////
// Alarm 2
////////////////////////////////////////////////////
void turnAlarmOn2() {
    digitalWrite(alarm_pin_2,HIGH); 
}
void turnAlarmOff2() {
    digitalWrite(alarm_pin_2,LOW);
}

void sendMalfunctionMessage(int status) {
  
}
