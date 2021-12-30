/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
#ifndef STMO_CFG_H_
#define STMO_CFG_H_

#define STMO_MAX_DEGREE  (36000)
#define STMO_ONE_DEGREE  (100)

#define STMO_ONE_STEP    (250*2)  // Degree Change in 1 Stmo_MainFunction call

typedef uint32_t Stmo_DegreeType;

typedef enum
{
	STMO_ID_SPEED,
	STMO_ID_TACHO,
	STMO_ID_TEMP,
	STMO_ID_FUEL,
	STMO_CFG_NUM
}Stmo_IdType;

typedef enum
{
	STMO_DIR_CLOCKWISE,
	STMO_DIR_ANTICLOCKWISE
}Stmo_DirectionType;

typedef struct
{
	Stmo_DegreeType SoftwareZero;
	Stmo_DirectionType Direction;

}Stmo_ChannelType;

typedef struct
{
	const Stmo_ChannelType* Channels;
}Stmo_ConfigType;

extern const Stmo_ConfigType Stmo_ConfigData;

#endif /* STMO_CFG_H_ */
