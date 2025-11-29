#include "ssd_hw_iface.h"

/**
 * @file		ssd_hw_iface.c
 * @brief		I2C hardware interface for different target microcontrollers or SoC
 * @details   
 * The SSD1306 HAL requires two functions, an I2C write method and an I2C
 * hardware initialization.
 * 
 * Refer to SSD1306 reference manual serial interface communication chapter for details
 * Default display's slave address is 0x3C (S1306_SLAVE_ADDR in ssd1306.h header file)
 * Requires a standard I2C protocol communication
 *
 * Use the following write function prototype 
 * void (*i2c_write)(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes);
 * 
 * Use the following hardware init function prototype
 * void (*i2c_hw_init)(void);
 * @author    Federico Baigorria
 * @version   0.2.0
 * @date      11-29-2025
 * @copyright GNU Public License.
*/

#if USING_LPC1769

/**
* @fn void lp1769_i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes)
* @brief I2C hardware interface for LPC1769
* @details 
* I2C firmware implementation for the LPC1769 using the default I2C port
* @warning This source file is hardware dependent and must be edited for the target microcontroller, SoC, etc. 
* @param slave_addr slave address
* @param data command/data stream to be sent
* @param nbytes number of bytes to be sent
* @return void
*/

void lp1769_i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes){
	I2C_XFER_T sddBuffer;

	///! Preparing datagram for continuos command transmision
	sddBuffer.rxBuff = 0;										///! No reception
	sddBuffer.rxSz = 0;											///! No reception
	sddBuffer.slaveAddr = S1306_SLAVE_ADDR;
	sddBuffer.status = 0;
	sddBuffer.txBuff = ssdData;
	sddBuffer.txSz = ssdNbytes;

	///! Transmission init
	Chip_I2C_MasterTransfer(SSD_I2C, &sddBuffer);
}

/**
* @fn void esp8266_i2c_init(void)
* @brief I2C hardware interface for ESP8266X
* @details 
* I2C hardware initialization. 
* Internal pull-up resistors are not required since the SSD1306 board has soldered pull-up resistors (always check the board)
* @warning This function is hardware dependent and must be created for the target microcontroller, SoC, etc. 
* @return void
*/

void lpc1769_i2c_init(void){
	Board_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 1000000);
	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandlerPolling);
}

#endif

#if USING_ESP8266X

/**
* @fn void esp8266_i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes)
* @brief I2C hardware interface for ESP8266X
* @details 
* I2C firmware implementation for the ESP8266X using the default I2C0 port
* @warning This function is hardware dependent and must be created for the target microcontroller, SoC, etc. 
* @param slave_addr slave address
* @param data command/data stream to be sent
* @param nbytes number of bytes to be sent
* @return void
*/

void esp8266_i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_WRITE, ACK_EN));
	ESP_ERROR_CHECK(i2c_master_write(cmd, data, nbytes, ACK_EN));
	ESP_ERROR_CHECK(i2c_master_stop(cmd));

	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);
}

/**
* @fn void esp8266_i2c_init(void)
* @brief I2C hardware interface for ESP8266X
* @details 
* I2C hardware initialization. 
* Using GPIO pin 4 as SDA and GPIO pin 5 as SCL
* Internal pull-up resistors are not required since the SSD1306 board has soldered pull-up resistors (always check the board)
* @warning This function is hardware dependent and must be created for the target microcontroller, SoC, etc. 
* @return void
*/

void esp8266_i2c_init(void){
	int i2c_master_port = I2C_NUM_0;
	i2c_config_t conf;

	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = I2C_SDA_IO;
	conf.sda_pullup_en = 0;
	conf.scl_io_num = I2C_SCL_IO;
	conf.scl_pullup_en = 0;
	conf.clk_stretch_tick = 300;

	ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
	ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
}

#endif