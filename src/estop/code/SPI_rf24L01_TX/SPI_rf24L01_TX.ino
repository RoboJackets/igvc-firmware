/*********************************************************************
**  Device:  nRF24L01+                                              **
**  File:   EF_nRF24L01_TX.c                                        **
**                                                                  **
**                                                                  **                                                                **
**********************************************************************
**   SPI-compatible                                                 **
**   CS -  to digital pin 8                                         **
**   CSN - to digital pin 9   (SS pin)                              **
**   IRQ - to digital pin 3  (IRQ pin)                             **
**   MOSI - to digital pin 11 (MOSI pin)                            **
**   MISO - to digital pin 12 (MISO pin)                            **
**   CLK - to digital pin 13  (SCK pin)                             **
*********************************************************************/

#include <SPI.h>
#include "API.h"

//***************************************************

#define IN_PIN 8 // estop button in pin
#define BTN_LED 5 // estop on/off status LED
#define WIRELESS_LED 6 // radio connection status LED
#define CE       20
// CE_BIT:   Digital Input     Chip Enable Activates RX or TX mode
#define CSN      21
// CSN BIT:  Digital Input     SPI Chip Select
#define IRQ      10
// IRQ BIT:  Digital Output    Maskable interrupt pin

//***************************************************

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  1  // 32 unsigned chars TX payload

unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = 
{
  0x34,0x43,0x10,0x10,0x01
}; // Define a static TX address

unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

//***************************************************

int estopStatus = 0;

//***************************************************

void setup() 
{
  pinMode(CE,  OUTPUT);
  pinMode(CSN, OUTPUT);
  pinMode(IRQ, INPUT);
  SPI.begin();
  delay(50);
  init_io();                        // Initialize IO port
  unsigned char sstatus=SPI_Read(STATUS);
  TX_Mode();                       // set TX mode
  pinMode(IN_PIN, INPUT);
  digitalWrite(IN_PIN, HIGH);
  pinMode(BTN_LED, OUTPUT);
  pinMode(WIRELESS_LED, OUTPUT);
}

void loop() 
{
  int k = !digitalRead(IN_PIN);
  if (k != estopStatus) {
    digitalWrite(WIRELESS_LED, HIGH);
    estopStatus = k;
  } else {
    digitalWrite(WIRELESS_LED, LOW);
  }
  
  digitalWrite(BTN_LED, estopStatus);
  tx_buf[0] = estopStatus;
  unsigned char sstatus = SPI_Read(STATUS);                   // read register STATUS's value
  if(sstatus&TX_DS)                                           // if receive data ready (TX_DS) interrupt
  {
    SPI_RW_Reg(FLUSH_TX,0);                                  
    SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);       // write playload to TX_FIFO
  }
  if(sstatus&MAX_RT)                                         // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR                          
  {
    SPI_RW_Reg(FLUSH_TX,0);
    SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);      // disable standy-mode
  }
  SPI_RW_Reg(WRITE_REG+STATUS,sstatus);                     // clear RX_DR or TX_DS or MAX_RT interrupt flag
  delay(50);
}

//**************************************************
// Function: init_io();
// Description:
// flash led one time,chip enable(ready to TX or RX Mode),
// Spi disable,Spi clock line init high
//**************************************************
void init_io(void)
{
  digitalWrite(IRQ, LOW);
  digitalWrite(CE, LOW);			// chip enable
  digitalWrite(CSN, HIGH);                 // Spi disable	
}

/************************************************************************
**   * Function: SPI_RW();
 * 
 * Description:
 * Writes one unsigned char to nRF24L01, and return the unsigned char read
 * from nRF24L01 during write, according to SPI protocol
************************************************************************/
unsigned char SPI_RW(unsigned char Byte)
{
  return SPI.transfer(Byte);
}

/**************************************************/

/**************************************************
 * Function: SPI_RW_Reg();
 * 
 * Description:
 * Writes value 'value' to register 'reg'
/**************************************************/
unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value)
{
  unsigned char status;

  digitalWrite(CSN, LOW);                   // CSN low, init SPI transaction
  SPI_RW(reg);                            // select register
  SPI_RW(value);                          // ..and write value to it..
  digitalWrite(CSN, HIGH);                   // CSN high again

  return(status);                   // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
 * Function: SPI_Read();
 * 
 * Description:
 * Read one unsigned char from nRF24L01 register, 'reg'
/**************************************************/
unsigned char SPI_Read(unsigned char reg)
{
  unsigned char reg_val;

  digitalWrite(CSN, LOW);                // CSN low, initialize SPI communication...
  SPI_RW(reg);                         // Select register to read from..
  reg_val = SPI_RW(0);                 // ..then read register value
  digitalWrite(CSN, HIGH);                // CSN high, terminate SPI communication

  return(reg_val);                     // return register value
}
/**************************************************/

/**************************************************
 * Function: SPI_Read_Buf();
 * 
 * Description:
 * Reads 'unsigned chars' #of unsigned chars from register 'reg'
 * Typically used to read RX payload, Rx/Tx address
/**************************************************/
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
  unsigned char sstatus,i;

  digitalWrite(CSN, LOW);                   // Set CSN low, init SPI tranaction
  sstatus = SPI_RW(reg);       	    // Select register to write to and read status unsigned char

  for(i=0;i<bytes;i++)
  {
    pBuf[i] = SPI_RW(0);    // Perform SPI_RW to read unsigned char from nRF24L01
  }

  digitalWrite(CSN, HIGH);                   // Set CSN high again

  return(sstatus);                  // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
 * Function: SPI_Write_Buf();
 * 
 * Description:
 * Writes contents of buffer '*pBuf' to nRF24L01
 * Typically used to write TX payload, Rx/Tx address
/**************************************************/
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
  unsigned char sstatus,i;

  digitalWrite(CSN, LOW);                   // Set CSN low, init SPI tranaction
  sstatus = SPI_RW(reg);             // Select register to write to and read status unsigned char
  for(i=0;i<bytes; i++)             // then write all unsigned char in buffer(*pBuf)
  {
    SPI_RW(*pBuf++);
  }
  digitalWrite(CSN, HIGH);                   // Set CSN high again
  return(sstatus);                  // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
 * Function: TX_Mode();
 * 
 * Description:
 * This function initializes one nRF24L01 device to
 * TX mode, set TX address, set RX address for auto.ack,
 * fill TX payload, select RF channel, datarate & TX pwr.
 * PWR_UP is set, CRC(2 unsigned chars) is enabled, & PRIM:TX.
 * 
 * ToDo: One high pulse(>10us) on CE will now send this
 * packet and expext an acknowledgment from the RX device.
 **************************************************/
void TX_Mode(void)
{
  digitalWrite(CE, LOW);

  SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
  SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack

  SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
  SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 unsigned chars) & Prim:TX. MAX_RT & TX_DS enabled..
  SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);

  digitalWrite(CE, HIGH);
}
