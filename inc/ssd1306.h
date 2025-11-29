/*!
 *  @file 	   ssd1306.h
 *  @brief     SSD1306 128x32 OLED library.
 *  @details   User can set different parameters detailed below.
 *  		   If an OS is used set QC_FRTOS to 1 (only portable for FreeRTOS)
 *  		   If dynamic image generation is needed set QC_MEM_DYN to 1
 *
 *			   Header files:
 *			   qc12864b.h 	Functionality configuration for QC12864B LCD control
 *			   qcBmp.h	  	Static bitmap image generation. Includes an static array for user edition
 *			   qcHardware.h All hardware related definitions, macro and header files needed for
 *			   				SPI control and communication to QC12864B LCD.
 *			   qcLCD.h		Application header file for QC12864B usage.
 *	@warning   Needs to be edited by the user accordingly
 *  @author    Federico Baigorria
 *  @date      06-11-2020
 *  @copyright GNU Public License.
 */

#ifndef __SSD1306_INC_SSD1306_H__
#define __SSD1306_INC_SSD1306_H__

/** DO NOT EDIT
 */

///! Self header files
#include <stdint.h>
#include "ssdHardware.h"
#include "ssdTypes.h"
#include "ssdFonts.h"

///! SSD1306 hardware limit
#define S1306_MAX_SEGMENT			128		///! OLED Display maximum width
#define S1306_MAX_PAGE				4		///! OLED Display maximum height (in pages). For a 128x32 set 4

///! Default slave address for SSD1306 Oled display
#define	S1306_SLAVE_ADDR			0x3C		///! 7 bits long address 011110(SAO). SAO bit may be 0 o 1. Default val 0x3C

///! Commands sent continuously.Packet: (Control byte - Command byte)
#define S1306_DISP_OFF				0x80,0xAE	///! Fundamental command. Set display off (sleep mode)
#define S1306_DISP_CLK_DIV_RATIO	0x80,0xD5 	///! Timing & Driving scheme. This is a double command (0xD5 Command + Data)
#define S1306_MULT_RATIO			0x80,0xA8	///! Hardware configuration. This is a double command (0xA8 Command + Data)
#define S1306_DISP_OFFSET 			0x80,0xD3	///! Hardware configuration. This is a double command (0xD3 Command + Data)
#define S1306_CHARGE_PUMP 			0x80,0x8D	///! Charge pump (special). This is a double command (0x8D Command + Data)
#define S1306_START_LINE_ADDR 		0x80,0x40	///! Hardware configuration. Set display RAM start line
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

///! Append for data packets (Continuosly sending data bytes)
#define	S1306_CMD_END_2RAM			0x40		///! End command transmision. Following bytes are data bytes to be stored at GDDRA
#define S1306_CMD_ONLY				0x00		///! End command transmision. Following bytes are command bytes

///! Separated command values
#define START_PAGE_VAL				0xB0

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

///! SSD1306 Init datagram transmission buffer. Configure according to users need
/*static uint8_t ssdInitDatagram[] = {
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
*/
static uint8_t ssdInitDatagram[] = {
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
								
///! Private functions
static void oled_i2c_write(uint8_t*, uint8_t);
static void ssdOledSendCmd(uint8_t);
static void ssdOledSendData(uint8_t);
static void ssdOledSendAscii(uint8_t);

#endif /* __SSD1306_INC_SSD1306_H__ */
