#include "NRF24L01.h"

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  3   // 3 unsigned chars TX payload
unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = 
{
    0x34,0x43,0x10,0x10,0x01
}; 
unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

void setup()
{
    Serial.begin(9600);
    while (!Serial) {
      ;
    }
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x11;

    NRF_Init();                        // Initialize IO port
    Serial.println("TX_Mode Start");    
}

void loop()
{
    NRF_SeTxMode();
    do
    { 
        NRF_Send(tx_buf);
    }
 while(!NRF_CheckAck());
}
