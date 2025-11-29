/*!
 *  @file 	   ssdTypes.h
 *  @brief     SSD1306 types.
 *  @details   For now it's not used.
 *
 *  @author    Federico Baigorria
 *  @date      03-24-2020
 *  @copyright GNU Public License.
 */

#ifndef __SSD1306_INC_SSDTYPES_H__
#define __SSD1306_INC_SSDTYPES_H__

#define	S1306_X_START_OFFS	16
#define	S1306_X_END_OFFS	16
#define S1306_HOR_LIMIT 	128 - S1306_X_START_OFFS - S1306_X_END_OFFS

typedef enum {
	ssdSDCardSymb = 0,
	ssdFileSymb,
	ssdNoSymb
} ssdSymbols;

#endif /* __SSD1306_INC_SSDTYPES_H__ */
