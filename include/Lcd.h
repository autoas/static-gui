/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
#ifndef _LCD_H_
#define _LCD_H_
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"

/* ================================ [ MACROS    ] ============================================== */

/* ================================ [ TYPES     ] ============================================== */

/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void Lcd_Init(void);
void Lcd_DrawPixel(uint32_t x, uint32_t y, uint32_t color);
#endif /* _LCD_H_ */
