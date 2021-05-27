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
char ssid[] = "Device-Northwestern";   //"MOTOE632";
char password[] = "";   //"97s0vv1tkr";
char auth[] = "KCRUeBA60hgY-Hkhfy52R2A3zgAIwO2L";


// To send Email using Gmail use port 465 (SSL) and SMTP Server smtp.gmail.com
// YOU MUST ENABLE less secure app option https://myaccount.google.com/lesssecureapps?pli=1
#define emailSenderAccount    ""    
#define emailSenderPassword   ""
#define emailRecipient        ""
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort         465 //587 //465
#define emailSubject          "ESP32_SndPicture"
//SMTPData smtpData;

int pictureNumber = 0;
int d5_val = 0;
bool alreadyRan = false;

/*
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
  
   
}
void loop() {
  Blynk.run();
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
