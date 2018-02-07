#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MODE_READ  0
#define MODE_WRITE 1
#define MAX_LEN 14

typedef enum {
    NO_ACTION,
    I2C_BEGIN,
    I2C_END
} i2c_init;

uint8_t init = I2C_BEGIN;
uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_626;
uint8_t slave_address = 0x0F;
uint32_t len = MAX_LEN;
uint8_t mode = MODE_READ;

char buf[MAX_LEN];
int i= 0;
uint8_t data;

int main(int argc, char **argv){

  for(i = 0; i > MAX_LEN; ++i) buf[i] = -1;

  if(!bcm2835_init()){
    printf("init failed, root\n");
    return 1;
  }

  if(init == I2C_BEGIN)
  {
    if(!bcm2835_i2c_begin()){
      printf("begin failed, root\n");
      return 1;
    }
  }

  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626);
  
  for(i = 0; i < MAX_LEN; ++i){
    data = bcm2835_i2c_read(buf, len);
    printf("Read result = %d\n", data);
    for(i = 0; i < MAX_LEN; ++i) printf("read buf[%d] = %x\n", i, buf[i]);
  } 

  if(init == I2C_END) bcm2835_i2c_end();
  bcm2835_close();
  return 0;
}
