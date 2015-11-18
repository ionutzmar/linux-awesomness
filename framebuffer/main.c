#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdint.h>

static int color;

void set_pixel(volatile uint8_t *buffer, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, struct fb_var_screeninfo vinfo, struct fb_fix_screeninfo finfo)
{
	/* Get memory location */
	int location = ((x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length);

 	if (vinfo.bits_per_pixel == 32) {
		*(buffer + location) = r;
		*(buffer + location + 1) = g;
		*(buffer + location + 2) = b;
		*(buffer + location + 3) = a;
	}
}

int main(int argc, char ** argv)
{
		int fbfd = 0;
		struct fb_var_screeninfo vinfo;
		struct fb_fix_screeninfo finfo;
		long int screensize = 0;
		volatile uint8_t *fbp = 0;

		/* Open the file for reading and writing */
		fbfd = open("/dev/fb0", O_RDWR);
		if (fbfd < 0) {
				fprintf(stderr, "Error: cannot open framebuffer device.\n");
				exit(-1);
		}

		/* Get fixed screen information */
		if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
			    fprintf(stderr, "Error reading fixed information.\n");
				exit(-1);
		}

		/* Get variable screen information */
		if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
				fprintf(stderr, "Error reading variable information.\n");
				exit(-1);
		}

		/* Figure out the size of the screen in bytes */
		screensize = vinfo.yres_virtual * finfo.line_length;

		/* Map the device to memory */
		fbp = (uint8_t *) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
		if (fbp == MAP_FAILED) {
				fprintf(stderr, "Error: failed to map framebuffer device to memory.\n");
				exit(-1);
		}

		/* MADNESS! Crash or kernel panic is imminent! */

		int pattern = 0;
		int i, j, k, a;
		if (argc > 1) sscanf(argv[1], "%i", &pattern);
		switch (pattern) {
			case 1:
				printf("Grid pattern selected. Press enter to continue\n"); getchar();
				while (1) {
					a = a % 15 + 1;
					for (i = 0; i < vinfo.xres_virtual; i++) {
						for (j = 0; j < vinfo.yres_virtual; j++) {
							k = ((i % 15 == j % 15) || ((i + j + 16 - a) % 15 == 0)) * 150;
							set_pixel(fbp, i, j, k, k, 0, 0, vinfo, finfo);
						}
					}
				}
				break;
			case 2:
				printf("Cancer pattern selected (NOT RECOMMENDED). Press enter to continue\n"); getchar();
				while (1) {
					a = a % 15 + 1;
					for (i = 0; i < vinfo.xres_virtual; i++) {
						for (j = 0; j < vinfo.yres_virtual; j++) {
							k = (i % 15 == j % 15) || ((i + j + 16 - a) % 15 == 0);
							set_pixel(fbp, i, j, (i - j + a * 10) * k, (i * j) * k, ((a % 7 + 1) * i - j) * k, 0, vinfo, finfo);
						}
					}
				}
				break;
			default:
				printf("Spectrum pattern selected. Press enter to continue\n"); getchar();
				while (1) {
					color += 5;
					for (i = 0; i < vinfo.xres_virtual; i++) {
						for (j = 0; j < vinfo.yres_virtual; j++) {
							set_pixel(fbp, i, j, i, j, color + i - j, 0, vinfo, finfo);
						}
					}
				}
		}

		/* Clean up */
		munmap((uint8_t *) fbp, screensize);
		close(fbfd);

		return 0;
}
