#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <math.h>

#define PI 3.14159265359

char * dev_mouse = "/dev/input/event4";

// Prints the given error message and the value of errno. The program is halted
void fuck(char * s) {
    if (s != NULL)
        fprintf(stderr, "\nA fuck was given: %s\n", s);
    else
        fprintf(stderr, "A fuck was given.\n");

    fprintf(stderr, "errno says: %s\n\n", strerror(errno));
    exit(1); // One fuck given
}

int main(int argc, char const *argv[]) {
    // Open the device
    int fd = open(dev_mouse, O_WRONLY);
    if (fd < 0)
        fuck("Failed to open device");

    // Create a mouse input event
    struct input_event ev;
    ev.type = EV_REL;   // Relative movement

    // Inject input events to create a circle
    int ret;
    double t = 0, dx, dy, A, T;
    while(t < 60) {
        T = 1; A = 10;
        dx = A  * sin(t * 2 * PI / T);
        dy = A  * cos(t * 2 * PI / T);

        ev.code = REL_X; // On the X axis
        ev.value = (int) dx;
        ret = write(fd, &ev, sizeof(ev));

        ev.code = REL_Y; // On the Y axis
        ev.value = (int) dy;
        ret += write(fd, &ev, sizeof(ev));

        if (ret < 0)
            fuck("Failed to write");
        if (ret == 0)
            fuck("Nothing written");
        else if (ret < 2 * sizeof(ev))
            fuck("Partial write");

        t += 0.01;

        usleep(10000);
    }
    return 0;
}
