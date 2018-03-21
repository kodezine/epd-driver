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

/** GPIO Output Pins                        VIOLA X9    SODIMM      CPU PIN */
#define panel_on_pin                79  /*  36          38          PTD0    */
#define border_pin                  80  /*  37          36          PTD1    */
#define discharge_pin               81  /*  38          34          PTD2    */
#define reset_pin                   82  /*  39          32          PTD3    */

/** GPIO Input Pins **********************************************************/
#define busy_pin                    26  /*  34          21          PTB4    */

/** PWM Pin ******************************************************************/
#define pwmb_pin                    42
#define pwm_b                       0


#define SPI_DEVICE                  "/dev/spidev1.0"
#define SPI_BPS                     8000000

#define I2C_DEVICE	                "/dev/i2c-0"

#define PWM_B				        "/sys/class/pwm/pwmchip8/pwm0/"

#endif
