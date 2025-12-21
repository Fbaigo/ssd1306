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
#include "ssd_fonts.h"

///! Commands sent continuously.Packet: (Control byte - Command byte)
#define S1306_DISP_OFF				0x80,0xAE	///! Fundamental command. Set display off (sleep mode)
#define S1306_DISP_CLK_DIV_RATIO	0x80,0xD5 	///! Timing & Driving scheme. This is a double command (0xD5 Command + Data)
#define S1306_MULT_RATIO			0x80,0xA8	///! Hardware configuration. This is a double command (0xA8 Command + Data)
#define S1306_DISP_OFFSET 			0x80,0xD3	///! Hardware configuration. This is a double command (0xD3 Command + Data)
#define S1306_CHARGE_PUMP 			0x80,0x8D	///! Charge pump (special). This is a double command (0x8D Command + Data)
#define S1306_START_LINE_ADDR 		0x80,0x40	///! Hardware configuration. Set display RAM start line
#define S1306_START_PAGE_VAL		0x80,0xB0
#define S1306_MEM_ADDR_MODE 		0x80,0x20	///! Addressing setting. This is a double command (0x20 Command + Data)
#define S1306_PAGE_LOWER_COL 		0x80,0x00	///! Addressing setting. Set lower nibble of column start address. Only page addressing mode
#define S1306_PAGE_HIGHER_COL 		0x80,0x10	///! Addressing setting. Set higher nibble of column start address. Only page addressing mode
#define S1306_START_PAGE_ADDR		0x80,0xB0	///! Addressing setting. Set start page address at Page 0. Only page addressing mode
#define S1306_NORMAL_MODE 			0x80,0xA6	///! Fundamental command. Set display in normal mode (positive logic)
#define S1306_DISP_ALL_ON_DIS 		0x80,0xA4 	///! Fundamental command. Disable entire display ON, depends on RAM content
#define S1306_SEGMNT_NO_REMAP 		0x80,0xA0	///! Hardware configuration. Column address 127 is mapped to SEG0
#define S1306_COM_SCAN_DIR 			0x80,0xC0	///! Hardware configuration. Normal mode
#define S1306_SET_COM_PINS 			0x80,0xDA	///! Hardware configuration. This is a double command (0xDA Command + Data)
#define S1306_CONTRAST_CTRL 		0x80,0x81	///! Fundamental command. This is a double command (0x81 Command + Data)
#define S1306_VCOM_DESELECT 		0x80,0xDB	///! Timing & Driving scheme. This is a double command (0xDB Command + Data)
#define S1306_PRECHARGE_PERIOD 		0x80,0xD9	///! Timing & Driving scheme. This is a double command (0xD9 Command + Data)
#define	S1306_DISP_ON				0x00,0xAF	///! Fundamental command. Set display on (in normal mode), last command sent (0x00)
#define S1306_NOP					0x00,0xE3	///! No operation command

///! Append for data packets (Continuosly sending data bytes)
#define	S1306_CMD_END_2RAM			0x40		///! End command transmision. Following bytes are data bytes to be stored at GDDRA
#define S1306_CMD_ONLY				0x00		///! End command transmision. Following bytes are command bytes

///! Separated command values

///! Command data value
#define S1306_DISP_CLK_RATIO_VAL	0x80,0x80	///! Follows S1306_DISP_CLK_DIV_RATIO command. Formal ratio value
#define	S1306_MULT_RATIO_VAL		0x80,0x3F	///! Follows S1306_MULT_RATIO command. Formal mux ratio
#define S1306_DISP_OFFSET_VAL		0x80,0x00	///! Follows S1306_DISP_OFFSET command. Formal vertical offset
#define S1306_CHARGE_PUMP_VAL		0x80,0x14	///! Follows S1306_CHARGE_PUMP command. Enables the charge pump (after disp on)
#define	S1306_SET_COM_PINS_VAL		0x80,0x02	///! Follows S1306_SET_COM_PINS command. Disable COM left/right remap. Alt com. config
#define S1306_CONTRAST_CTRL_VAL		0x80,0x7F	///! Follows S1306_CONTRAST_CTRL command. Formal contrast value, high values mean higher contrast
#define S1306_VCOM_DESELECT_VAL		0x80,0x20	///! Follows S1306_VCOM_DESELECT command. Vcomh deselect level
#define S1306_MEM_ADDR_MODE_VAL		0x80,0x02	///! Follows S1306_MEM_ADDR_MODE command. Page addressing mode
#define S1306_PRECHARGE_PERIOD_VAL	0x80,0x22	///! Follows S1306_PRECHARGE_PERIOD command. Phase periods.

