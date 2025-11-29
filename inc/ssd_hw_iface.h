/*!
 *  @file 	   ssd_hw_iface.h
 *  @brief     SSD1306 128x64 LCD library.
 *  @details   Hardware dependencies needed for I2C communication. This header file is used
 *  		   by the ssd1306 device initialization and communication
 *
 *  @warning   Application dependent. User must edit this as needed
 *  @author    Federico Baigorria
 *  @date      11-29-2025
 *  @copyright GNU Public License.
 */

#ifndef __SSD_HW_IFACE_H__
#define __SSD_HW_IFACE_H__

#include <stdint.h>

#define USING_ESP8266X	1
#define USING_LPC1769	0

#if USING_LPC1769

#include "board.h"
#include "i2c_17xx_40xx.h"

void lp1769_i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes);
void lpc1769_i2c_init(void);

#endif

#if USING_ESP8266X

#include "driver/i2c.h"
#include "driver/gpio.h"
#include "FreeRTOS.h"

#define I2C_SCL_IO      GPIO_NUM_5  ///! gpio number for I2C master clock
#define I2C_SDA_IO      GPIO_NUM_4  ///! gpio number for I2C master data

#define ACK_EN          0x1         ///! I2C master will check ack from slave*/
#define ACK_DIS         0x0         ///! I2C master will not check ack from slave */

void esp8266_i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes);
void esp8266_i2c_init(void);

#endif

#endif /* __SSD_HW_IFACE_H__ */
