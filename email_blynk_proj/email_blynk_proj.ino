//#include "esp_camera.h"
#include "Arduino.h"
//#include "FS.h"                // SD Card ESP32
//#include "SD_MMC.h"            // SD Card ESP32
//#include "soc/soc.h"           // Disable brownour problems
//#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
//#include "driver/rtc_io.h"
//#include <EEPROM.h>            // read and write from flash memory
//#include "SD.h"
#include <SPI.h>
//#include <WiFi.h>
#include <Blynk.h>
#include<WiFiNINA.h>
#include<BlynkSimpleWiFiNINA.h>
//#include "ESP32_MailClient.h"
//#include "SPIFFS.h"
//#include "esp_timer.h"
//#include "img_converters.h"
//#include <BlynkSimpleEsp32.h>


// REPLACE WITH YOUR NETWORK CREDENTIALS
char ssid[] = "ARRIS-5E23";   //"MOTOE632";
char password[] = "NULIBRARY637";   //"97s0vv1tkr";
char auth[] = "cPVTiodD_S00EWwUmFulqEH1fJo831oa"; //KCRUeBA60hgY-Hkhfy52R2A3zgAIwO2L


// To send Email using Gmail use port 465 (SSL) and SMTP Server smtp.gmail.com
// YOU MUST ENABLE less secure app option https://myaccount.google.com/lesssecureapps?pli=1
#define emailSenderAccount    ""    
#define emailSenderPassword   ""
#define emailRecipient        ""
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort         465 //587 //465
#define emailSubject          "ESP32_SndPicture"
//SMTPData smtpData;


// Alarm notes (customizable)
#define NOTE_C5  523
#define NOTE_FS5 740


// Alarm pins (Blynk uses the same virtual pin numbers)
// ex: node 01(window_door_one) -> alarm_pin_1(digital pin 5) -> Blynk virtual pin 5
int alarm_pin_1 = 5;
int alarm_pin_2 = 4;
int BUZZER = 21;
int buzzerPin;
int armPin;
int nodeStatus;


/*
bool alreadyRan = false;
int pictureNumber = 0;
int d5_val = 0;

void sendCallback(SendStatus msg) {
  // Print the current status
  Serial.println(msg.info());
  // Do something when complete
  if (msg.success()) {
    Serial.println("----------------");
  }
}
void sendEmail() {
  //camera_fb_t * fb = NULL;
  delay(3000);
  // Take Picture with Camera
  //fb = esp_camera_fb_get(); 
  
  // Set the SMTP Server Email host, port, account and password
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  // Set the sender name and Email
  smtpData.setSender("MCU", emailSenderAccount);
  // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
  smtpData.setPriority("High");
  // Set the subject
  smtpData.setSubject(emailSubject);
  // Set the message with HTML format
  smtpData.setMessage("<div style=\"color:#2f4468;\"><h1>One of the Window sensors was opened!</h1><p>- Sent from Arduino nano WIFI board</p></div>", true);
  // Set the email message in text format (raw)
  // Add recipients, you can add more than one recipient
  smtpData.addRecipient(emailRecipient);
  //smtpData.addRecipient("YOUR_OTHER_RECIPIENT_EMAIL_ADDRESS@EXAMPLE.com");
  //HERE!!! ONE BELOW MIGHT BE NEEDED
 
  
  smtpData.setSendCallback(sendCallback);
  //Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
  //Clear all data from Email object to free memory
  smtpData.empty();
  Serial.println("Going to sleep ...");
}
*/

/*
 * OLD CODE WHEN I WAS USING VIRTUAL PINS
 * #define PIN_REED2 V2;
BLYNK_READ(PIN_REED2)
{
  // This command writes Arduino's uptime in seconds to Virtual Pin (5)
  Blynk.virtualWrite(PIN_UPTIME, millis() / 1000);
}
*/


void setup() {
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  //Serial.begin(115200);
  //Serial.println();
  pinMode(4,INPUT_PULLDOWN); // these may not be needed, but this is what got it working so who knows
  pinMode(5,INPUT_PULLDOWN);
  Blynk.begin(auth, ssid, password); 

  // Define buzzer pin as output
  pinMode(BUZZER, OUTPUT);
  //Blynk.email("veronicagong2021@u.northwestern.edu", "Security Alert", "Front door is open!");
  //Blynk.notify("Security Alert - Front door is open!");
}


void loop() {
  Blynk.run();

  // Update sensor status in Blynk
  DisplayState(alarm_pin_1);
  DisplayState(alarm_pin_2);

  // Play alarm sound
  if (buzzerPin == HIGH) {
    Alarm();
  } else {
    noTone(BUZZER);
  }

  // Send email and notification if window 
  // or door is OPEN while system is ARMED
//  if (armPin == HIGH) {
//    if (DisplayState(alarm_pin_1) == HIGH) {
//      // As an example, alarm_pin_1 can be Front Door
//      Blynk.email("veronicagong2021@u.northwestern.edu", "Security Alert", "Front door is open!");
//      Blynk.notify("Security Alert - Front door is open!");
//    }
//    if (DisplayState(alarm_pin_2) == HIGH) {
//      // As an example, alarm_pin_2 can be Bedroom Window
//      Blynk.email("veronicagong2021@u.northwestern.edu", "Security Alert", "Bedroom window is open!");
//      Blynk.notify("Security Alert - Bedroom window is open!");
//    }
//  }
  
  /*
  delay(500);
  int test = digitalRead(4);
  if ((test == LOW) && (alreadyRan == false)) {
    alreadyRan = true;
    //savetoSD();
    delay(500);
    //sendEmail();
  }
  */
}


int DisplayState(int alarm_pin) {
  nodeStatus = digitalRead(alarm_pin);
  if (nodeStatus == LOW) {
    Blynk.virtualWrite(alarm_pin, "CLOSED");    // Function to send to the Blynk virtual pins
  } else if (nodeStatus == HIGH) {
    Blynk.virtualWrite(alarm_pin, "OPEN");    // Function to send to the Blynk virtual pins
  }
  return nodeStatus;
}

void Alarm() {
  tone(BUZZER,NOTE_FS5);
  delay(500);
  tone(BUZZER,NOTE_C5);
  delay(400);
}


// Functions to read from the Blynk virtual pins
BLYNK_WRITE(V1) {
  buzzerPin = param.asInt(); 
}
BLYNK_WRITE(V0) {
  armPin = param.asInt();
}
