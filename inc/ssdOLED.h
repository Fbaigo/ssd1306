/*!
 *  @file 	   ssdOLED.h
 *  @brief     SSD1306 application header file. DO NOT EDIT.
 *  @details   Include this header in your application display control file.
 *
 *	@warning   SSD1306 sddOledWrite() static function needs that all the required peripheral and pins to
 *			   be previously set and configured by user's application. Call this function after
 *			   hardware initialization only. Refer to ssd1306.h file.
 *  @author    Federico Baigorria
 *  @date      03-24-2020
 *  @copyright GNU Public License.
 */

#ifndef __SSD1306_INC_SSDOLED_H__
#define __SSD1306_INC_SSDOLED_H__

#include "ssdHardware.h"
#include "ssdTypes.h"
///! Page addressing mode
#define S1306_PAGE0		0
#define S1306_PAGE1		1
#define S1306_PAGE2		2
#define S1306_PAGE3		3

///! Public functions
void ssdI2CHardwareInit(void);
void ssdOledInit(void);
void ssdOledSetPageOrigin(uint8_t);
void ssdOledSetPageOffset(uint8_t, uint8_t);
void ssdOledSetText(uint8_t *, uint8_t);
void ssdOledSetSymbol(uint8_t);
void ssdOledCleanPage(uint8_t);
void ssdOledCleanScreen(void);
void ssdOnResetMsg(uint8_t *);
void ssdOledBarGraph(void);
void ssdOledUpdateBar(uint8_t);

#endif /* __SSD1306_INC_SSDOLED_H__ */
