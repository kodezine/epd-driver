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


#if !defined(EPD_IO_H)
#define EPD_IO_H 1

/** GPIO Output Pins *********************************************************/
#define panel_on_pin                101
#define border_pin                  45
#define discharge_pin               97
#define reset_pin                   85

/** GPIO Input Pins **********************************************************/
#define busy_pin                    79

/** PWM Pin ******************************************************************/
#define pwmb_pin                    42
#define pwm_b                       0
/** VIOLA 1.1 to Colibri VF SODIMM values ************************************/
#define VIOLA_12	                gpio101
#define VIOLA_13	                gpio97
#define VIOLA_14	                gpio85
#define VIOLA_15	                gpio79
#define VIOLA_16	                gpio45


/** */
#ifdef __ARM_EABI__    // path when executing on ARM target
    #define PATH_GPIO_VALUE(x)		"/sys/class/gpio/##x/value"
    #define PATH_GPIO_DIRECTION(x)	"/sys/class/gpio/##x/direction"
#else                  // path when executing on x86 host
    #define PATH_GPIO_VALUE(x)		"/tmp/class/gpio/##x/value"
    #define PATH_GPIO_DIRECTION(x)	"/tmp/class/gpio/##x/direction"
#endif

#define SPI_DEVICE                  "/dev/spidev1.0"
#define SPI_BPS                     8000000

#define I2C_DEVICE	                "/dev/i2c-0"

#define PWM_B				        "/sys/class/pwm/pwmchip8/pwm0/"

#endif
