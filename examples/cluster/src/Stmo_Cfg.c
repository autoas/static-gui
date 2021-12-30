/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
#include "Stmo.h"

static const Stmo_ChannelType StmoChannels[] = {
  {
    /* Speed */
    /*.SoftwareZero =*/15100,
    /*.Direction =*/STMO_DIR_CLOCKWISE,
  },
  {
    /* Tacho */
    /*.SoftwareZero =*/15100,
    /*.Direction =*/STMO_DIR_CLOCKWISE,
  },
  {
    /* Temperature */
    /*.SoftwareZero =*/13100,
    /*.Direction =*/STMO_DIR_CLOCKWISE,
  },
  {
    /* Fuel */
    /*.SoftwareZero =*/4800,
    /*.Direction =*/STMO_DIR_ANTICLOCKWISE,
  },

};

const Stmo_ConfigType Stmo_ConfigData = {
  /*.Channels =*/StmoChannels,
};
