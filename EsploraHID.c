/**
 * Arduino Esplora LUFA Generic HID firmware, paired with libesplora_hid.
 *
 * Report contract (64 bytes):
 *   IN:  [0] bits0-3 = switch1-4 pressed; [1..2] light ADC LE uint16
 *   OUT: [0]=R [1]=G [2]=B
 *
 * IN is event-driven (switch change or host OUT sync), allowing the PC to
 * block on the interrupt IN endpoint without a polling loop.
 *
 * Based on the LUFA LowLevel GenericHID demo by Dean Camera.
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

#include "EsploraHID.h"

/** True → send one IN when the endpoint is ready (config / host OUT). */
static bool    g_NeedHidIn    = true;
static uint8_t g_PrevSwitches = 0xFF;

int main(void)
{
	SetupHardware();
	GlobalInterruptEnable();

	for (;;)
	{
		HID_Task();
		USB_USBTask();
	}
}

void SetupHardware(void)
{
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	clock_prescale_set(clock_div_1);

	EsploraBoard_Init();
	USB_Init();
}

void EVENT_USB_Device_Connect(void) {}
void EVENT_USB_Device_Disconnect(void) {}

void EVENT_USB_Device_ConfigurationChanged(void)
{
	Endpoint_ConfigureEndpoint(GENERIC_IN_EPADDR, EP_TYPE_INTERRUPT, GENERIC_EPSIZE, 1);
	Endpoint_ConfigureEndpoint(GENERIC_OUT_EPADDR, EP_TYPE_INTERRUPT, GENERIC_EPSIZE, 1);
	g_NeedHidIn    = true;
	g_PrevSwitches = 0xFF;
}

void EVENT_USB_Device_ControlRequest(void)
{
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				uint8_t GenericData[GENERIC_REPORT_SIZE];
				CreateGenericHIDReport(GenericData);
				Endpoint_ClearSETUP();
				Endpoint_Write_Control_Stream_LE(&GenericData, sizeof(GenericData));
				Endpoint_ClearOUT();
			}
			break;
		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				uint8_t GenericData[GENERIC_REPORT_SIZE];
				Endpoint_ClearSETUP();
				Endpoint_Read_Control_Stream_LE(&GenericData, sizeof(GenericData));
				Endpoint_ClearIN();
				ProcessGenericHIDReport(GenericData);
			}
			break;
	}
}

void ProcessGenericHIDReport(uint8_t* DataArray)
{
	EsploraBoard_WriteRgb(DataArray[0], DataArray[1], DataArray[2]);
	/* Host OUT also requests one status IN (sync without continuous stream). */
	g_NeedHidIn = true;
}

void CreateGenericHIDReport(uint8_t* DataArray)
{
	memset(DataArray, 0, GENERIC_REPORT_SIZE);

	uint8_t switches = 0;
	if (EsploraBoard_ReadSwitchPressed(1))
		switches |= (1U << 0);
	if (EsploraBoard_ReadSwitchPressed(2))
		switches |= (1U << 1);
	if (EsploraBoard_ReadSwitchPressed(3))
		switches |= (1U << 2);
	if (EsploraBoard_ReadSwitchPressed(4))
		switches |= (1U << 3);

	const uint16_t light = EsploraBoard_ReadLight();
	DataArray[0] = switches;
	DataArray[1] = (uint8_t)(light & 0xFFU);
	DataArray[2] = (uint8_t)((light >> 8) & 0xFFU);
}

void HID_Task(void)
{
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	Endpoint_SelectEndpoint(GENERIC_OUT_EPADDR);
	if (Endpoint_IsOUTReceived())
	{
		if (Endpoint_IsReadWriteAllowed())
		{
			uint8_t GenericData[GENERIC_REPORT_SIZE];
			Endpoint_Read_Stream_LE(&GenericData, sizeof(GenericData), NULL);
			ProcessGenericHIDReport(GenericData);
		}
		Endpoint_ClearOUT();
	}

	/*
	 * Event-driven IN: switch nibble change or host OUT sync kick.
	 * Host blocks in libusb interrupt IN — no userspace busy-wait.
	 * If the IN bank is stuck after a driver switch, abort then retry next loop.
	 */
	Endpoint_SelectEndpoint(GENERIC_IN_EPADDR);
	if (!Endpoint_IsINReady())
	{
		if (g_NeedHidIn)
		{
			Endpoint_AbortPendingIN();
		}
		return;
	}

	{
		uint8_t GenericData[GENERIC_REPORT_SIZE];
		CreateGenericHIDReport(GenericData);

		const uint8_t switches = GenericData[0];
		if (g_NeedHidIn || switches != g_PrevSwitches)
		{
			Endpoint_Write_Stream_LE(&GenericData, sizeof(GenericData), NULL);
			Endpoint_ClearIN();
			g_PrevSwitches = switches;
			g_NeedHidIn    = false;
		}
	}
}
