/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
#ifndef _STMO_H_
#define _STMO_H_
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"
#include "Stmo_Cfg.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
extern void Stmo_Init(const Stmo_ConfigType *Config);
extern Std_ReturnType Stmo_SetPosDegree(Stmo_IdType Id, Stmo_DegreeType Degree);
extern Std_ReturnType Stmo_GetPosDegree(Stmo_IdType Id, Stmo_DegreeType *Degree);
extern void Stmo_MainFunction(void);

#endif /* _STMO_H_ */
