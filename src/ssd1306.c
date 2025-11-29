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
#define SSD1306_STARTED		1

typedef void (*i2c_write_fn)(uint32_t const, uint8_t*, uint32_t);
typedef void (*i2c_hw_init_fn)(void);

typedef struct {
	uint32_t slave_addr;
	uint32_t is_started;
	i2c_write_fn i2c_write;
	i2c_hw_init_fn i2c_hw_init;
} ssd1306_devt;

static ssd1306_devt ssd1306_device;

static void ssd1306_i2c_write(uint8_t *data, uint32_t bytes){
	ssd1306_device.i2c_write(
		ssd1306_device.slave_addr,
		data, 
		bytes
	);
}

/**
* @fn void ssd1306_device_startup(uint32_t const slave_addr, i2c_write_fn writer_fn, i2c_hw_init_fn hw_init_fn)
* @brief SSD1306 device initialization
* @details
* The SSD1306 device abstraction requires a slave address, an I2C writer function and an I2C hardware init function
* This function will send a series of default commands for initialization and these can be found in the ssd1306.h header file (ssd1306_init_cfg)
* @warning
* This is the first function to be called
* @return void
*/

void ssd1306_device_startup(uint32_t const slave_addr, i2c_write_fn writer_fn, i2c_hw_init_fn hw_init_fn){
	ssd1306_device.slave_addr = slave_addr;
	ssd1306_device.i2c_write = writer_fn;
	ssd1306_device.i2c_hw_init = hw_init_fn;
	ssd1306_device.is_started = !SSD1306_STARTED;

	ssd1306_i2c_write(ssd1306_init_cfg, sizeof(ssd1306_init_cfg)/sizeof(uint8_t));

	///! TODO: Verify
	ssd1306_device.is_started = SSD1306_STARTED;
}

/**
* @fn static void ssd1306_print_ascii(uint8_t ascii_char)
* @brief Writes an ASCII character in the display
* @details 
* Since this is a full graphic display a text font is needed (see ssd1306_font[X][Y] in ssd_fonts.h)
* Index [X] is the ASCII character starting from 0x20 
* Index [Y] prints the pixel group (character's width)
* 
* The character's width sets the number of data bytes to be sent written in the ascii_packet packet array
* @param ascii_char ascii character to be print
* @return void
*/

void ssd1306_print_ascii(uint8_t ascii_char){
	uint8_t idx;
	uint8_t offset = (ascii_char - 0x20);
	uint8_t ascii_packet[SSD1306_FONT_CHAR_WIDTH + 1];

	ascii_packet[0] = S1306_CMD_END_2RAM;

	for(idx = 0; idx < SSD1306_FONT_CHAR_WIDTH; idx++){
		ascii_packet[idx + 1] = ssd1306_font[offset][idx];
	}

	ssd1306_i2c_write(ascii_packet, sizeof(ascii_packet)/sizeof(uint8_t));
}

/**
* @fn void ssd1306_set_page_offset(uint8_t page, uint8_t column_addr)
* @details 
* Set a new start page address at ssdPage | START_PAGE_VAL. The starting column address is
* set by default at the very beginning (0). It may change in the future.
* Column address is compound by two nibbles (first low then high) corresponding to 0 - 127.
* First nibble is 0x00 | low starting column address.
* Second nibble is 0x10 | high starting column address.
* 
* @warning Use this only when the display has been configured as page mapping mode.
* 
* @return void
*/
void ssd1306_set_page_offset(uint8_t page, uint8_t column_addr){
	page %= S1306_MAX_PAGE;

	uint8_t data[] = {	
		S1306_START_PAGE_VAL | page,							///! Set new start page
		S1306_PAGE_LOWER_COL | (column_addr & 0x0F),			///! Set starting column low nibble
		S1306_PAGE_HIGHER_COL | ((column_addr & 0xF0) >> 4), 	///! Set starting column high nibble. End transmission.
		S1306_NOP
	};
	
	ssd1306_i2c_write(data, sizeof(data)/sizeof(uint8_t));
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
		ssd1306_print_ascii(*ssdTxt);
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
