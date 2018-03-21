// Copyright 2013 Pervasive Displays, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.  See the License for the specific language
// governing permissions and limitations under the License.

// device tree base GPIO
// dts files from:  https://github.com/sohal... 

// OS:
//   root@colibri-vf:~# uname -a; lsb_release -a
//   Linux kolibriWhite 4.4.107-dirty #1 Fri Mar 2 01:40:25 CET 2018 armv7l GNU/Linux
//   Distributor ID: Angstrom
//   Description:    Angstrom GNU/Linux v2016.12 (Core edition)
//   Release:        v2016.12
//   Codename:       Core edition



#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <err.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/stat.h>
#include "gpio.h"
#include "epd_io.h"

// GPIO states / direction
#define STATE_rxDisable_pullNone "rxDisable_pullNone"
#define STATE_rxEnable_pullNone  "rxEnable_pullNone"
#define STATE_rxDisable_pullUp   "rxDisable_pullUp"
#define STATE_rxEnable_pullUp    "rxEnable_pullUp"
#define STATE_rxDisable_pullDown "rxDisable_pullDown"
#define STATE_rxEnable_pullDown  "rxEnable_pullDown"

#define DIRECTION_in  "in"
#define DIRECTION_out "out"

// compute array size at compile time
#define SIZE_OF_ARRAY(a) (sizeof(a) / sizeof((a)[0]))

// get the size of a constant string at compile time
// no need for extra strlen at run time
// (note ignore the trailing '\0')
#define CONST_STRLEN(const_string) (sizeof(const_string) - sizeof((char)('\0')))

/**
 * Static local functions 
 * https://github.com/embeddedarm/ts4900-utils/blob/master/src/gpiolib.c
 * */
/* returns -1 or the file descriptor of the gpio value file */
static int gpio_export(int gpio);
/* Set direction to 2 = high output, 1 low output, 0 input */
static int gpio_direction(int gpio, int dir);
/* Release the GPIO to be claimed by other processes or a kernel driver */
static void gpio_unexport(int gpio);
/* Single GPIO read */
static int gpio_read(int gpio);
/* Set GPIO to val (1 = high) */
static int gpio_write(int gpio, int val);
/* Set which edge(s) causes the value select to return */
int gpio_setedge(int gpio, int rising, int falling);
/* Blocks on select until GPIO toggles on edge */
int gpio_select(int gpio);
/* Return the GPIO file descriptor */
int gpio_getfd(int gpio);
/*********************************************************************************************/
/**
 * My pwm functions
 * */
static int pwm_export(int pwm);
static int pwm_unexport(int pwm);
static int pwm_setperiod(int pwm, unsigned int frequency);
static int pwm_setduty(int pwm, int16_t value);
static int pwm_enable(int pwm);
static int pwm_disable(int pwm);
/*********************************************************************************************/
// set up access to the GPIO and PWM
bool GPIO_setup() {

	if(-1 == gpio_export(panel_on_pin))
	{
		return (false);
	}
	if(-1 == gpio_export(border_pin))
	{
		return (false);
	}
	if(-1 == gpio_export(discharge_pin))
	{
		return (false);
	}
	if(-1 == gpio_export(reset_pin))
	{
		return (false);
	}
	if(-1 == gpio_export(reset_pin))
	{
		return (false);
	}
	if(-1 == gpio_export(busy_pin))
	{
		return (false);
	}
	if(-1 == pwm_export(pwm_b))
	{
		return (false);
	}
	return true;
}


/// revoke access to GPIO and PWM
bool GPIO_teardown() {
	gpio_unexport(panel_on_pin);
	gpio_unexport(border_pin);
	gpio_unexport(discharge_pin);
	gpio_unexport(reset_pin);
	gpio_unexport(reset_pin);
	gpio_unexport(busy_pin);
	pwm_unexport(pwm_b);
	return true;
}


