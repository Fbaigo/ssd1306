/*!
 *  @file 	   ssd1306.c
 *  @brief     Firmware for SSD1306 OLED controller
 *  @details   Firmware functions for SSD1306, those include:
 *  		   Display initialization.
 *  		   Clear display screen.
 *  		   Clear selected page.
 *  		   Input ASCII text.
 *
 *  @author    Federico Baigorria
 *  @version   0.2.0
 *  @date      03-24-2020
 *  @copyright GNU Public License.
 */

#include "ssd1306.h"

typedef struct {
	uint32_t slave_addr;
	void (*i2c_write)(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes);
	void (*i2c_hw_init)(void);
} ssd1306_devt;

static ssd1306_devt ssd1306_device;

/**
@fn static void ssdOledSendCmd(uint8_t ssdCmd)
@detail Sends only one command byte.

@param ssdCmd command to be sent to the SSD1306 controller.
@return void
*/
static void ssdOledSendCmd(uint8_t ssdCmd){
	uint8_t sddPacket[] = {S1306_CMD_ONLY, ssdCmd};
	oled_i2c_write(sddPacket, sizeof(sddPacket)/sizeof(uint8_t));
}

/**
@fn static void ssdOledSendData(uint8_t ssdPixel)
@detail Sends only one data byte. Depending on the SSD1306's configuration it may be seen
		as a vertical or horizontal 8 pixels group.

@param ssdPixel data to be sent to the SSD1306 controller
@return void
*/
static void ssdOledSendData(uint8_t ssdPixel){
	uint8_t sddPacket[] = {S1306_CMD_END_2RAM, ssdPixel};
	oled_i2c_write(sddPacket, sizeof(sddPacket)/sizeof(uint8_t));
}

/**
@fn static void ssdOledSendAscii(uint8_t ssdFontidx)
@detail Writes an ASCII character in the display. Since this is a full graphic display a text font
		is needed. The ssd1306_font[][] matrix provides it. First index is the ASCII character coordinate
		starting from 0x20 and the second index directs each pixel group (the character's width)

@warning Since the character's width is the number of data bytes to be sent then sddPacket packet structure
		 must be updated whenever a new font is used.
@param ssdPixel data to be sent to the SSD1306 controller
@return void
*/
static void ssdOledSendAscii(uint8_t ssdFontidx){
	uint8_t idx;
	///! ASCII Matrix Offset. First ASCII character 0x20 is the first table element
	uint8_t ssdOffset = (ssdFontidx - 0x20);
	///! Transmit buffer for ASCII character (data)
	uint8_t sddPacket[SSD1306_FONT_CHAR_WIDTH + 1];
	///! Send data start command
	sddPacket[0] = S1306_CMD_END_2RAM;

	for(idx = 0; idx < SSD1306_FONT_CHAR_WIDTH; idx++){
		sddPacket[idx + 1] = ssd1306_font[ssdOffset][idx];
	}

	oled_i2c_write(sddPacket, sizeof(sddPacket)/sizeof(uint8_t));
}

/** Public functions
 */

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

/**
@fn void ssdOledInit(void)
@detail SSD1306 OLED display Init. Whole command stream configuration is written in ssdInitDatagram[]
		Display is configured for:
		*** PENDING ***

@param ssdPixel data to be sent to the SSD1306 controller
@return void
*/
void ssdOledInit(void){
	oled_i2c_write(ssdInitDatagram, sizeof(ssdInitDatagram)/sizeof(uint8_t));
}

/**
@fn void ssdOledSetPageOrigin(uint8_t ssdPage)
@detail Set a new start page address at ssdPage | START_PAGE_VAL. The starting column address is
		set by default at the very beginning (0). It may change in the future.
		Column address is compound by two nibbles (first low then high) corresponding to 0 - 127.
		First nibble is 0x00 | low starting column address.
		Second nibble is 0x10 | high starting column address.

@warning Use this only when the display has been configured as page mapping mode.

@return void
*/
void ssdOledSetPageOffset(uint8_t ssdPage, uint8_t ssdColumnAddr){
	///! Page index limit
	ssdPage %= S1306_MAX_PAGE;

	uint8_t sddPacket[] = {	0x80,START_PAGE_VAL | ssdPage,				///! Set new start page
							0x80,0x00 | (ssdColumnAddr & 0x0F),			///! Set starting column low nibble
							0x00,0x10 | ((ssdColumnAddr & 0xF0) >> 4) 	///! Set starting column high nibble. End transmission.
							};
	oled_i2c_write(sddPacket, sizeof(sddPacket)/sizeof(uint8_t));
}

/**
@fn void ssdOledSetPageOrigin(uint8_t ssdPage)
@detail Set a new start page address at ssdPage | START_PAGE_VAL. The starting column address is
		set by default at the very beginning (0). It may change in the future.
		Column address is compound by two nibbles (first low then high) corresponding to 0 - 127.
		First nibble is 0x00 | low starting column address.
		Second nibble is 0x10 | high starting column address.

@warning Use this only when the display has been configured as page mapping mode.

@return void
*/
void ssdOledSetPageOrigin(uint8_t ssdPage){
	ssdOledSetPageOffset(ssdPage, 0x00);
}