#define SSD1306_STARTED				1

static void ssd1306_i2c_write(uint8_t *data, uint32_t bytes);
static void ssd1306_gddr_write(uint8_t pixel);

typedef struct {
	uint32_t slave_addr;
	uint32_t is_started;
	i2c_write_fn i2c_write;
	i2c_hw_init_fn i2c_hw_init;
} ssd1306_devt;

static ssd1306_devt ssd1306_device;

///! SSD1306 Init datagram transmission buffer. Configure according to users need
static uint8_t ssd1306_init_cfg[] = {
								S1306_DISP_OFF,
								S1306_DISP_CLK_DIV_RATIO,S1306_DISP_CLK_RATIO_VAL,
								S1306_MULT_RATIO,S1306_MULT_RATIO_VAL,
								S1306_DISP_OFFSET,S1306_DISP_OFFSET_VAL,
								S1306_CHARGE_PUMP,S1306_CHARGE_PUMP_VAL,
								S1306_START_LINE_ADDR,
								S1306_MEM_ADDR_MODE,S1306_MEM_ADDR_MODE_VAL,
								S1306_START_PAGE_ADDR,
								S1306_PAGE_LOWER_COL,
								S1306_PAGE_HIGHER_COL,
								S1306_NORMAL_MODE,
								S1306_DISP_ALL_ON_DIS,
								S1306_SEGMNT_NO_REMAP,
								S1306_COM_SCAN_DIR,
								S1306_SET_COM_PINS,S1306_SET_COM_PINS_VAL,
								S1306_CONTRAST_CTRL,S1306_CONTRAST_CTRL_VAL,
								S1306_VCOM_DESELECT,S1306_VCOM_DESELECT_VAL,
								S1306_DISP_ON};	///! Last command

static void ssd1306_i2c_write(uint8_t *data, uint32_t bytes){
	ssd1306_device.i2c_write(
		ssd1306_device.slave_addr,
		data, 
		bytes
	);
}

