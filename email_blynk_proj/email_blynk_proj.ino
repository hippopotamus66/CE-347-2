//
// ce347-2 microprocessor systems project 2
// security system: blynk interface
//
// hardware: Arduino Nano IOT 33 (Wifi connectivity)
//
// Update the Blynk app in real-time on doors and windows closing 
// and opening, remotely set off an alarm sound through the Blynk
// app, and send emails to a recipient when a door or window is 
// opened while the system is armed. Requires Wifi connection in 
// order for Blynk and Arduino Nano IOT 33 to communicate.


#include "Arduino.h"
#include <SPI.h>
#include <Blynk.h>
#include <WiFiNINA.h>
#include <EMailSender.h>
#include <BlynkSimpleWiFiNINA.h>


// REPLACE WITH YOUR NETWORK CREDENTIALS
char ssid[] = "";       // name of wifi connection
char password[] = "";   // wifi password

// REPLACE WITH AUTH CODE GENERATED FROM YOUR BLYNK APP
char auth[] = "";       // ex: KCRUeBA60hgY-Hkhfy52R2A3zgAIwO2L


//
// IMPORTANT:
// To send Email using Gmail use port 465 (SSL) and SMTP Server smtp.gmail.com
// YOU MUST ENABLE less secure app option https://myaccount.google.com/lesssecureapps?pli=1
// Also update your firmware certificate to include google.com
//
// REPLACE WITH YOUR EMAIL ADDRESSES AND PASSWORD
#define emailSenderAccount    ""      // securityteam@gmail.com
#define emailSenderPassword   ""      // securityiscool2021
#define emailRecipient        ""      // TacoBell@gmail.com
#define emailSubject          "URGENT: Door/window sensor opened"
EMailSender emailSend(emailSenderAccount, emailSenderPassword);


// Buzzer notes (customizable)
#define NOTE_C5  523
#define NOTE_FS5 740


// Alarm pins (Blynk uses the same virtual pin numbers)
int alarm_pin_1 = 5;      // node 01 (window_door_one)
int alarm_pin_2 = 4;      // node 02 (window_door_two)
int BUZZER = 21;
int buzzer_led = 20;      // Debugging: toggle buzzer
int buzzerPin;            // Blynk virtual pin1 (V1)
int armPin;               // Blynk virtual pin0 (V0)
int nodeStatus;

int pin1_prevState;
int pin2_prevState;
int pin1_currState;
int pin2_currState;
int previousEmail;
bool emailSent = false;


void setup() {
  // Debugging
//  Serial.begin(9600);

  Blynk.begin(auth, ssid, password); 

  int pin1_prevState = 1;
  int pin2_prevState = 1;
  int pin1_currState = 1;
  int pin2_currState = 1;

  pinMode(4,INPUT_PULLDOWN);
  pinMode(5,INPUT_PULLDOWN);
  pinMode(BUZZER, OUTPUT);
  pinMode(buzzer_led, OUTPUT);
}


void loop() {
  Blynk.run();

  // Update sensor status in Blynk
  UpdateState(alarm_pin_1);
  UpdateState(alarm_pin_2);

  // Update sensor status in local variables
  pin1_prevState = pin1_currState;
  pin2_prevState = pin2_currState;
  pin1_currState = DisplayState(alarm_pin_1);
  pin2_currState = DisplayState(alarm_pin_2);
  
  // Debugging: alarm pin state
  // Note: DisplayState(alarm_pin_1) == HIGH -> OPEN
//  Serial.println(pin1_prevState);
//  Serial.println(pin1_currState);
//  Serial.println(DisplayState(alarm_pin_1));

  // Restart timer from when email was sent
  if (emailSent == true) {
    previousEmail = millis();
    emailSent = false;
  }

  // Check if system is armed
  if (armPin == HIGH) {
      
    // Send emails no quicker than every 10 seconds (10000 ms)
    if (!emailSent && ((millis() - previousEmail) >= 10000)) {

      // Send email only when sensor that was closed is now open
      if ((pin1_currState == HIGH && pin1_prevState == LOW) || (pin2_currState == HIGH && pin2_prevState == LOW)) {

        // Send email to recipient
        SendEmail();
        emailSent = true;
      }
    }
  }

  // Play alarm sound if button is pressed in Blynk app
  if (buzzerPin == HIGH) {
    SoundBuzzer();
  } else {
    noTone(BUZZER);
  }

}


// Returns HIGH for OPEN sensor, LOW for CLOSED sensor
int DisplayState(int alarm_pin) {
  nodeStatus = digitalRead(alarm_pin);
  return nodeStatus;
}

// Updates sensor status in Blynk
void UpdateState(int alarm_pin) {
  DisplayState(alarm_pin);
  if (nodeStatus == LOW) {
    Blynk.virtualWrite(alarm_pin, "CLOSED");
  } else if (nodeStatus == HIGH) {
    Blynk.virtualWrite(alarm_pin, "OPEN");
  }
}

// Plays alarm buzzer
void SoundBuzzer() {
  tone(BUZZER,NOTE_FS5);
  digitalWrite(buzzer_led, HIGH);
  delay(500);
  tone(BUZZER,NOTE_C5);
  digitalWrite(buzzer_led, LOW);
  delay(400);
}

// Send email to recipient
void SendEmail() {
  // Set up email message
  EMailSender::EMailMessage message;
  message.subject = emailSubject;
  message.message = "A door or window was opened while your security system was armed. Stay safe!<br><br>Sincerely,<br>Your Security Team";

  // Set up recipient 
  EMailSender::Response resp = emailSend.send(emailRecipient, message);
  
  // Debugging: print email status
//  Serial.println("Sending status: ");
//  Serial.println(resp.status);
//  Serial.println(resp.code);
//  Serial.println(resp.desc);
}


// Read from Blynk virtual pins
BLYNK_WRITE(V1) {buzzerPin = param.asInt();}
BLYNK_WRITE(V0) {armPin = param.asInt();}