void GPIO_mode(GPIO_pin_type pin, GPIO_mode_type mode) {

	switch (mode) {
	default:
	case GPIO_INPUT:
		if(0 > gpio_direction(pin, 0))
		{
			;
		}
		break;

	case GPIO_OUTPUT:
		if(0 > gpio_direction(pin, 2))
		{
			;
		}
		break;

	case GPIO_PWM:  
		pwm_setperiod(pin, 500000);
		pwm_setduty(pin, 512);
		break;
	}
}


int GPIO_read(int pin) {
	return(gpio_read(pin));
}


void GPIO_write(GPIO_pin_type pin, int value) {
	return((void)gpio_write(pin, value));
}


// only affetct PWM if correct pin is addressed
void GPIO_pwm_write(int pin, uint32_t value) {
	if (value > 1023) {
		value = 1023;
	}
	pwm_setduty(pin, value);
}

static int pwm_setperiod(int pwm, unsigned int frequency)
{
	int ret = 0, nread;
	char buf[50];
	char val[16];
	sprintf(buf, "/sys/class/pwm/pwmchip8/pwm%d/period", pwm);
	int pwmfd_period = open(buf, O_RDWR | O_SYNC);
	
	if(pwmfd_period < 0) {
		perror("Couldn't open PWM file");
		ret = -1;
	}
	
	if (frequency < 0) {
		frequency = 500000;
	} else if (frequency > 500000) {
		frequency = 500000;
	}

	memset(val, 0, sizeof(val));

	int len = sprintf(val, "%d", frequency);

	if(len != write(pwmfd_period, val, len))
	{
		fprintf(stderr, "Couldn't set PWM %d Period to %d: %s\n", 
				pwm, frequency,
				strerror(errno));
		ret = -2;
	}

	close(pwmfd_period);
	return(ret);	
}

static int pwm_setduty(int pwm, int16_t value) {
	int ret = 0, nread;
	char buf[50];
	char val[16];

	pwm_disable(pwm);

	sprintf(buf, "/sys/class/pwm/pwmchip8/pwm%d/duty_cycle", pwm);
	int pwmfd_dutycycle = open(buf, O_RDWR | O_SYNC);
	
	if(pwmfd_dutycycle < 0) {
		perror("Couldn't open PWM file");
		ret = -1;
	}

	sprintf(buf, "/sys/class/pwm/pwmchip8/pwm%d/period", pwm);
	int pwmfd_period = open(buf, O_RDWR | O_SYNC);
	
	if(pwmfd_period < 0) {
		perror("Couldn't open PWM file");
		ret = -1;
	}

	
	if (value < 0) {
		value = 0;
	} else if (value > 1023) {
		value = 1023;
	}

	unsigned int period = 0U;

	memset(val, 0, sizeof(val));
	char x, i=0;
    do
	{
		nread = read(pwmfd_period, &x, 1);
		val[i++] = x;
		if(x == '\n' || '\0')
		{
			period = atoi(val);
		} 
	}while(nread != 0);

	unsigned int duty_cycle = value * period / 1023;

	memset(val, 0, sizeof(val));
	
	int len = sprintf(val, "%d\0", duty_cycle);

	if(len != write(pwmfd_dutycycle, val, len))
	{
		fprintf(stderr, "Couldn't set GPIO %d direction to out/high: %s\n", 
				pwm,
				strerror(errno));
		ret = -2;
	}
	close(pwmfd_dutycycle);
	close(pwmfd_period);
	pwm_enable(pwm);
	return(ret);
}

