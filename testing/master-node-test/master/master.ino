/*********************************************************************
**   SPI-compatible                                                 **
**   CE - to digital pin 9                                          **
**   CSN - to digital pin 10  (SS pin)                               **
**   SCK - to digital pin 13 (SCK pin)                              **
**   MOSI - to digital pin 11 (MOSI pin)                            **
**   MISO - to digital pin 12 (MISO pin)                            **
**   IRQ - to digital pin 8 (MISO pin)                             **
*********************************************************************/
#include "NRF24L01.h"

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  3  // 32 unsigned chars TX payload
unsigned char RX_ADDRESS[TX_ADR_WIDTH]  = 
{
  0x34,0x43,0x10,0x10,0x01
};
unsigned char rx_buf[TX_PLOAD_WIDTH] = {0};
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

void setup()
{
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect, needed for native USB
    }
    NRF_Init();                        // Initialize IO     
    NRF_SetRxMode();
    Serial.println("RX_Mode start...");
}

void loop()
{
    NRF_SetRxMode();
    if(NRF_Receive(rx_buf))
    {
        Serial.print("RX = ");
        for(int i = 0; i < 3; i++)
        {
            Serial.print(rx_buf[i]);
            Serial.print(",");
        }
        Serial.println();
        for(int i = 0; i < 3; i++)
            rx_buf[i] = 0;
    }
    delay(500);
}
