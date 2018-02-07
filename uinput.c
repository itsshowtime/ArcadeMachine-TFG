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
		
int main(void) {
	
    if(!bcm2835_init()){
      printf("init failed, root\n");
      return 1;
    }

    int                    fd;
    struct uinput_user_dev uidev;
    struct input_event     ev;

    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);

	// SETTING UINPUT
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_APPEND);
    if(fd < 0)
        die("error: open");
    if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        die("error: ioctl"); 
    if(ioctl(fd, UI_SET_EVBIT, EV_REP) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_W) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_A) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_S) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_D) < 0)
        die("error: ioctl");	
    if(ioctl(fd, UI_SET_KEYBIT, KEY_0) < 0)
        die("error: ioctl");	
    if(ioctl(fd, UI_SET_KEYBIT, KEY_1) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_2) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_3) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_4) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_5) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_6) < 0)
        die("error: ioctl");	
    if(ioctl(fd, UI_SET_KEYBIT, KEY_7) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_8) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_9) < 0)
        die("error: ioctl");	

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "analog stroker");
    uidev.id.bustype = BUS_VIRTUAL;
    uidev.id.vendor  = 1;
    uidev.id.product = 2;
    uidev.id.version = 3;

    if(write(fd, &uidev, sizeof(uidev)) < 0)
        die("error: write");

    if(ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl");

    sleep(2);

    srand(time(NULL));

    while(1) {
		
		get_i2c();
		
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_0;
        ev.value = buf[0];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_1;
        ev.value = buf[1];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_2;
        ev.value = buf[2];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_3;
        ev.value = buf[3];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_4;
        ev.value = buf[4];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_5;
        ev.value = buf[5];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_6;
        ev.value = buf[6];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_7;
        ev.value = buf[7];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_8;
        ev.value = buf[8];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_9;
        ev.value = buf[9];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_W;
        ev.value = buf[10];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_A;
        ev.value = buf[11];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_S;
        ev.value = buf[12];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_D;
        ev.value = buf[13];
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");		
		
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_SYN;
        ev.code = 0;
        ev.value = 0;
        if(write(fd,&ev,sizeof(struct input_event)) < 0)
            die("error: write");
        delay(30);
    }

    sleep(2);

    if(ioctl(fd, UI_DEV_DESTROY) < 0) die("error: ioctl");
    close(fd);

	bcm2835_close();
    
	return 0;
}
