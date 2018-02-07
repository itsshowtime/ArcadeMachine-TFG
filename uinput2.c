#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <errno.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include <bcm2835.h>

#define PIN RPI_V2_GPIO_P1_29
#define die(str, args...) do { \
        perror(str); \
        exit(EXIT_FAILURE); \
    } while(0)

// I2C SETTINGS
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
// END OF I2C SETTINGS

void get_i2c(){ //get i2C data on buf[]

  for(i = 0; i < MAX_LEN; ++i) buf[i] = 0;

  if(init == I2C_BEGIN)
  {
    if(!bcm2835_i2c_begin()){
      printf("begin failed, root\n");
    }
  }

  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626);
  
  for(i = 0; i < MAX_LEN; ++i){
    data = bcm2835_i2c_read(buf, len);
    //printf("Read result = %d\n", data);
    //for(i = 0; i < MAX_LEN; ++i) printf("read buf[%d] = %x\n", i, buf[i]);
  } 

  if(init == I2C_END) bcm2835_i2c_end();	
	
}

void emit(int fd, int type, int code, int val) { //send a key stroke to OS
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   if(write(fd, &ie, sizeof(ie)) == -1) die("error: write");
}

int main(void) {
	
    if(!bcm2835_init()){
      printf("init failed, root\n");
      return 1;
    }

    // SETTING UINPUT
    struct uinput_setup usetup;

    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    if(fd < 0) die("error: open");
    // add keystroke and repeat events
    if(ioctl(fd, UI_SET_EVBIT, EV_KEY) == -1) die("error: ioctl"); 
    if(ioctl(fd, UI_SET_EVBIT, EV_REP) == -1) die("error: ioctl");
    // add joystick keys
	if(ioctl(fd, UI_SET_KEYBIT, KEY_W) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_A) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_S) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_D) == -1) die("error: ioctl");	
    if(ioctl(fd, UI_SET_KEYBIT, KEY_0) == -1) die("error: ioctl");	
    if(ioctl(fd, UI_SET_KEYBIT, KEY_1) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_2) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_3) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_4) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_5) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_6) == -1) die("error: ioctl");	
    if(ioctl(fd, UI_SET_KEYBIT, KEY_7) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_8) == -1) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_9) == -1) die("error: ioctl");	

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; /* sample vendor */
    usetup.id.product = 0x5678; /* sample product */
    strcpy(usetup.name, "Arcade Machine Analog and Buttons");

    if(ioctl(fd, UI_DEV_SETUP, &usetup) == -1)
		die("error: ioctl");
    if(ioctl(fd, UI_DEV_CREATE) == -1)
		die("error: ioctl");
    sleep(1);

    while(1) {
		
		get_i2c();
		//send all key status
		emit(fd, EV_KEY, KEY_0, buf[0]);
		emit(fd, EV_KEY, KEY_1, buf[0]);
		emit(fd, EV_KEY, KEY_2, buf[0]);
		emit(fd, EV_KEY, KEY_3, buf[0]);
		emit(fd, EV_KEY, KEY_4, buf[0]);
		emit(fd, EV_KEY, KEY_5, buf[0]);
		emit(fd, EV_KEY, KEY_6, buf[0]);
		emit(fd, EV_KEY, KEY_7, buf[0]);
		emit(fd, EV_KEY, KEY_8, buf[0]);
		emit(fd, EV_KEY, KEY_9, buf[0]);
		emit(fd, EV_KEY, KEY_W, buf[0]);
		emit(fd, EV_KEY, KEY_A, buf[0]);
		emit(fd, EV_KEY, KEY_S, buf[0]);
		emit(fd, EV_KEY, KEY_D, buf[0]);
		//report all keys at once
		emit(fd, EV_SYN, SYN_REPORT, 0);
		//wait some time until next poll
        delay(30);
    }

    sleep(1);

    if(ioctl(fd, UI_DEV_DESTROY) == -1) die("error: ioctl");
    close(fd);
	bcm2835_close();

    return 0;
}
