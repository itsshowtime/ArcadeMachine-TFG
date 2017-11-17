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

int main(void) {
    if (!bcm2835_init())
        return 1;

    int                    fd;
    struct uinput_user_dev uidev;
    struct input_event     ev;

    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);

    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_APPEND);
    if(fd < 0)
        die("error: open");
    if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_K) < 0)
        die("error: ioctl");

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "k stroker");
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
        uint8_t dx = bcm2835_gpio_lev(PIN);
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = KEY_K;
        ev.value = dx;
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            die("error: write");

        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_SYN;
        ev.code = 0;
        ev.value = 0;
        if(write(fd,&ev,sizeof(struct input_event)) < 0)
            die("error: write");
        delay(20);
/*
        for(i = 0; i < 100; i++) {
            memset(&ev, 0, sizeof(struct input_event));
            ev.type = EV_KEY;
            ev.code = KEY_K;
            ev.value = dx;
            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                die("error: write");

            memset(&ev, 0, sizeof(struct input_event));
            ev.type = EV_KEY;
            ev.code = KEY_K;
            ev.value = dx;
            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                die("error: write");

            memset(&ev, 0, sizeof(struct input_event));
            ev.type = EV_SYN;
            ev.code = 0;
            ev.value = 0;
            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                die("error: write");

            usleep(15000);
            if(i==99) exit(0);
        }

        sleep(5);
*/    }

    sleep(2);

    if(ioctl(fd, UI_DEV_DESTROY) < 0)
        die("error: ioctl");

    close(fd);

    return 0;
}
