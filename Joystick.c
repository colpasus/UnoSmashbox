/*
Nintendo Switch Fightstick - Proof-of-Concept

Based on the LUFA library's Low-Level Joystick Demo
	(C) Dean Camera
Based on the HORI's Pokken Tournament Pro Pad design
	(C) HORI

This project implements a modified version of HORI's Pokken Tournament Pro Pad
USB descriptors to allow for the creation of custom controllers for the
Nintendo Switch. This also works to a limited degree on the PS3.

Since System Update v3.0.0, the Nintendo Switch recognizes the Pokken
Tournament Pro Pad as a Pro Controller. Physical design limitations prevent
the Pokken Controller from functioning at the same level as the Pro
Controller. However, by default most of the descriptors are there, with the
exception of Home and Capture. Descriptor modification allows us to unlock
these buttons for our use.
*/



/* 

Switch controller:

XYAB                       4
PLUS MINUS (HOME)(CAPTURE) 4
DPAD (Up/Down/Right/Left)  4
L R LZ RZ                  4

Bottom Stick Right/Left    2

Right Stick Analog         4
Left  Stick Analog         4

Tilt                       1

                     ----------
					     27




Version 1.0 Colpasus
First working version.


*/




/** \file
 *
 *  Main source file for the posts printer demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "Joystick.h"
#include <avr/power.h>
#include "dataForController_t.h"
#include <avr/wdt.h>
 



dataForController_t dataToSend;
char buttonData1;
char buttonData2;
char buttonData3;

#define RXLED 4
#define TXLED 5

typedef enum {
	SYNC_CONTROLLER,
	SYNC_POSITION,
	STOP_X,
	STOP_Y,
	MOVE_X,
	MOVE_Y,
	DONE
} State_t;


typedef enum {
	MINUS,
	PLUS,
	HOME,
	CAPTURE,
	LCLICK,
	RCLICK
} BControl_t;






// State_t state = STOP_X; Control
State_t state = MOVE_X;

BControl_t BControl = MINUS;


#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
//int data = 0;

// Main entry point.
int main(void) {

	wdt_reset();
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// We'll start by performing hardware and peripheral setup.
	SetupHardware();
	// We'll then enable global interrupts for our use.
	GlobalInterruptEnable();
	// Once that's done, we'll enter an infinite loop.

	#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

	// Start up the USART for serial communications
	// 25 corresponds to 38400 baud - see datasheet for more values
	USART_Init(25);// 103 corresponds to 9600, 8 corresponds to 115200 baud, 3 for 250000

/////////////////////////////////////////////


	CPU_PRESCALE(0);



//	DDRD = 0x0f; //set D3 as input

//	dataForController_t dataToSend;


	state = MOVE_X; //do nothing

	BControl = MINUS;

	//state = STOP_X; //do blink



	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	// This wait also gives the Arduino bootloader time to timeout,
	//  so the serial data you'll be properly aligned.



	for (int i = 0; i < 6; ++i)
	{
		LEDoff(TXLED);
		_delay_ms(200);
		LEDon(TXLED);
		_delay_ms(200);
	}



	for (;;)
	{
		// We need to run our task to process and deliver data for our IN and OUT endpoints.
		My_HID_Task();


		// We also need to run the main USB management task.
		USB_USBTask();

		_delay_ms(1);
	}
}




// Initializes the USART to receive and transmit,
//  takes in a value you can find in the datasheet
//  based on desired communication and clock speeds
void USART_Init(uint16_t baudSetting) {
	// Set baud rate
	UBRR1 = baudSetting;
	// Enable receiver and transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	// Set frame format: 8data, 1stop bit
	UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
}







// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
	// We need to disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// We need to disable clock division before initializing the USB hardware.

	clock_prescale_set(clock_div_1);
	// We can then initialize our hardware and peripherals, including the USB stack.

	#ifdef ALERT_WHEN_DONE
	// Both PORTD and PORTB will be used for the optional LED flashing and buzzer.
	#warning LED and Buzzer functionality enabled. All pins on both PORTB and \
PORTD will toggle when printing is done.
	DDRD  = 0xFF; //Teensy uses PORTD
	PORTD =  0x0;
                  //We'll just flash all pins on both ports since the UNO R3
	DDRB  = 0xFF; //uses PORTB. Micro can use either or, but both give us 2 LEDs
	PORTB =  0x0; //The ATmega328P on the UNO will be resetting, so unplug it?
	#endif
	// The USB stack should be initialized last.
	USB_Init();
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
	// We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
	// We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	// We setup the HID report endpoints.
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

	// We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
	// We can handle two control requests: a GetReport and a SetReport.

	// Not used here, it looks like we don't receive control request from the Switch.
}




// Process and deliver data from IN and OUT endpoints.
void My_HID_Task(void) {
	// If the device isn't connected and properly configured, we can't do anything here.
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	// We'll start with the OUT endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
	// We'll check to see if we received something on the OUT endpoint.
	if (Endpoint_IsOUTReceived())
	{
		// If we did, and the packet has data, we'll react to it.
		if (Endpoint_IsReadWriteAllowed())
		{
			// We'll create a place to store our data received from the host.
			USB_JoystickReport_Output_t JoystickOutputData;
			// We'll then take in that data, setting it up in our storage.
			while (Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
			// At this point, we can react to this data.

			// However, since we're not doing anything with this data, we abandon it.
		}
		// Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
		Endpoint_ClearOUT();
	}

	// We'll then move on to the IN endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
	// We first check to see if the host is ready to accept data.
	if (Endpoint_IsINReady())
	{
		// We'll create an empty report.
		USB_JoystickReport_Input_t JoystickInputData;
		// We'll then populate this report with what we want to send to the host.
//		GetNextReport(&JoystickInputData);


		GenerateReport(&JoystickInputData);




		// Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
		while (Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) != ENDPOINT_RWSTREAM_NoError);
		// We then send an IN packet on this endpoint.
		Endpoint_ClearIN();
	}


	///// JOINING UNOJOY CODE



		// Delay so we're not going too fast

		// We get our data from the ATmega328p by writing which byte we
		//  want from the dataForController_t, and then wait for the
		//  ATmega328p to send that back to us.
		// The serialRead(number) function reads the serial port, and the
		//  number is a timeout (in ms) so if there's a transmission error,
		//  we don't stall forever.
	
//		_delay_ms(125);

		LEDon(TXLED);
		flushSerialRead();

//		_delay_ms(125);

		serialWrite(0);
		buttonData1 = serialRead(25);

		serialWrite(1);
		buttonData2 = serialRead(25);

		serialWrite(2);
		buttonData3 = serialRead(25);

		serialWrite(3);
		dataToSend.leftStickX = serialRead(25);

		serialWrite(4);
		dataToSend.leftStickY = serialRead(25);

		serialWrite(5);
		dataToSend.rightStickX = serialRead(25);

		serialWrite(6);
		dataToSend.rightStickY = serialRead(25);

		LEDoff(TXLED);

		// Now, we take the button data we got in and input that information
		//  into our controller data we want to send
		dataToSend.triangleOn = 1 & (buttonData1 >> 0);
		dataToSend.circleOn = 1 & (buttonData1 >> 1);
		dataToSend.squareOn = 1 & (buttonData1 >> 2);
		dataToSend.crossOn = 1 & (buttonData1 >> 3);
		dataToSend.l1On = 1 & (buttonData1 >> 4);
		dataToSend.l2On = 1 & (buttonData1 >> 5);
		dataToSend.l3On = 1 & (buttonData1 >> 6);
		dataToSend.r1On = 1 & (buttonData1 >> 7);

		dataToSend.r2On = 1 & (buttonData2 >> 0);
		dataToSend.r3On = 1 & (buttonData2 >> 1);
		dataToSend.MinusOn = 1 & (buttonData2 >> 2);
		dataToSend.PlusOn = 1 & (buttonData2 >> 3);
		dataToSend.HomeOn = 1 & (buttonData2 >> 4);
		dataToSend.dpadLeftOn = 1 & (buttonData2 >> 5);
		dataToSend.dpadUpOn = 1 & (buttonData2 >> 6);
		dataToSend.dpadRightOn = 1 & (buttonData2 >> 7);

		dataToSend.dpadDownOn = 1 & (buttonData3 >> 0);



}







#define ECHOES 2
int echoes = 0;
USB_JoystickReport_Input_t last_report;

int report_count = 0;
int xpos = 0;
int ypos = 0;
int portsval = 0;

// Prepare the next report for the host.

                         




// This reads the USART serial port, returning any data that's in the
//  buffer, or a guaranteed zero if it took longer than timeout ms
//  Input: uint_16 timeout - milliseconds to wait for data before timing out
unsigned char serialRead(uint16_t timeout) {
	// Wait for data to be received 
	while (!(UCSR1A & (1 << RXC1))) {
		_delay_ms(1);
		timeout--;
		if (timeout == 0) {
			return 0b0;
		}
	}
	// Get and return received data from buffer 
	return UDR1;
}

// This sends out a byte of data via the USART.
void serialWrite(unsigned char data)
{
	// Wait for empty transmit buffer
	while (!(UCSR1A & (1 << UDRE1))) {
	}
	// Put data into buffer, sends the data
	UDR1 = data;
}




void flushSerialRead()
{
	unsigned char dummy;
	while (UCSR1A & (1 << RXC1))
		dummy = UDR1;
}

// This turns on one of the LEDs hooked up to the chip
void LEDon(char ledNumber) {
	DDRD |= 1 << ledNumber;
	PORTD &= ~(1 << ledNumber);
}

// And this turns it off
void LEDoff(char ledNumber) {
	DDRD &= ~(1 << ledNumber);
	PORTD |= 1 << ledNumber;
}




void GenerateReport(USB_JoystickReport_Input_t* const ReportData) {

	// Prepare an empty report
	memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
	ReportData->LX = STICK_CENTER;
	ReportData->LY = STICK_CENTER;
	ReportData->RX = STICK_CENTER;
	ReportData->RY = STICK_CENTER;
	ReportData->HAT = HAT_CENTER;

	// Repeat ECHOES times the last report
	if (echoes > 0)
	{
		memcpy(ReportData, &last_report, sizeof(USB_JoystickReport_Input_t));
		echoes--;
		return;
	}



//	state = MOVE_X; //do nothing

	// States and moves management

/*
	switch (state)
	{
	case SYNC_CONTROLLER:
		if (report_count > 100)
		{
			report_count = 0;
			state = SYNC_POSITION;
		}
		else if (report_count == 25 || report_count == 50)
		{
			ReportData->Button |= SWITCH_L | SWITCH_R;
		}
		else if (report_count == 75 || report_count == 100)
		{
			ReportData->Button |= SWITCH_A;
		}
		report_count++;
		break;
	case SYNC_POSITION:
		if (report_count == 250)
		{
			report_count = 0;
			xpos = 0;
			ypos = 0;
			state = STOP_X;
		}
		else
		{
			// Moving faster with LX/LY
			ReportData->LX = STICK_MIN;
			ReportData->LY = STICK_MIN;
		}
		if (report_count == 75 || report_count == 150)
		{
			// Clear the screen
			ReportData->Button |= SWITCH_MINUS;
		}
		report_count++;
		break;

	case STOP_X:
		ReportData->Button |= SWITCH_A;
		ReportData->Button |= SWITCH_B;
		state = STOP_Y;
		break;
	case STOP_Y:
		ReportData->Button |= SWITCH_X;
		ReportData->Button |= SWITCH_Y;
		state = STOP_X;
		break;
	case MOVE_X:
		break;
	case MOVE_Y:
		break;


	case DONE:
#ifdef ALERT_WHEN_DONE
		portsval = ~portsval;
		PORTD = portsval; //flash LED(s) and sound buzzer if attached
		PORTB = portsval;
		_delay_ms(250);
#endif
		return;
	}

*/


