/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
#ifndef _SG_H_
#define _SG_H_
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"

/* ================================ [ MACROS    ] ============================================== */
/* declaration of the LCD size */
#define SG_LCD_WIGTH __SG_WIDTH__
#define SG_LCD_HEIGHT __SG_HEIGHT__

enum
{
  SGT_DMP,
  SGT_BMP,
  SGT_TXT,
  SGT_INVALID = 0xFF
};
enum
{ /* layer */
  SGL_0,
  SGL_1,
  SGL_2,
  SGL_3,
  SGL_4,
  SGL_5,
  SGL_6,
  SGL_7,
  SGL_8,
  SGL_9,
  SGL_10,
  SGL_11,
  SGL_12,
  SGL_13,
  SGL_14,
  SGL_15,
  SGL_INVALID = 0xFF
};
/* ================================ [ TYPES     ] ============================================== */
typedef struct /* base resource object */
{
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
  union {
    uint8_t *pu8;
    uint32_t *pu32;
    void *(*f)(void *);
  } u;
} SgRes;

typedef struct /* public:SgRes*/
{
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
  const uint8_t *p; /* without information of color, for fonts */
} SgDMP;            /* dotmap */

typedef struct /* public:SgRes*/
{
  /* (x,y) record the related center point of this BMP resource */
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
  const uint32_t *p; /* with information of color, for pictures */
} SgBMP;             /* bitmap */

typedef struct /* public:SgRes */
{
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
  void *(*f)(void *);
} SgSDD; /*Sg Special Dynamic Draw */

typedef struct /* public:SgRes */
{
  const uint16_t *l; /* look up table          */
  const uint8_t **p; /* resource pointer table */
  uint32_t w;
  uint32_t h;
  uint16_t s;
} SgTXT;

typedef struct {
  uint8_t t;
  uint16_t rs;         /* resource size */
  const SgRes **r;     /* resource */
  void *(*rf)(void *); /* refresh resource according to the cache */
  void (*cf)(void *);  /* cache current status/data of the widget */
  uint16_t weight;     /* weight of this widget */
  char *name;
} SgSRC; /* static resource configuration */
/*		  	  Width (w)
 * 		  + ---------- x
 *  H     |
 *  e     |
 *  i     |
 *  g (h) |
 *  h     |
 *  t     |
 *        y
 */
typedef struct {
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
  uint32_t c; /* color 		 */
  uint16_t d; /* rotate degree:unit in 1 */
  uint8_t l;  /* layer number of the widget */
  uint8_t ri; /* resource index */
  const SgSRC *src;
} SgWidget;
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void Sg_Init(void);
void Sg_Update(void);
void Sg_MainFunction(void);
boolean Sg_IsDataReady(void);
#endif /* _SG_H_ */
