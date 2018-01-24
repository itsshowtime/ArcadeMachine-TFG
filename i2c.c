#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MODE_READ 1
#define MODE_WRITE 0

#define MAX_LEN 32

char wbuf[MAX_LEN];

typedef enum {
    NO_ACTION, I2C_BEGIN, I2C_END
} i2c_init;

uint8_t init = NO_ACTION;
uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
uint8_t slave_address = 0x1E;
uint32_t len = 0;
uint8_t mode = MODE_READ;

char buf[MAX_LEN];
int i;
uint8_t data;

int main(int argc, char **argv){

  if(!bcm2835_init()){
    printf("init failed, running as root?\n");
    return 1;
  }

  if(init == I2C_BEGIN)
  {
    if(!bcm2835_i2c_begin()){
      printf("begin failed, running as root?\n");
      return 1;
    }
  }

  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(clk_div);
  
  for(i = 0; i<MAX_LEN; ++i) buf[i] = 'n';
  data = bcm2835_i2c_read(buf,len);
  printf("read result = %d\n", data);
  for(i = 0; i < MAX_LEN; ++i){
    if(buf[i] != 'n') printf("read buf[%d] = %x\n", i, buf[i]);
  }
}