/* TILTING
* 
* 
	if (dataToSend.dpadUpOn == 1) 
		if (dataToSend.dpadDownOn ==0)
			ReportData->LX = STICK_MIN;
		else
			ReportData->LX = STICK_CENTER/2;
*/

/*
	if (dataToSend.dpadUpOn == 1)    ReportData->Button |= SWITCH_MINUS;
	if (dataToSend.dpadDownOn == 1)  ReportData->Button |= SWITCH_PLUS;
	if (dataToSend.dpadRightOn == 1) ReportData->Button |= SWITCH_LCLICK;
	if (dataToSend.dpadLeftOn == 1)  ReportData->Button |= SWITCH_RCLICK;
*/



// DPAD LEFT/RIGHT/UP/DOWN

	if (dataToSend.dpadUpOn == 1)
	{
		if (dataToSend.dpadRightOn == 0 && dataToSend.dpadLeftOn == 0) ReportData->HAT = HAT_TOP;
		if (dataToSend.dpadRightOn == 1 && dataToSend.dpadLeftOn == 0) ReportData->HAT = HAT_TOP_RIGHT;
	}
	if (dataToSend.dpadRightOn == 1)
	{
		if (dataToSend.dpadDownOn == 0 && dataToSend.dpadUpOn == 0) ReportData->HAT = HAT_RIGHT;
		if (dataToSend.dpadDownOn == 1 && dataToSend.dpadUpOn == 0) ReportData->HAT = HAT_BOTTOM_RIGHT;
	}
	if (dataToSend.dpadDownOn == 1)
	{
		if (dataToSend.dpadRightOn == 0 && dataToSend.dpadLeftOn == 0) ReportData->HAT = HAT_BOTTOM;
		if (dataToSend.dpadRightOn == 0 && dataToSend.dpadLeftOn == 1) ReportData->HAT = HAT_BOTTOM_LEFT;
	}
	if (dataToSend.dpadLeftOn == 1)
	{
		if (dataToSend.dpadDownOn == 0 && dataToSend.dpadUpOn == 1) ReportData->HAT = HAT_TOP_LEFT;
		if (dataToSend.dpadDownOn == 0 && dataToSend.dpadUpOn == 0) ReportData->HAT = HAT_LEFT;
	}


