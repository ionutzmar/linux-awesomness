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

char * dev_input = "/dev/input/";

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
    // Check for parameter
    if (argc < 2) {
        printf("Usage: circle event_file\n\n");
        printf("  event_file                  event file name inside /dev/input/.\n\n");
        printf("To find out the event file for your mouse\n");
        printf("  cat /proc/bus/input/devices | grep mouse\n");
        printf("Then look for 'eventx' where x is a number.\n");
        return 0;
    }

    // Open the device
    char * path = (char *) malloc((strlen(dev_input) + 32) * sizeof(char));
    strcpy(path, dev_input);
    strncat(path, argv[1], 31);
    int fd = open(path, O_WRONLY);
    if (fd < 0)
        fuck("Failed to open device");
    free(path);

    // Create a mouse input event
    struct input_event ev;

    // Inject input events to create a circle
    int ret;
    double t = 0, dx, dy, A, T;
    while(t < 60) {
        T = 1; A = 10;
        dx = A  * sin(t * 2 * PI / T);
        dy = A  * cos(t * 2 * PI / T);

        ev.type = EV_REL;   // Relative movement
        ev.code = REL_X; // On the X axis
        ev.value = (int) dx;
        ret = write(fd, &ev, sizeof(ev));

        if (ret < 0)
            fuck("Failed to write");
        if (ret == 0)
            fuck("Nothing written");
        else if (ret < sizeof(ev))
            fuck("Partial write");

        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        ret = write(fd, &ev, sizeof(ev));

        if (ret < 0)
            fuck("Failed to write");
        if (ret == 0)
            fuck("Nothing written");
        else if (ret < sizeof(ev))
            fuck("Partial write");

        ev.type = EV_REL;   // Relative movement
        ev.code = REL_Y; // On the Y axis
        ev.value = (int) dy;
        ret += write(fd, &ev, sizeof(ev));

        if (ret < 0)
            fuck("Failed to write");
        if (ret == 0)
            fuck("Nothing written");
        else if (ret <  sizeof(ev))
            fuck("Partial write");

        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        ret = write(fd, &ev, sizeof(ev));

        if (ret < 0)
            fuck("Failed to write");
        if (ret == 0)
            fuck("Nothing written");
        else if (ret < sizeof(ev))
            fuck("Partial write");

        t += 0.01;

        usleep(10000);
    }
    return 0;
}
