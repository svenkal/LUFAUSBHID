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

#include "Keyboard.h"

uint8_t counter; // incrementet every SOF, reset on KEYSETTIMEOUT
uint8_t keybuffer[KEYAMOUNT], RXCounter; 
uint8_t key0code = 0x4F, key1code = 0x50, key2code = 0x51, key3code = 0x52, key4code = 0x04, key5code = 0x05, key6code = 0x06; // default values if EEPROM was erased

static uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)]; // for Driver

USB_ClassInfo_HID_Device_t Keyboard_HID_Interface = {
	.Config = {
		.InterfaceNumber = INTERFACE_ID_Keyboard,
		.ReportINEndpoint = { // IN = device to host, reports pressed keys
				.Address  = KEYBOARD_EPADDR,
				.Size = KEYBOARD_EPSIZE,
				.Banks = 1,
			},
		.PrevReportINBuffer = PrevKeyboardHIDReportBuffer,
		.PrevReportINBufferSize = sizeof(PrevKeyboardHIDReportBuffer)
	}
};

int main(void) {
	SetupHardware();
	GlobalInterruptEnable(); //sei();

	while(1) {
		HID_Device_USBTask(&Keyboard_HID_Interface); // call before ustask
		USB_USBTask(); // handles events (e.g. setup event)
	}
}

void SetupHardware() {
	// disable Watchdog if fuse is burnt
	MCUSR &= ~(1 << 3);
	wdt_disable();

	// disable clkdiv if fuse is burnt
	clock_prescale_set(clock_div_1);

	SetupKeys();
	SetupGPIO();
	SetupUART();
	USB_Init();
}

void SetupKeys(void) {
	// you could do all this with arrays and loops (same for later stuff)
	// I think this also saves flash space since we dont need to store the array of consts e.g. static uint8_t keylocs[KEYAMOUNT] = {KEY0LOC, ...}
	// but that would be overengineered (also loop unrolling)
	if(eeprom_read_byte(KEY0LOC) != 0xFF) key0code = eeprom_read_byte(KEY0LOC); // read stored keycodes (erased eeprom ist 0xFF)
	if(eeprom_read_byte(KEY1LOC) != 0xFF) key1code = eeprom_read_byte(KEY1LOC);
	if(eeprom_read_byte(KEY2LOC) != 0xFF) key2code = eeprom_read_byte(KEY2LOC);
	if(eeprom_read_byte(KEY3LOC) != 0xFF) key3code = eeprom_read_byte(KEY3LOC);
	if(eeprom_read_byte(KEY4LOC) != 0xFF) key4code = eeprom_read_byte(KEY4LOC);
	if(eeprom_read_byte(KEY5LOC) != 0xFF) key5code = eeprom_read_byte(KEY5LOC);
	if(eeprom_read_byte(KEY6LOC) != 0xFF) key6code = eeprom_read_byte(KEY6LOC);
}

void SetupUART(void) {
	UBRR1 = 103; // 9600 BAUD @ 16MHz
	UCSR1B |= (1<<4); //Receiver on
}

void SetupGPIO(void) {

	DDRB &= ~(KEY0 | KEY1);
	DDRE &= ~KEY2;
	DDRD &= ~(KEY3 | KEY4 | KEY5);
	DDRC &= ~KEY6;

	PORTB |= KEY0 | KEY1;
	PORTE |= KEY2; 
	PORTD |= KEY3 | KEY4 | KEY5;
	PORTC |= KEY6;

	//DDRB |= CAPSLED; // Caps LED
	//PORTB &= ~CAPSLED; // Caps LED clear
}

void EVENT_USB_Device_StartOfFrame(void) {
	HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
	++counter;

	// if rxbuffer not empty
	if(UCSR1A & (1<<7)) keybuffer[RXCounter++] = UDR1; // no validity check for HID scancode

	// every KEYSETTIMEOUTms
	// although a loop would have been more readable here
	if(counter >= KEYSETTIMEOUT) {
		counter = 0;

		if(RXCounter == KEYAMOUNT) {
			// set keys
			// 0 is no key
			if(key0code != keybuffer[0]) {
				key0code = keybuffer[0];
				eeprom_update_byte(KEY0LOC, key0code);
			}
			if(key1code != keybuffer[1]) {
				key1code = keybuffer[1];
				eeprom_update_byte(KEY1LOC, key1code);
			}
			if(key2code != keybuffer[2]) {
				key2code = keybuffer[2];
				eeprom_update_byte(KEY2LOC, key2code);
			}
			if(key3code != keybuffer[3]) {
				key3code = keybuffer[3];
				eeprom_update_byte(KEY3LOC, key3code);
			}
			if(key4code != keybuffer[4]) {
				key4code = keybuffer[4];
				eeprom_update_byte(KEY4LOC, key4code);
			}
			if(key5code != keybuffer[5]) {
				key5code = keybuffer[5];
				eeprom_update_byte(KEY5LOC, key5code);
			}
			if(key6code != keybuffer[6]) {
				key6code = keybuffer[6];
				eeprom_update_byte(KEY6LOC, key6code);
			}
		} 
		
		RXCounter = 0;
	}
}

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize) {
	USB_KeyboardReport_Data_t* KeyboardReport = (USB_KeyboardReport_Data_t*)ReportData;

	uint8_t UsedKeyCodes = 0;

	if(!(PINB & KEY0)) KeyboardReport->KeyCode[UsedKeyCodes++] = key0code;
	if(!(PINB & KEY1)) KeyboardReport->KeyCode[UsedKeyCodes++] = key1code;
	if(!(PINE & KEY2)) KeyboardReport->KeyCode[UsedKeyCodes++] = key2code;
	if(!(PIND & KEY3)) KeyboardReport->KeyCode[UsedKeyCodes++] = key3code;
	if(!(PIND & KEY4)) KeyboardReport->KeyCode[UsedKeyCodes++] = key4code;
	if(!(PIND & KEY5)) KeyboardReport->KeyCode[UsedKeyCodes++] = key5code;
	if(!(PINC & KEY6)) KeyboardReport->KeyCode[UsedKeyCodes++] = key6code;

	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
	return true; //sends report when it hasnt changed (button held down)
}

// process HID OUT report
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize) {
//	uint8_t* LEDReport = (uint8_t*)ReportData;
/*
	if (*LEDReport & HID_KEYBOARD_LED_CAPSLOCK) {
		PORTB |= CAPSLED;
	} else {
		PORTB &= ~CAPSLED;
	}
	*/
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void){}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void){}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void) {
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
}