int gpio_direction(int gpio, int dir)
{
	int ret = 0;
	char buf[50];
	sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
	int gpiofd = open(buf, O_WRONLY | O_SYNC);
	if(gpiofd < 0) {
		perror("Couldn't open direction file");
		ret = -1;
	}

	if(dir == 2 && gpiofd){
		if (4 != write(gpiofd, "high", 4)) {
			fprintf(stderr, "Couldn't set GPIO %d direction to out/high: %s\n", 
				gpio,
				strerror(errno));
			ret = -2;
		}
	} else if(dir == 1 && gpiofd){
		if (3 != write(gpiofd, "out", 3)) {
			fprintf(stderr, "Couldn't set GPIO %d direction to out/low: %s\n", 
				gpio,
				strerror(errno));
			ret = -3;
		}
	} else if(gpiofd) {
		if(2 != write(gpiofd, "in", 2)) {
			fprintf(stderr, "Couldn't set GPIO %d direction to in: %s\n", 
				gpio,
				strerror(errno));
			ret = -4;
		}
	}

	close(gpiofd);
	return ret;
}

int gpio_setedge(int gpio, int rising, int falling)
{
	int ret = 0;
	char buf[50];
	sprintf(buf, "/sys/class/gpio/gpio%d/edge", gpio);
	int gpiofd = open(buf, O_WRONLY);
	if(gpiofd < 0) {
		perror("Couldn't open IRQ file");
		ret = -1;
	}

	if(gpiofd && rising && falling) {
		if(4 != write(gpiofd, "both", 4)) {
			perror("Failed to set IRQ to both falling & rising");
			ret = -2;
		}
	} else {
		if(rising && gpiofd) {
			if(6 != write(gpiofd, "rising", 6)) {
				perror("Failed to set IRQ to rising");
				ret = -2;
			}
		} else if(falling && gpiofd) {
			if(7 != write(gpiofd, "falling", 7)) {
				perror("Failed to set IRQ to falling");
				ret = -3;
			}
		}
	}

	close(gpiofd);
	return(ret);
}

/**
 *  export a pin in sysfs
 * 
 * */
static int gpio_export(int gpio)
{
	int efd;
	char buf[50];
	int gpiofd, ret;

	/* Quick test if it has already been exported */
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	efd = open(buf, O_WRONLY);
	if(efd != -1) {
		close(efd);
		return 0;
	}

	efd = open("/sys/class/gpio/export", O_WRONLY | O_SYNC);

	if(efd != -1) {
		sprintf(buf, "%d", gpio); 
		ret = write(efd, buf, strlen(buf));
		if(ret < 0) {
			perror("Export failed");
			return -2;
		}
		close(efd);
	} else {
		// If we can't open the export file, we probably
		// dont have any gpio permissions
		return -1;
	}
	return 0;
}

void gpio_unexport(int gpio)
{
	int gpiofd, ret;
	char buf[50];
	gpiofd = open("/sys/class/gpio/unexport", O_WRONLY | O_SYNC);
	sprintf(buf, "%d", gpio);
	ret = write(gpiofd, buf, strlen(buf));
	close(gpiofd);
}

int gpio_getfd(int gpio)
{
	char in[3] = {0, 0, 0};
	char buf[50];
	int gpiofd;
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	gpiofd = open(buf, O_RDWR | O_SYNC);
	if(gpiofd < 0) {
		fprintf(stderr, "Failed to open gpio %d value\n", gpio);
		perror("gpio failed");
	}

	return gpiofd;
}

int gpio_read(int gpio)
{
	char in[3] = {0, 0, 0};
	char buf[50];
	int nread, gpiofd;
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	gpiofd = open(buf, O_RDWR | O_SYNC);
	if(gpiofd < 0) {
		fprintf(stderr, "Failed to open gpio %d value\n", gpio);
		perror("gpio failed");
	}

	do {
		nread = read(gpiofd, in, 1);
	} while (nread == 0);
	if(nread == -1){
		perror("GPIO Read failed");
		return -1;
	}
	
	close(gpiofd);
	return atoi(in);
}

int gpio_write(int gpio, int val)
{	
	char buf[50];
	int nread, ret, gpiofd;
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
	gpiofd = open(buf, O_RDWR);
	if(gpiofd > 0) {
		snprintf(buf, 2, "%d", val ? 1 : 0);
		ret = write(gpiofd, buf, 2);
		if(ret < 0) {
			perror("failed to set gpio");
			return 1;
		}

		close(gpiofd);
		if(ret == 2) return 0;
	}
	return 1;
}

