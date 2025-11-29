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

#define USING_ESP8266X	1
#define USING_LPC1769	0

#if USING_LPC1769

#include "board.h"
#include "i2c_17xx_40xx.h"

#endif

#if USING_ESP8266X

#include "driver/i2c.h"
#include "gpio.h"
#include "FreeRTOS.h"

#define I2C_SCL_IO      GPIO_NUM_5  ///! gpio number for I2C master clock
#define I2C_SDA_IO      GPIO_NUM_4  ///! gpio number for I2C master data

#define ACK_EN          0x1         ///! I2C master will check ack from slave*/
#define ACK_DIS         0x0         ///! I2C master will not check ack from slave */


#endif

#endif /* __SSD_HW_IFACE_H__ */
