/**
 * Public interface for the Esplora PCB helpers (mux, light sensor, and RGB PWM).
 * Pin map based on arduino-libraries/Esplora.
 *
 * Copyright(C) 2025 Pierre Quelin <pierre.quelin.1972@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * For the full license text, see: https://www.gnu.org/licenses/gpl-3.0.txt
 */

#ifndef ESPLORA_BOARD_H
#define ESPLORA_BOARD_H

#include <stdint.h>
#include <stdbool.h>

enum
{
	ESPLORA_CH_SWITCH_1 = 0,
	ESPLORA_CH_SWITCH_2 = 1,
	ESPLORA_CH_SWITCH_3 = 2,
	ESPLORA_CH_SWITCH_4 = 3,
	ESPLORA_CH_LIGHT    = 5,
};

void EsploraBoard_Init(void);

/** 10-bit ADC via onboard mux (channels 0–9). */
uint16_t EsploraBoard_ReadChannel(uint8_t channel);

/** true if switch pressed (active low via divider / mux). */
bool EsploraBoard_ReadSwitchPressed(uint8_t switchIndex1to4);

uint16_t EsploraBoard_ReadLight(void);

void EsploraBoard_WriteRgb(uint8_t red, uint8_t green, uint8_t blue);

#endif
