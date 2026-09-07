/**
 * Arduino Esplora board I/O (ATmega32U4), without the Arduino core.
 *
 * Mux: A0..A3 = PF7..PF4 address, A4 = PF1 common (ADC1).
 * RGB: D5=PC6/OC3A red, D10=PB6/OC1B green, D9=PB5/OC1A blue.
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

#include "EsploraBoard.h"

#include <avr/io.h>
#include <util/delay.h>

void EsploraBoard_Init(void)
{
	/* Mux address outputs PF7..PF4 */
	DDRF |= (1 << PF7) | (1 << PF6) | (1 << PF5) | (1 << PF4);
	/* Mux common ADC input PF1 */
	DDRF &= ~(1 << PF1);

	/* RGB pins low as GPIO before PWM (avoids boot glow on common-anode LED) */
	DDRC |= (1 << PC6);
	PORTC &= ~(1 << PC6);
	DDRB |= (1 << PB6) | (1 << PB5);
	PORTB &= ~((1 << PB6) | (1 << PB5));
	OCR1A = 0;
	OCR1B = 0;
	OCR3A = 0;

	/* Timer1 Fast PWM 8-bit, non-inverting OC1A/OC1B, no prescale */
	TCCR1A = (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
	TCCR1B = (1 << WGM12) | (1 << CS10);

	/* Timer3 Fast PWM 8-bit, non-inverting OC3A */
	TCCR3A = (1 << WGM30) | (1 << COM3A1);
	TCCR3B = (1 << WGM32) | (1 << CS30);

	EsploraBoard_WriteRgb(0, 0, 0);

	/* ADC: AVCC ref, right-adjusted, enable */
	ADMUX  = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRB = 0;
}

static void MuxSelect(uint8_t channel)
{
	if (channel & 0x01U)
		PORTF |= (1 << PF7);
	else
		PORTF &= ~(1 << PF7);

	if (channel & 0x02U)
		PORTF |= (1 << PF6);
	else
		PORTF &= ~(1 << PF6);

	if (channel & 0x04U)
		PORTF |= (1 << PF5);
	else
		PORTF &= ~(1 << PF5);

	if (channel & 0x08U)
		PORTF |= (1 << PF4);
	else
		PORTF &= ~(1 << PF4);

	/* Settle mux + sample (Arduino library uses digitalWrite then analogRead). */
	_delay_us(50);
}

uint16_t EsploraBoard_ReadChannel(uint8_t channel)
{
	MuxSelect(channel);

	/* ADC1 = PF1 */
	ADMUX = (1 << REFS0) | (1 << MUX0);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
	{
	}

	return ADC;
}

bool EsploraBoard_ReadSwitchPressed(uint8_t switchIndex1to4)
{
	if (switchIndex1to4 < 1 || switchIndex1to4 > 4)
		return false;

	const uint16_t val = EsploraBoard_ReadChannel((uint8_t)(switchIndex1to4 - 1U));
	/* Esplora library: pressed → LOW when val <= 512 */
	return val <= 512U;
}

uint16_t EsploraBoard_ReadLight(void)
{
	return EsploraBoard_ReadChannel(ESPLORA_CH_LIGHT);
}

void EsploraBoard_WriteRgb(uint8_t red, uint8_t green, uint8_t blue)
{
	OCR3A = red;
	OCR1B = green;
	OCR1A = blue;
}
