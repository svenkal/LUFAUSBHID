/*
  Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "Descriptors.h"

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

// Timeout for UART Keycode setting
#define KEYSETTIMEOUT 50	

// Scancodes storted in EEPROM
#define KEYAMOUNT 7
#define KEY0LOC ((uint8_t*)50) // 1kB EEPROM / 1 Byte = 1000 
#define KEY1LOC ((uint8_t*)51)
#define KEY2LOC ((uint8_t*)52)
#define KEY3LOC ((uint8_t*)53)
#define KEY4LOC	((uint8_t*)54)
#define KEY5LOC	((uint8_t*)55)
#define KEY6LOC	((uint8_t*)56)	

//#define CAPSLED (1<<5) //PORTB,5
#define KEY0 (1<<5) //PB5 
#define KEY1 (1<<4) //PB4
#define KEY2 (1<<6) //PE6
#define KEY3 (1<<7) //PD7
#define KEY4 (1<<4) //PD4
#define KEY5 (1<<0) //PD0
#define KEY6 (1<<6) //PC6

void SetupKeys(void);
void SetupUART(void);
void SetupGPIO(void);
void SetupHardware(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
	                                        uint8_t* const ReportID,
	                                        const uint8_t ReportType,
	                                        void* ReportData,
	                                        uint16_t* const ReportSize);
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
		                                    const uint8_t ReportID,
		                                    const uint8_t ReportType,
		                                    const void* ReportData,
		                                    const uint16_t ReportSize);

#endif