int gpio_select(int gpio)
{
	char gpio_irq[64];
	int ret = 0, buf, irqfd;
	fd_set fds;
	FD_ZERO(&fds);

	snprintf(gpio_irq, sizeof(gpio_irq), "/sys/class/gpio/gpio%d/value", gpio);
	irqfd = open(gpio_irq, O_RDONLY, S_IREAD);
	if(irqfd < 1) {
		perror("Couldn't open the value file");
		return -1;
	}

	// Read first since there is always an initial status
	ret = read(irqfd, &buf, sizeof(buf));

	while(1) {
		FD_SET(irqfd, &fds);
		ret = select(irqfd + 1, NULL, NULL, &fds, NULL);
		if(FD_ISSET(irqfd, &fds))
		{
			FD_CLR(irqfd, &fds);  //Remove the filedes from set
			// Clear the junk data in the IRQ file
			ret = read(irqfd, &buf, sizeof(buf));
			return 1;
		}
	}
}
static int pwm_export(int pwm)
{
	int efd;
	char buf[50];
	int gpiofd, ret;

	/* Quick test if it has already been exported */
	sprintf(buf, "/sys/class/pwm/pwmchip8/pwm%d", pwm);
	efd = open(buf, O_WRONLY);
	if(efd != -1) {
		close(efd);
		return 0;
	}

	efd = open("/sys/class/pwm/pwmchip8/export", O_WRONLY | O_SYNC);

	if(efd != -1) {
		sprintf(buf, "%d", pwm); 
		ret = write(efd, buf, strlen(buf));
		if(ret < 0) {
			perror("Export failed");
			return -2;
		}
		close(efd);
	} else {
		// If we can't open the export file, we probably
		// dont have any gpio permissions
		return -1;
	}
	return 0;
}
static int pwm_unexport(int pwm)
{
	int efd;
	char buf[50];
	int gpiofd, ret;

	/* Quick test if it has already been exported */
	sprintf(buf, "/sys/class/pwm/pwmchip8/pwm%d", pwm);
	efd = open(buf, O_WRONLY);
	if(efd != -1) {
		close(efd);
		return 0;
	}

	efd = open("/sys/class/pwm/pwmchip8/unexport", O_WRONLY | O_SYNC);

	if(efd != -1) {
		sprintf(buf, "%d", pwm); 
		ret = write(efd, buf, strlen(buf));
		if(ret < 0) {
			perror("Unexport failed");
			return -2;
		}
		close(efd);
	} else {
		// If we can't open the export file, we probably
		// dont have any gpio permissions
		return -1;
	}
	return 0;
}

int pwm_getfd(int pwm)
{
	char buf[50];
	int pwmfd;
	sprintf(buf, "/sys/class/pwm/pwmchip8/pwm%d/enable", pwm);
	pwmfd = open(buf, O_RDWR | O_SYNC);
	if(pwmfd < 0) {
		fprintf(stderr, "Failed to open gpio %d value\n", pwm);
		perror("gpio failed");
	}
	return pwmfd;
}

static int pwm_enable(int pwm)
{
	char in[3] = {0,0,0};
	char x = '1';
	int pwmfd_enable = pwm_getfd(pwm);
	if(pwmfd_enable < 0)
	{
		return -1;
	}

	sprintf(in, &x, 1);
	if(1 != write(pwmfd_enable, in, 1))
	{
		return -1;
	}
	close(pwmfd_enable);
	return 0;
}

static int pwm_disable(int pwm)
{
	char in[3] = {0,0,0};
	char x = '0';
	int pwmfd_disable = pwm_getfd(pwm);
	if(pwmfd_disable < 0)
	{
		return -1;
	}

	sprintf(in, &x, 1);
	if(1 != write(pwmfd_disable, in, 1))
	{
		return -1;
	}
	close(pwmfd_disable);
	return 0;
}
