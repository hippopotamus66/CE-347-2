#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
#include "SD.h"
#include <WiFi.h>
#include "ESP32_MailClient.h"
#include "SPIFFS.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "driver/rtc_io.h"
#include <BlynkSimpleEsp32.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
char ssid[] = "";
char password[] = "";
char auth[] = "";


// To send Email using Gmail use port 465 (SSL) and SMTP Server smtp.gmail.com
// YOU MUST ENABLE less secure app option https://myaccount.google.com/lesssecureapps?pli=1
#define emailSenderAccount    ""    
#define emailSenderPassword   ""
#define emailRecipient        ""
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort         465 //587 //465
#define emailSubject          "ESP32_SendPicture"
SMTPData smtpData;

#define CAMERA_MODEL_AI_THINKER

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define EEPROM_SIZE 1
int pictureNumber = 0;
bool alreadyRan = false;

void sendCallback(SendStatus msg) {
  // Print the current status
  Serial.println(msg.info());

  // Do something when complete
  if (msg.success()) {
    Serial.println("----------------");
  }
}

static esp_err_t get_image(fs::FS &fs, const char * path) {
    // Variable definitions for camera frame buffer
    camera_fb_t * fb = NULL;
    int64_t fr_start = esp_timer_get_time();
    delay(100);
    // Get the contents of the camera frame buffer
    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed\n");
        return ESP_FAIL;
    }

    // Save image to file
    size_t fb_len = 0;
    fb_len = fb->len;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
      Serial.println("Failed to open file in writing mode\n");
    } else {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.printf("Saved file to path: %s\n", path);
    }
    file.close();

    // Release the camera frame buffer
    esp_camera_fb_return(fb);
    int64_t fr_end = esp_timer_get_time();
    Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start)/1000));
    return ESP_OK;
}

void savetoSD() {
  Serial.print("Starting savetoSD function\n");
  delay(100);

  // Start microSD
  Serial.println("Starting SD Card\n");

  delay(500);
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed\n");
    return;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached\n");
    return;
  }

  Serial.println("SD card mounted\n");
  
  pictureNumber = EEPROM.read(0) + 1;
  
  String path = "/picture" + String(pictureNumber) + ".jpg";
  if(get_image(SD_MMC, path.c_str()) != ESP_OK ) {
    Serial.println("Failed to capture picture\n");
    ESP.restart();
  } else{
    Serial.println("Captured picture\n");
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
}

void sendEmail() {
  camera_fb_t * fb = NULL;
  delay(3000);
  // Take Picture with Camera
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed\n");
    ESP.restart();
  }
  String path = "/Sight.jpg";
  File file = SPIFFS.open(path, FILE_WRITE);

   if(!file){
    Serial.println("Failed to open file in writing mode\n");
  } 
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path);

  }
  file.close();
  esp_camera_fb_return(fb);

  Serial.printf("Successfully saved to SPIFFS\n");
  Serial.println("Preparing to send email\n");
  Serial.println();

  
  // Set the SMTP Server Email host, port, account and password
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);

  // For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be 
  // enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
  //smtpData.setSTARTTLS(true);

  // Set the sender name and Email
  smtpData.setSender("Home Camera", emailSenderAccount);

  // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
  smtpData.setPriority("High");

  // Set the subject
  smtpData.setSubject(emailSubject);

  // Set the message with HTML format
  smtpData.setMessage("<div style=\"color:#2f4468;\"><h1>Motion was detected!</h1><p>- Sent from ESP32 board</p></div>", true);
  // Set the email message in text format (raw)
  //smtpData.setMessage("Thing on position #00", false);

  // Add recipients, you can add more than one recipient
  smtpData.addRecipient(emailRecipient);
  //smtpData.addRecipient("YOUR_OTHER_RECIPIENT_EMAIL_ADDRESS@EXAMPLE.com");
 smtpData.setFileStorageType(MailClientStorageType::SPIFFS);
  smtpData.addAttachFile("/Sight.jpg");
 
  //smtpData.setFileStorageType(MailClientStorageType::SD);
  
  smtpData.setSendCallback(sendCallback);

  //Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

  //Clear all data from Email object to free memory
  smtpData.empty();

  Serial.println("Going to sleep ...");
  esp_sleep_enable_timer_wakeup(5 * 1000000);
  esp_deep_sleep_start();
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);
  Serial.println();

  Blynk.begin(auth, ssid, password);

camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("Camera initialized!\n");
  
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    Serial.println("SPIFFS mounted successfully\n");
  }
  SPIFFS.format();

  EEPROM.begin(EEPROM_SIZE);
   
}

void loop() {
  Blynk.run();
  
  delay(500);
  int test = digitalRead(13);   // GPIO13 is inverted, so PIR sensor will be low when detected
  int disarm = digitalRead(GPIO_NUM_12);     
  Serial.println(disarm);
  if ((test == LOW) && (alreadyRan == false) && (disarm == LOW)) {
    alreadyRan = true;
    savetoSD();
    delay(500);
    sendEmail();
  }
}