static void ssd1306_gddr_write(uint8_t pixel){
	uint8_t data[] = {S1306_CMD_END_2RAM, pixel};
	ssd1306_i2c_write(data, sizeof(data)/sizeof(uint8_t));
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

	ssd1306_device.i2c_hw_init();

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
* @brief Sets the cursor at a new page address and a given X offset (column)
* @details 
* Column address is compound by two nibbles (first low then high) corresponding to 0 - 127.
* First nibble is 0x00 | low starting column address. (S1306_PAGE_LOWER_COL)
* Second nibble is 0x10 | high starting column address. (S1306_PAGE_HIGHER_COL)
* 
* @warning 
* Use this function only when the SSD1306 has been set to page mapping mode
*
* @param page page number
* @param column_addr offset in X axis
* @return void
*/
void ssd1306_set_page_offset(ssd1306_pages_t page, uint8_t column_addr){
	uint8_t data[] = {	
		S1306_START_PAGE_VAL | page,							///! Set new start page
		S1306_PAGE_LOWER_COL | (column_addr & 0x0F),			///! Set starting column low nibble
		S1306_PAGE_HIGHER_COL | ((column_addr & 0xF0) >> 4), 	///! Set starting column high nibble. End transmission.
		S1306_NOP
	};
	
	ssd1306_i2c_write(data, sizeof(data)/sizeof(uint8_t));
}

/**
* @fn void ssd1306_page_clear(ssd1306_pages page)
* @brief Clears a single page (all segments)

* @warning 
* Use this function only when the SSD1306 has been set to page mapping mode
* @param page Page index to be clear from 0 to S1306_MAX_PAGE - 1
* @return void
*/
void ssd1306_page_clear(ssd1306_pages_t page){
	///! SEG0 to SEG127
	uint8_t seg_idx;
	ssd1306_set_page_offset(page, 0x00);

	for(seg_idx = 0; seg_idx < S1306_MAX_SEGMENT; seg_idx++){
		ssd1306_gddr_write(0x00);
	}
}

/**
* @fn void ssd1306_display_clear(void)
* @brief Clears the entire screen
* 
* @warning 
* Use this function only when the SSD1306 has been set to page mapping mode
* 
* @return void
*/
void ssd1306_display_clear(void){
	uint8_t page;

	for(page = 0; page < S1306_MAX_PAGE; page++){
		ssd1306_page_clear(page);
	}
	ssd1306_set_page_offset(S1306_PAGE0, 0);
}

/**
* @fn void ssd1306_print_text(uint8_t *text, uint8_t en_page_change)
* @brief Prints a message
* @details
* Writes a given message to the OLED display.
* If enabled, after writing the maximum allowed number of characters per line it will
* automatically switch to the next page.
* 
* @warning
* Use this function only when the SSD1306 has been set to page mapping mode
* String text must be NULL terminated
* 
* @param text String to be printed
* @param en_page_change Page change enable flag 
* @return void
*/

void ssd1306_print_text(uint8_t *text, uint8_t en_page_change){
	uint8_t page = 0;
	uint8_t char_counter = 0;

	while(*text){
		if(*text == '\n'){
			page++; page %= S1306_MAX_PAGE;
			ssd1306_set_page_offset(page, 0x00);
			ssd1306_page_clear(page);
			text++;
			continue;
		}

		ssd1306_print_ascii(*text);
		text++;
		
		if(en_page_change && char_counter > SSD1306_FONT_MAXCHARS_LINE){
			char_counter = 0;
			page++; page %= S1306_MAX_PAGE;
			ssd1306_set_page_offset(page, 0x00);
			ssd1306_page_clear(page);
		}
		else {
			char_counter++;
		}
	}
}

/**
 * Graphics
 */

/**
* @fn void ssd1306_static_horizontal_bar(uint8_t at_page, uint32_t x_start_loc, uint32_t x_bar_len, double percent_fill)
* @brief Prints a horizontal bar filled at a given percentage
* @details
* User can set a starting X offset and the bar's horizontal length. It will be filled
* to the selected percentage.
*
* @param at_page Page to be printed (equivalent to a row of 8 bits of height)
* @param x_start_loc Horizontal X offset to start printing the bar
* @param x_bar_len Bar's horizontal length
* @param percent_fill Bar's initial percentage
* @return void
*/

void ssd1306_static_horizontal_bar(ssd1306_pages_t at_page, uint32_t x_start_loc, uint32_t x_bar_len, double percent_fill){
	uint32_t x_loc, x_until_fill_loc;

	at_page = at_page > S1306_MAX_PAGE ? S1306_MAX_PAGE-1 : at_page;
	x_start_loc = x_start_loc > S1306_MAX_SEGMENT ? 0 : x_start_loc;
	percent_fill = percent_fill > 100 ? 50 : percent_fill;
	x_bar_len = x_bar_len > S1306_MAX_SEGMENT ? S1306_MAX_SEGMENT - 2 : x_bar_len;
	x_until_fill_loc = (x_bar_len / 100.0) * percent_fill;

	ssd1306_page_clear(at_page);
	ssd1306_set_page_offset(at_page, x_start_loc);

	///! Bar plot
	for(x_loc = 0; x_loc < x_bar_len - 1; x_loc++){
		if(x_loc == 0 ||
			x_loc == (x_bar_len - 2) ||
			(x_loc > 0 && x_loc < x_until_fill_loc))
		{
			ssd1306_gddr_write(0xFF);
		}
		else {
			ssd1306_gddr_write(0x81);
		}
	}
}

#if SSD1306_SYMBOLS_FONT

/**
* @fn ssd1306_print_symbol(ssd1306_symbols_t ssd_symbol, ssd1306_pages_t at_page, uint32_t x_loc)
* @brief Prints a symbol from ssd1306_symbols symbols array
* @details
* Select a symbol or add new ones in the ssd1306_symbols symbols array
*
* @param ssd_symbol Symbol to be printed
* @param at_page Page to be printed (equivalent to a row of 8 bits of height)
* @param x_loc Horizontal X location from printing the symbol
* @return void
*/

void ssd1306_print_symbol(ssd1306_symbols_t ssd_symbol, ssd1306_pages_t at_page, uint32_t x_loc){
	uint8_t idx;
	ssd_symbol = ssd_symbol > SYM_TOTAL ? SYM_NOSYMBOL : ssd_symbol;

	ssd1306_set_page_offset(at_page, x_loc);

	for(idx = 0; idx < SSD1306_FONT_SYMBOL_WIDTH; idx++){
		ssd1306_gddr_write(ssd1306_symbols[ssd_symbol][idx]);
	}
}

#endif
