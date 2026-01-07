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

#include <stdint.h>

#define	S1306_SLAVE_ADDR			0x3C		///! 7 bits long address 011110(SAO). SAO bit may be 0 o 1. Default slave address for SSD1306 OLED display is 0x3C
#define USING_128X64_OLED			1			///! OLED Display maximum height (in pages). For a 128x32 is set to 4 and for 128x64 it's 8

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
	} ssd1306_pages_t;
#else
	typedef enum {
		S1306_PAGE0,
		S1306_PAGE1,
		S1306_PAGE2,
		S1306_PAGE3,
		S1306_MAX_PAGE
	} ssd1306_pages_t;
#endif

typedef void (*i2c_write_fn)(uint32_t const, uint8_t*, uint32_t);
typedef void (*i2c_hw_init_fn)(void);
typedef enum {SYM_SDCARD, SYM_FILE, SYM_NOSYMBOL, SYM_TOTAL} ssd1306_symbols_t;
typedef enum {VH_2FRAMES=7, VH_3FRAMES=4, VH_4FRAMES=5, VH_5FRAMES=0, VH_25FRAMES=6, VH_64FRAMES=1, VH_128FRAMES=2, VH_256FRAMES=3} ssd1306_frame_int_t;
typedef enum {LH_SCROLL_EN, RH_SCROLL_EN} ssd1306_horz_side_scroll_t;

///! Common
void ssd1306_device_startup(uint32_t const slave_addr, i2c_write_fn writer_fn, i2c_hw_init_fn hw_init_fn);
void ssd1306_set_page_offset(ssd1306_pages_t page, uint8_t column_addr);
void ssd1306_page_clear(ssd1306_pages_t page);
void ssd1306_display_clear(void);
void ssd1306_print_ascii(uint8_t ascii_char);
void ssd1306_print_text(uint8_t *text, uint8_t en_page_change);

///! Tests
void ssd1306_display_all_on_test(void);
void ssd1306_display_resume(void);

///! Graphics
void ssd1306_static_horizontal_bar(ssd1306_pages_t at_page, uint32_t x_start_loc, uint32_t x_bar_len, double percent_fill);
void ssd1306_print_symbol(ssd1306_symbols_t ssd_symbol, ssd1306_pages_t at_page, uint32_t x_loc);
void ssd1306_scroll_enable(void);
void ssd1306_scroll_disable(void);
void ssd1306_vh_cont_scroll_setup(ssd1306_pages_t start_page, ssd1306_pages_t end_page, ssd1306_frame_int_t frame_int, uint8_t v_scroll_offs, ssd1306_horz_side_scroll_t h_scroll);
void ssd1306_h_cont_scroll_setup(ssd1306_pages_t start_page, ssd1306_pages_t end_page, ssd1306_frame_int_t frame_int, ssd1306_horz_side_scroll_t h_scroll);
void ssd1306_set_v_addressing_mode(void);
void ssd1306_inverse_display_mode(void);
void ssd1306_normal_display_mode(void);

#endif /* __SSD1306_INC_SSD1306_H__ */
