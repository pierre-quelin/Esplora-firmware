/**
 * USB descriptor declarations for the Esplora Generic HID device.
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

#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include <LUFA/Drivers/USB/USB.h>
#include <avr/pgmspace.h>
#include "Config/AppConfig.h"

typedef struct
{
	USB_Descriptor_Configuration_Header_t Config;
	USB_Descriptor_Interface_t            HID_Interface;
	USB_HID_Descriptor_HID_t              HID_GenericHID;
	USB_Descriptor_Endpoint_t             HID_ReportINEndpoint;
	USB_Descriptor_Endpoint_t             HID_ReportOUTEndpoint;
} USB_Descriptor_Configuration_t;

enum InterfaceDescriptors_t
{
	INTERFACE_ID_GenericHID = 0,
};

enum StringDescriptors_t
{
	STRING_ID_Language     = 0,
	STRING_ID_Manufacturer = 1,
	STRING_ID_Product      = 2,
};

#define GENERIC_IN_EPADDR  (ENDPOINT_DIR_IN | 1)
#define GENERIC_OUT_EPADDR (ENDPOINT_DIR_OUT | 2)
#define GENERIC_EPSIZE     64

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
