/*!
 *  @file 	   ssd1306.h
 *  @brief     SSD1306 128x32 / 128x64 OLED library.
 *  @details   User can set different parameters detailed below.
 *
 *			   Header files:
 *			   qc12864b.h 	Functionality configuration for QC12864B LCD control
 *			   qcBmp.h	  	Static bitmap image generation. Includes an static array for user edition
 *			   qcHardware.h All hardware related definitions, macro and header files needed for
 *			   				SPI control and communication to QC12864B LCD.
 *			   qcLCD.h		Application header file for QC12864B usage.
 *	@warning   Needs to be edited by the user accordingly
 *  @author    Federico Baigorria
 *  @date      11-29-2025
 *  @copyright GNU Public License.
 */

#ifndef __SSD1306_INC_SSD1306_H__
#define __SSD1306_INC_SSD1306_H__

///! Self header files
#include <stdint.h>

#define USING_128X64_OLED					1			///! OLED Display maximum height (in pages). For a 128x32 is set to 4 and for 128x64 it's 8

#if USING_128X64_OLED
	typedef enum {
		S1306_PAGE0,
		S1306_PAGE1,
		S1306_PAGE2,
		S1306_PAGE3,
		S1306_PAGE4,
		S1306_PAGE5,
		S1306_PAGE6,
		S1306_PAGE7,
		S1306_MAX_PAGE
	} ssd1306_pages;
#else
	typedef enum {
		S1306_PAGE0,
		S1306_PAGE1,
		S1306_PAGE2,
		S1306_PAGE3,
		S1306_MAX_PAGE
	} ssd1306_pages;
#endif

///! Page offseting
#define	S1306_X_START_OFFS					16			///! X axis offset for page offset setting. User editable
#define	S1306_X_END_OFFS					16			///! X axis offset for page offset setting. User editable

typedef void (*i2c_write_fn)(uint32_t const, uint8_t*, uint32_t);
typedef void (*i2c_hw_init_fn)(void);

///! Default slave address for SSD1306 Oled display
#define	S1306_SLAVE_ADDR			0x3C		///! 7 bits long address 011110(SAO). SAO bit may be 0 o 1. Default val 0x3C

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

void ssd1306_device_startup(uint32_t const slave_addr, i2c_write_fn writer_fn, i2c_hw_init_fn hw_init_fn);
void ssd1306_print_ascii(uint8_t ascii_char);
void ssd1306_set_page_offset(ssd1306_pages page, uint8_t column_addr);
void ssd1306_page_clear(ssd1306_pages page);
void ssd1306_display_clear(void);
void ssd1306_print_text(uint8_t *text, uint8_t en_page_change);

//void ssdOledSetSymbol(uint8_t);
//void ssdOnResetMsg(uint8_t *);
//void ssdOledBarGraph(void);
//void ssdOledUpdateBar(uint8_t);

#endif /* __SSD1306_INC_SSD1306_H__ */
