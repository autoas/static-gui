/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
#ifndef _SGDRAW_H_
#define _SGDRAW_H_
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"

/* ================================ [ MACROS    ] ============================================== */

/* ================================ [ TYPES     ] ============================================== */

/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void Sg_DrawPixel(int x, int y, uint32_t color);
void Sg_DrawLine(int x0, int y0, int x1, int y1, uint32_t color);
void Sg_FillArea(int x, int y, int cx, int cy, uint32_t color);
void Sg_DrawCircle(int x, int y, int radius, uint32_t color);
void Sg_FillCircle(int x, int y, int radius, uint32_t color);
void Sg_DrawEllipse(int x, int y, int a, int b, uint32_t color);
void Sg_FillEllipse(int x, int y, int a, int b, uint32_t color);
#endif /* _SGDRAW_H_ */
