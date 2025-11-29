#include "ssd_hw_iface.h"

#define USING_ESP8266	1
#define USING_LPC1769	0

/**
@fn static void sddOledWrite(uint8_t *ssdData, uint8_t ssdNbytes)
@detail Refer to SSD1306 reference manual serial interface communication chapter for details.
		As default display's slave address is 0x3C and it's defined in S1306_SLAVE_ADDR.
		Standard I2C protocol communication.

@warning This function is hardware dependent and should be configured accordingly.
@param ssdData command/data stream to be sent.
@param ssdNbytes number of bytes to be sent.
@return void
*/

#if
static void lp1769_i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes){
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

static void esp8266_i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN));
	ESP_ERROR_CHECK(i2c_master_write(cmd, data, nbytes, ACK_CHECK_EN));
	ESP_ERROR_CHECK(i2c_master_stop(cmd));

	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);
}

/**
@fn void ssdI2CHardwareInit(void)
@detail SSD1306 Hardware init configuration for I2C communication.

@warning This function is hardware dependent and must be configured by the user.
@return void
*/
void ssdI2CHardwareInit(void){
	//Board_I2C_Init(I2C0);
	//Chip_I2C_SetClockRate(I2C0, 1000000);
	//Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandlerPolling);

	int i2c_master_port = I2C_NUM_0;
    i2c_config_t conf;

    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = 1;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = 1;
    conf.clk_stretch_tick = 3000; // 300 ticks, Clock stretch is about 210us, you can make changes according to the actual situation.
	
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    //return ESP_OK;
}