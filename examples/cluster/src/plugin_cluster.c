/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Sg.h"
#include "Lcd.h"
#include "Stmo.h"
#include "Com.h"
#include "Com_Cfg.h"
#include "plugin.h"
#include "Dio.h"
#include "Dio_Cfg.h"
/* ================================ [ MACROS    ] ============================================== */
#ifndef PLUGIN_MAIN_PERIOD
#define PLUGIN_MAIN_PERIOD 10
#endif
#define mMS2Ticks(t) (((t) + PLUGIN_MAIN_PERIOD - 1) / PLUGIN_MAIN_PERIOD)
/* ================================ [ TYPES     ] ============================================== */
typedef enum
{
  eTelltaleTPMS = 0,
  eTelltaleLowOil,
  eTelltalePosLamp,
  eTelltaleTurnLeft,
  eTelltaleTurnRight,
  eTelltaleAutoCruise,
  eTelltaleHighBeam,
  eTelltaleSeatbeltDriver,
  eTelltaleSeatbeltPassenger,
  eTelltaleAirbag,
  eTelltaleMax
} TellTaleType;

typedef enum
{
  eTelltaleStatusOff = 0,
  eTelltaleStatusOn,
  eTelltaleStatus1Hz,
  eTelltaleStatus2Hz,
  eTelltaleStatus3Hz,
  eTelltaleStatusMax
} TelltaleStatusType;
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
static uint16_t TellatleTimer[eTelltaleStatusMax];
static Dio_LevelType TellatleLevel[eTelltaleStatusMax];
static const Dio_ChannelType TellatleChannel[eTelltaleMax] = {
  DIO_CHL_TelltaleTPMS,     DIO_CHL_TelltaleLowOil,         DIO_CHL_TelltalePosLamp,
  DIO_CHL_TelltaleTurnLeft, DIO_CHL_TelltaleTurnRight,      DIO_CHL_TelltaleAutoCruise,
  DIO_CHL_TelltaleHighBeam, DIO_CHL_TelltaleSeatbeltDriver, DIO_CHL_TelltaleSeatbeltPassenger,
  DIO_CHL_TelltaleAirbag};

static const uint16_t TellatleHzCfg[eTelltaleStatusMax][2] = {
  /* { duty, period } */
  {mMS2Ticks(1000), mMS2Ticks(1000) - 1}, /* eTelltaleStatusOff */
  {mMS2Ticks(0), mMS2Ticks(0) - 1},       /* eTelltaleStatusOn */
  {mMS2Ticks(500), mMS2Ticks(1000) - 1},  /* eTelltaleStatus1Hz */
  {mMS2Ticks(250), mMS2Ticks(500) - 1},   /* eTelltaleStatus2Hz */
  {mMS2Ticks(167), mMS2Ticks(333) - 1},   /* eTelltaleStatus3Hz */
};

