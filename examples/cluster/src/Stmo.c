/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Stmo.h"
#include <string.h>
/* ================================ [ MACROS    ] ============================================== */

#define VALIDATE_ID(Id, _api)                                                                      \
  if ((Id) > STMO_CFG_NUM) {                                                                       \
    goto cleanup;                                                                                  \
  }

#define GET_STMO_SOFTWARE_ZERO(Id) (pConfig->Channels[Id].SoftwareZero)

#define GET_STMO_DIRECTION(Id) (pConfig->Channels[Id].Direction)
/* ================================ [ TYPES     ] ============================================== */
typedef struct {
  Stmo_DegreeType Current;
  Stmo_DegreeType Inter;
  Stmo_DegreeType Command;
} Stmo_ControllerType;
/* ================================ [ DECLARES  ] ============================================== */
extern const Stmo_ConfigType Stmo_ConfigData;
/* ================================ [ DATAS     ] ============================================== */
static Stmo_ControllerType StmoCtrl[STMO_CFG_NUM];
static const Stmo_ConfigType *pConfig = NULL;
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void Stmo_Init(const Stmo_ConfigType *Config) {
  uint32_t i;
  if (NULL != Config) {
    pConfig = Config;
  } else {
    pConfig = &Stmo_ConfigData;
  }
  memset(StmoCtrl, 0, sizeof(StmoCtrl));

  for (i = 0; i < STMO_CFG_NUM; i++) {
    StmoCtrl[i].Command = GET_STMO_SOFTWARE_ZERO(i);
    StmoCtrl[i].Inter = GET_STMO_SOFTWARE_ZERO(i);
    StmoCtrl[i].Current = GET_STMO_SOFTWARE_ZERO(i);
  }
}

Std_ReturnType Stmo_SetPosDegree(Stmo_IdType Id, Stmo_DegreeType Degree) {
  Std_ReturnType ercd = E_NOT_OK;
  VALIDATE_ID(Id, Stmo_SetPosDegree);

  if (Degree <= STMO_MAX_DEGREE) {
    StmoCtrl[Id].Command = Degree;
    ercd = E_OK;
  }

cleanup:
  return ercd;
}

Std_ReturnType Stmo_GetPosDegree(Stmo_IdType Id, Stmo_DegreeType *Degree) {
  Std_ReturnType ercd = E_NOT_OK;
  VALIDATE_ID(Id, Stmo_GetPosDegree);

  *Degree = StmoCtrl[Id].Current;
  ercd = E_OK;

cleanup:
  return ercd;
}

void Stmo_MainFunction(void) {
  for (int i = 0; i < STMO_CFG_NUM; i++) {
    if (StmoCtrl[i].Command != StmoCtrl[i].Inter) {
      if (StmoCtrl[i].Command > StmoCtrl[i].Inter) {
        if (StmoCtrl[i].Command > (StmoCtrl[i].Inter + STMO_ONE_STEP)) {
          StmoCtrl[i].Inter = StmoCtrl[i].Inter + STMO_ONE_STEP;
        } else {
          StmoCtrl[i].Inter = StmoCtrl[i].Command;
        }
      } else if (StmoCtrl[i].Command < StmoCtrl[i].Inter) {
        if ((StmoCtrl[i].Command + STMO_ONE_STEP) < StmoCtrl[i].Inter) {
          StmoCtrl[i].Inter = StmoCtrl[i].Inter - STMO_ONE_STEP;
        } else {
          StmoCtrl[i].Inter = StmoCtrl[i].Command;
        }
      } else {
        StmoCtrl[i].Inter = StmoCtrl[i].Command;
      }

      if (GET_STMO_DIRECTION(i) == STMO_DIR_CLOCKWISE) {
        StmoCtrl[i].Current = GET_STMO_SOFTWARE_ZERO(i) + StmoCtrl[i].Inter;
      } else {
        if (GET_STMO_SOFTWARE_ZERO(i) >= StmoCtrl[i].Inter) {
          StmoCtrl[i].Current = GET_STMO_SOFTWARE_ZERO(i) - StmoCtrl[i].Inter;
        } else {
          StmoCtrl[i].Current = STMO_MAX_DEGREE + GET_STMO_SOFTWARE_ZERO(i) - StmoCtrl[i].Inter;
        }
      }
    }
  }
}
