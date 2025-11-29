/*!
 *  @file 	   ssdHardware.h
 *  @brief     SSD1306 128x64 LCD library.
 *  @details   Hardware dependencies needed for I2C communication. This header file can be used
 *  		   in application defined hardware initialization for I2C.
 *
 *  @warning   Needs to be edited by the user accordingly.
 *  @author    Federico Baigorria
 *  @date      03-24-2020
 *  @copyright GNU Public License.
 */

#ifndef __SSD1306_INC_SSDHARDWARE_H__
#define __SSD1306_INC_SSDHARDWARE_H__

/* Include here all the board and hardware header files needed for SPI peripheral
 * control and communication
 */

///! For LPC1769 
//#include "board.h"
//#include "i2c_17xx_40xx.h"

///! For ESP8266x
#include "driver/i2c.h"
#include "FreeRTOS.h"

///! Hardware macro definitions
//#define SSD_I2C		I2C0			///! I2C ID Port to be used

#define I2C_EXAMPLE_MASTER_SCL_IO           5                /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO           4               /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_NUM              I2C_NUM_0        /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0                /*!< I2C master do not need buffer */

#define ACK_CHECK_EN                        0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                       0x0              /*!< I2C master will not check ack from slave */
#define WRITE_BIT                           I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                            I2C_MASTER_READ  /*!< I2C master read */

#endif /* __SSD1306_INC_SSDHARDWARE_H__ */