// ANALOGIC STICK LEFT + TILT


	ReportData->LX = dataToSend.leftStickX;
	ReportData->LY = dataToSend.leftStickY;


// X Y A B bottoms


	if (dataToSend.crossOn == 1)         ReportData->Button |= SWITCH_B;
	if (dataToSend.circleOn == 1)        ReportData->Button |= SWITCH_A;
	if (dataToSend.squareOn == 1)        ReportData->Button |= SWITCH_Y;
	if (dataToSend.triangleOn == 1)      ReportData->Button |= SWITCH_X;



	if (dataToSend.l1On == 1) ReportData->Button |= SWITCH_L;
	if (dataToSend.r1On == 1) ReportData->Button |= SWITCH_R;

	if (dataToSend.l2On == 1) ReportData->Button |= SWITCH_ZL;
	if (dataToSend.r2On == 1) ReportData->Button |= SWITCH_ZR;



	if (dataToSend.l3On == 1)    ReportData->Button |= SWITCH_LCLICK;
	if (dataToSend.r3On == 1)  ReportData->Button |= SWITCH_RCLICK;
	if (dataToSend.MinusOn == 1) ReportData->Button |= SWITCH_MINUS;
	if (dataToSend.PlusOn == 1)  ReportData->Button |= SWITCH_PLUS;
	if (dataToSend.HomeOn == 1)  ReportData->Button |= SWITCH_HOME;

/*
	ReportData->Button |= SWITCH_LCLICK;
	ReportData->Button |= SWITCH_RCLICK;
	ReportData->Button |= SWITCH_CAPTURE;
	ReportData->Button |= SWITCH_PLUS;
	ReportData->Button |= SWITCH_MINUS;
	ReportData->Button |= SWITCH_HOME;
*/	
	
/*
	report_count++;

	ReportData->Button |= SWITCH_ZL;
	switch (BControl)
	{
	case MINUS:

		ReportData->Button |= SWITCH_MINUS;
		ReportData->Button |= SWITCH_A;

		if (report_count > 25)
		{
			report_count = 0;
			state = PLUS;
		}
		break;

	case PLUS:

		ReportData->Button |= SWITCH_PLUS;
		ReportData->Button |= SWITCH_B;

		if (report_count > 25)
		{
			report_count = 0;
			state = MINUS;
		}
		break;

	default:

		ReportData->Button |= SWITCH_ZR;
		break;

	}
*/





	// Prepare to echo this report
	memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
	echoes = ECHOES;



}



