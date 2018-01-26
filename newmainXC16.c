/*
 * File:   newmainXC16.c
 * Author: Kilian
 *
 * Created on 22 January 2018, 16:41
 */
#define USE_AND_OR

#include "xc.h"
#include <p24F16KA101.h>
#include <i2c.h>
#include <p24Fxxxx.h>
        
volatile char write_byte;

#define Fosc (8000000) // 8MHz crystal
#define Fcy (Fosc*4/2) // w.PLL (Instruction Per Second)
#define Fsck 400000    // 1 MHz I2C
#define I2C1_BRG ((Fcy/2/Fsck)-1)

#define SLAVE_ADD 15   // slave address

 typedef enum  {
   STATE_WAIT_FOR_ADDR,
   STATE_WAIT_FOR_WRITE_DATA,
   STATE_SEND_READ_DATA,
   STATE_SEND_READ_LAST
 } STATE;
 volatile STATE e_mystate = STATE_WAIT_FOR_ADDR;
 
#define BUFFSIZE 15
 volatile char  read_buffer[BUFFSIZE]; //master write slave read
 volatile char  write_buffer[BUFFSIZE];//master read slave write
 volatile uint16_t buffer_index;

void __attribute__((__interrupt__, no_auto_psv )) _SI2C1Interrupt(void) {
    uint8_t u8_c;
    IFS1bits.SI2C1IF = 0;
    //int i;
    
    switch (e_mystate) {
       
    case STATE_WAIT_FOR_ADDR:
        u8_c = I2C1RCV; //clear RBF bit for address
        buffer_index = 0;
        
        if (I2C1STATbits.R_W) { //check the R/W bit and see if read or write transaction
            I2C1TRN = write_buffer[buffer_index];  //send first data byte
            buffer_index++;
            I2C1CONbits.SCLREL = 1;     //release clock line so MASTER can drive it
            e_mystate = STATE_SEND_READ_DATA; //read transaction
        } 
        else e_mystate = STATE_WAIT_FOR_WRITE_DATA;
        break;     
     
    case STATE_WAIT_FOR_WRITE_DATA:
       //character arrived, place in buffer
       read_buffer[buffer_index] = I2C1RCV;  //read the byte
       
       if (read_buffer[buffer_index] == 0) {
           e_mystate = STATE_WAIT_FOR_ADDR; //wait for next transaction
       }
       break;
    
    case STATE_SEND_READ_DATA:
       I2C1TRN = write_buffer[buffer_index];
       buffer_index++;
       I2C1CONbits.SCLREL = 1;     //release clock line so MASTER can drive it
       if (write_buffer[buffer_index] == 8) e_mystate = STATE_SEND_READ_LAST;
         //this is the last character, after byte is shifted out, release the clock line again
       break;
    
    case STATE_SEND_READ_LAST:  //this is interrupt for last character finished shifting out
        //for(i = 0; i < BUFFSIZE - 1; ++i) {
        //    write_buffer[i] = 5;
        //}
        //i = 0;
        e_mystate = STATE_WAIT_FOR_ADDR;
       break;
    
    default:
       e_mystate = STATE_WAIT_FOR_ADDR;
   }
 }

int main (void) {
    unsigned int i = 0; 
    TRISA = 0xFFFF;
    TRISB = 0xFFFF;
    
    for(i = 0; i < BUFFSIZE - 1; ++i) {
        read_buffer[i] = 5;
        write_buffer[i] = 5;
    }   
    read_buffer[BUFFSIZE - 1]  = 8;
    write_buffer[BUFFSIZE - 1] = 8;
        
    I2C1CONbits.I2CEN   = 1;
    I2C1CONbits.I2CSIDL = 0;    
    I2C1CONbits.IPMIEN  = 0;
    I2C1CONbits.A10M    = 0;
    I2C1CONbits.DISSLW  = 1;
    I2C1CONbits.SMEN    = 0;
    I2C1CONbits.GCEN    = 0;
    I2C1CONbits.STREN   = 1;
   
    I2C1BRG = I2C1_BRG;
    I2C1ADD = SLAVE_ADD;
    
    IFS1bits.SI2C1IF = 0;
    IPC4bits.SI2C1P2 = 1;
    IPC4bits.SI2C1P1 = 1;
    IPC4bits.SI2C1P0 = 1;
    IEC1bits.SI2C1IE = 1;
    
    while(1){
        while(i < 100){++i;}
        i = 0;
        if (PORTBbits.RB4){
            write_buffer[4] = 1;
        }
        else write_buffer[4] = 0;
    }
    return 0;
}

/*
int main(void) { 
    unsigned int i = 0; 
 
    CloseI2C1();                            // close i2c if was operating earlier 
    TRISB = 0xFFFF;
    OpenI2C1((I2C_ON | I2C_IDLE_CON | I2C_7BIT_ADD | I2C_CLK_REL | I2C_STR_EN), I2C1_BRG); 
    I2C1ADD = 15;                     // initialze slave address to 1E 
    //I2C1BRG = 37;
    
    while(1) { 
        IdleI2C1();
        //while(!I2C1STATbits.S);
        //SI2C1_Clear_Intr_Status_Bit;
        //while(!I2C1STATbits.R_W);
        //SI2C1_Clear_Intr_Status_Bit;
        while(!IFS1bits.SI2C1IF);
        SlaveWriteI2C1(write_byte);
        write_byte = 0;
        //StopI2C1(); 
        
        while(i < 100){++i;}
          i = 0;
          if (PORTBbits.RB4)
          {
              write_byte = 1;
          }  
 
    } 
 
    CloseI2C1(); 
    return 0;
 }
 * */