static TelltaleStatusType TelltaleStatus[eTelltaleMax];
/* ================================ [ LOCALS    ] ============================================== */
static void sample_pointer(void) {
#ifdef COM_SID_VehicleSpeed
  uint16_t VehicleSpeed = 0;
#else
  static boolean speed_up = TRUE;
#endif
#ifdef COM_SID_TachoSpeed
  uint16_t TachoSpeed = 0;
#else
  static boolean tacho_up = TRUE;
#endif

  static Stmo_DegreeType tacho = 0;
  static Stmo_DegreeType speed = 0;
  static Stmo_DegreeType temp = 0;
  static Stmo_DegreeType fuel = 0;

  static boolean temp_up = TRUE;
  static boolean fuel_up = TRUE;

#ifdef COM_SID_TachoSpeed
  (void)Com_ReceiveSignal(COM_SID_TachoSpeed, &TachoSpeed);
  tacho = TachoSpeed;
#else
  if (tacho_up) {
    tacho += 50;
    if (tacho >= 24000) {
      tacho = 24000;
      tacho_up = FALSE;
    }
  } else {
    if (tacho > 100) {
      tacho -= 100;
    } else {
      tacho = 0;
      tacho_up = TRUE;
    }
  }
#endif

#ifdef COM_SID_VehicleSpeed
  (void)Com_ReceiveSignal(COM_SID_VehicleSpeed, &VehicleSpeed);
  speed = VehicleSpeed;
#else
  if (speed_up) {
    speed += 50;
    if (speed >= 24000) {
      speed = 24000;
      speed_up = FALSE;
    }
  } else {
    if (speed > 100) {
      speed -= 100;
    } else {
      speed = 0;
      speed_up = TRUE;
    }
  }
#endif
  if (temp_up) {
    temp += 50;
    if (temp >= 9700) {
      temp = 9700;
      temp_up = FALSE;
    }
  } else {
    if (temp > 50) {
      temp -= 50;
    } else {
      temp = 0;
      temp_up = TRUE;
    }
  }

  if (fuel_up) {
    fuel += 50;
    if (fuel >= 9700) {
      fuel = 9700;
      fuel_up = FALSE;
    }
  } else {
    if (fuel > 50) {
      fuel -= 50;
    } else {
      fuel = 0;
      fuel_up = TRUE;
    }
  }

  Stmo_SetPosDegree(STMO_ID_SPEED, speed);
  Stmo_SetPosDegree(STMO_ID_TACHO, tacho);
  Stmo_SetPosDegree(STMO_ID_TEMP, temp);
  Stmo_SetPosDegree(STMO_ID_FUEL, fuel);
}

static void sample_telltale(void) {
  TelltaleStatus[eTelltaleTPMS] = eTelltaleStatusOn;
  TelltaleStatus[eTelltaleLowOil] = eTelltaleStatus1Hz;
  TelltaleStatus[eTelltalePosLamp] = eTelltaleStatus2Hz;
  TelltaleStatus[eTelltaleTurnLeft] = eTelltaleStatus3Hz;
  TelltaleStatus[eTelltaleTurnRight] = eTelltaleStatusOn;
  TelltaleStatus[eTelltaleAutoCruise] = eTelltaleStatus1Hz;
  TelltaleStatus[eTelltaleHighBeam] = eTelltaleStatus2Hz;
  TelltaleStatus[eTelltaleSeatbeltDriver] = eTelltaleStatus3Hz;
  TelltaleStatus[eTelltaleSeatbeltPassenger] = eTelltaleStatusOn;
  TelltaleStatus[eTelltaleAirbag] = eTelltaleStatus1Hz;
}

static void Swc_TelltaleManager(void) { /* period is 5ms */
  int i;
  for (i = 0; i < eTelltaleStatusMax; i++) {
    TellatleTimer[i]++;
    if (TellatleTimer[i] < TellatleHzCfg[i][0]) { /* in the low duty */
      TellatleLevel[i] = STD_LOW;                 /* off the Telltale */
    } else {
      TellatleLevel[i] = STD_HIGH;                  /* on the Telltale */
      if (TellatleTimer[i] > TellatleHzCfg[i][1]) { /* reach the period */
        TellatleTimer[i] = 0;
        TellatleLevel[i] = STD_LOW; /* off the Telltale */
      }
    }
  }
  /* refresh Telltale */
  for (i = 0; i < eTelltaleMax; i++) {
    Dio_WriteChannel(TellatleChannel[i], TellatleLevel[TelltaleStatus[i]]);
  }
}
/* ================================ [ FUNCTIONS ] ============================================== */
void cluster_init(void) {
  Lcd_Init();
  Sg_Init();
  Stmo_Init(NULL);
}

void cluster_main(void) {
  sample_pointer();
  sample_telltale();
  Swc_TelltaleManager();

  Sg_MainFunction();
  Stmo_MainFunction();
}

void cluster_deinit(void) {
}

REGISTER_PLUGIN(cluster)