/**
@fn void ssdOledCleanPage(uint8_t ssdPageidx)
@detail Clears a whole page (when configured as Page mapping).

@warning Use this only when the display has been configured as page mapping mode.

@param ssdPageidx Page index to be clear from 0 to S1306_MAX_PAGE - 1
@return void
*/
void ssdOledCleanPage(uint8_t ssdPage){
	///! SEG0 to SEG127
	uint8_t ssdSegidx;
	///! Set starting page address
	ssdOledSetPageOrigin(ssdPage);
	///! Clear all horizontal pixels
	for(ssdSegidx = 0; ssdSegidx < S1306_MAX_SEGMENT; ssdSegidx++){
		ssdOledSendData(0x00);
	}
}

/**
@fn void ssdOledCleanScreen(void)
@detail Clears whole display's screen.

@warning Use this only when the display has been configured as page mapping mode.

@return void
*/

void ssdOledCleanScreen(void){
	///! Page 0 to Page S1306_MAX_PAGE - 1
	uint8_t ssdPage;
	///! Clear all horizontal pixels
	for(ssdPage = 0; ssdPage < S1306_MAX_PAGE; ssdPage++){
		ssdOledCleanPage(ssdPage);
	}
	///! Set starting page address
	ssdOledSetPageOrigin(0);
}

/**
@fn void ssdOledSetText(uint8_t *ssdTxt, uint8_t enPageSwitch)
@detail Writes text to the OLED display.

@warning Use this only when the display has been configured as page mapping mode.
		 No null terminating character control is made, user is advised to proper terminate any character string.

@param ssdTxt Pointer to output ASCII text
@param enPageSwitch Page switch enable flag. After writing the maximum allowed number of characters per line it will
		automatically switch to the next page.
@return void
*/

void ssdOledSetText(uint8_t *ssdTxt, uint8_t enPageSwitch){
	uint8_t ssdPage = 0;		///! Page 0 to Page S1306_MAX_PAGE - 1
	uint8_t ssdTextcnt = 0;		///! ASCII character counter (per display line)

	while(*ssdTxt){
		///! Send the formatted ASCII character
		ssdOledSendAscii(*ssdTxt);
		ssdTxt++;

		if(enPageSwitch){
			ssdTextcnt++;
			if(ssdTextcnt > SSD1306_FONT_MAXCHARS_LINE){
				///! Reset ASCII characters counter and set next page
				ssdTextcnt = 0;
				ssdPage++;
				ssdOledSetPageOrigin(ssdPage);
				///! Page index limit
				ssdPage %= S1306_MAX_PAGE;
			}
		}
	}
}

/**
@fn void ssdOnResetMsg(void)
@detail Splash message on screen during startup.

@return void
*/

/*void ssdOnResetMsg(uint8_t *appVersion){
	ssdOledSetPageOrigin(1);
	ssdOledSetText("   TDS meter    ", 0);
	ssdOledSetPageOrigin(2);
	ssdOledSetText("  Firmware v", 0);
	ssdOledSetText(appVersion, 0);
}*/

/**
@fn void ssdOnResetMsg(void)
@detail Splash message on screen during startup.

@return void
*/
void ssdOledBarGraph(void){
	uint8_t idx;

	///! Clear page and set new origin
	ssdOledCleanPage(0x03);
	ssdOledSetPageOffset(0x03, S1306_X_START_OFFS);
	///! Bar graph
	for(idx = 0; idx < S1306_HOR_LIMIT - 1; idx++){
		if(idx == 0 || idx == (S1306_HOR_LIMIT - 2)){
			ssdOledSendData(0xFF);
		} else {
			ssdOledSendData(0x81);
		}
	}
}

/**
@fn void ssdOnResetMsg(void)
@detail Splash message on screen during startup.

@return void
*/
void ssdOledUpdateBar(uint8_t ssdPercent){
	uint8_t idx;
	uint8_t idxMax = ((float)(S1306_HOR_LIMIT - 1)) * (ssdPercent / 100.0);

	///! Set origin
	ssdOledSetPageOffset(0x03, S1306_X_START_OFFS);
	///! Low threshold
	if(idxMax == 0)
		idxMax = 1;
	///! High threshold
	if(idxMax > S1306_HOR_LIMIT - 2){
		idxMax = S1306_HOR_LIMIT - 1;
	}
	///! Bar graph update: Filled
	for(idx = 0; idx < idxMax; idx++){
		ssdOledSendData(0xFF);
	}
	///! Bar graph update: Empty
	for(idx = idxMax; idx < S1306_HOR_LIMIT - 2; idx++){
		ssdOledSendData(0x81);
	}
}

void ssdOledSetSymbol(uint8_t ssdSymbolIdx){
	uint8_t idx;

	///! Transmit buffer for symbol (data)
	uint8_t sddPacket[SSD1306_FONT_SYMBOL_WIDTH + 1];
	///! Send data start command
	sddPacket[0] = S1306_CMD_END_2RAM;

	for(idx = 0; idx < SSD1306_FONT_SYMBOL_WIDTH; idx++){
		sddPacket[idx + 1] = ssd1306_symbols[ssdSymbolIdx][idx];
	}

	oled_i2c_write(sddPacket, sizeof(sddPacket)/sizeof(uint8_t));
}
