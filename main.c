/*
 * main.c
 *
 *  Created on: 2016 Mar 15 18:42:25
 *  Author: DominikH
 */

//#define XMC_DEBUG_ENABLE
#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

/* Semihosting -specs=rdimon.specs 	*/
extern void initialise_monitor_handles(void);

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */

#define LED1 P5_9
#define LED2 P5_8
#define BUTTON1 P15_13

#define LED_INFO LED1
#define LED_ERROR LED2

#ifdef XMC_DEBUG_ENABLE
#include <stdio.h>
#define XMC_DEBUG(...) { printf(__VA_ARGS__); }
#else
#define XMC_DEBUG(...) { ; }
#endif

#define info(...) { \
		XMC_DEBUG("Info: %s: %d: %s: ", __FILE__, __LINE__, __FUNCTION__);\
		XMC_DEBUG(__VA_ARGS__);\
		XMC_DEBUG("\n")\
	}

#define error(...) { \
		XMC_DEBUG("Error: %s: %d: %s: ", __FILE__, __LINE__, __FUNCTION__);\
		XMC_DEBUG(__VA_ARGS__);\
		XMC_DEBUG("\n")\
		_error();\
	}

void _error()
{
	XMC_GPIO_SetOutputHigh(LED_ERROR);
	while (1 == 1)
		;
}

#define assert(x) { if (!(x)) error(#x); }

#define USBD_VCOM_BUFFSIZE (256)

int8_t usb_rx_buffer[USBD_VCOM_BUFFSIZE] =
{ 0 };

#define messageObjectsMax (32)

const CAN_NODE_LMO_t * messageObjects[32] =
{ NULL, &CAN_NODE_0_LMO_01_Config, &CAN_NODE_0_LMO_02_Config,
		&CAN_NODE_0_LMO_03_Config, &CAN_NODE_0_LMO_04_Config,
		&CAN_NODE_0_LMO_05_Config, &CAN_NODE_0_LMO_06_Config,
		&CAN_NODE_0_LMO_07_Config, &CAN_NODE_0_LMO_08_Config,
		&CAN_NODE_0_LMO_09_Config, &CAN_NODE_0_LMO_10_Config,
		&CAN_NODE_0_LMO_11_Config, &CAN_NODE_0_LMO_12_Config,
		&CAN_NODE_0_LMO_13_Config, &CAN_NODE_0_LMO_14_Config,
		&CAN_NODE_0_LMO_15_Config, &CAN_NODE_0_LMO_16_Config,
		&CAN_NODE_0_LMO_17_Config, /* &CAN_NODE_0_LMO_18_Config,*/
		&CAN_NODE_0_LMO_19_Config, &CAN_NODE_0_LMO_20_Config,
		&CAN_NODE_0_LMO_21_Config, &CAN_NODE_0_LMO_22_Config,
		&CAN_NODE_0_LMO_23_Config, &CAN_NODE_0_LMO_24_Config,
		&CAN_NODE_0_LMO_25_Config, &CAN_NODE_0_LMO_26_Config,
		&CAN_NODE_0_LMO_27_Config, &CAN_NODE_0_LMO_28_Config,
		&CAN_NODE_0_LMO_29_Config, &CAN_NODE_0_LMO_30_Config,
		&CAN_NODE_0_LMO_31_Config, &CAN_NODE_0_LMO_32_Config };

XMC_CAN_MO_t *ReceivedMsg = NULL;

#define S(s) ((int8_t*)(s))

const char CommandSeperator[] = " ";

typedef enum echoEnabled
{
	Echo_Off, Echo_On
} echoEnabled_t;

echoEnabled_t echoEnabled = Echo_Off;

typedef enum USB_CAN_STATUS
{
	USB_CAN_STATUS_SUCCESS = 0U, USB_CAN_STATUS_FAILURE
} USB_CAN_STATUS_t;

typedef union can_data
{
	uint8_t b[8];
	uint32_t i[2];
} can_data_t;

typedef enum condition
{
	Condition_Passive, Condition_Active
} condition_t;

typedef struct trigger
{
	uint32_t can_identifier;
	/* bit numbering in can frame
	 *  7  6  5  4  3  2  1  0
	 * 15 14 13 12 11 10  9  8
	 * 23 22 21 20 19 18 17 16
	 * 31 30 29 28 27 26 25 24
	 * 39 38 37 36 35 34 33 32
	 * 47 46 45 44 43 42 41 40
	 * 55 54 53 52 51 50 49 48
	 * 63 62 61 60 59 58 57 56
	 */
	uint8_t can_bit;
	bool is_valid;
} trigger_t;

typedef enum
{
	SendCommand_Kind_CanMsg, SendCommand_Kind_Surge
} SendCommand_Kind_t;

typedef struct SendCommand
{
	condition_t condition;
	SendCommand_Kind_t kind;
	trigger_t trigger;
	uint32_t can_identifier;
	int32_t count;
	uint8_t can_data_length;
	can_data_t can_data;

} SendCommand_t;

SendCommand_t SendCommand =
{ .condition = Condition_Passive, .kind = SendCommand_Kind_CanMsg, .trigger =
{ .can_identifier = 0x0, .can_bit = 56, .is_valid = true }, .can_identifier =
		0x400, .can_data_length = 8, .count = -1 };

int is_printable(int8_t data_byte)
{
	return (31 < data_byte) && (data_byte < 126);
}

USB_CAN_STATUS_t USB_CAN_printf(const char *format, ...)
{
	char outPutString[USBD_VCOM_BUFFSIZE];
	USB_CAN_STATUS_t result;
	va_list args;
	va_start(args, format);

	vsnprintf(outPutString, USBD_VCOM_BUFFSIZE, format, args);
	result = USBD_VCOM_SendString(S(outPutString));

	va_end(args);
	return result;
}

USB_CAN_STATUS_t printCanMsg(XMC_CAN_MO_t* canMessageObject)
{
	uint8_t length;

	if (canMessageObject == NULL)
	{
		return USB_CAN_STATUS_FAILURE;
	}

	length = canMessageObject->can_data_length;

	USB_CAN_printf("\r\nReceived 0x%0x %u", canMessageObject->can_identifier,
			length);

	for (uint8_t i = 0; i < length; i++)
	{
		USB_CAN_printf(" 0x%0x", canMessageObject->can_data_byte[i]);
	}

	USB_CAN_printf("\r\n");

	return USB_CAN_STATUS_SUCCESS;
}

USB_CAN_STATUS_t consumeCanMsg(XMC_CAN_MO_t* canMessageObject)
{
	if (canMessageObject == NULL)
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if (echoEnabled == Echo_On)
	{
		return printCanMsg(canMessageObject);
	}
	else
	{
		return USB_CAN_STATUS_SUCCESS;
	}
}

USB_CAN_STATUS_t executeAnalyserCommand(XMC_CAN_NODE_t *const node_ptr, bool enable)
{
	if ( enable == true)
	{
		XMC_CAN_NODE_SetInitBit(node_ptr);
		XMC_CAN_NODE_SetAnalyzerMode(node_ptr);
		XMC_CAN_NODE_ResetInitBit(node_ptr);
	}
	else
	{
		XMC_CAN_NODE_SetInitBit(node_ptr);
		XMC_CAN_NODE_ReSetAnalyzerMode(node_ptr);
		XMC_CAN_NODE_ResetInitBit(node_ptr);
	}
}

uint32_t XMC_CAN_MO_Transmission_Ongoing(const XMC_CAN_MO_t * const mo_ptr)
{
	uint32_t status = XMC_CAN_MO_GetStatus(mo_ptr);

	return ((status) & CAN_MO_MOSTAT_TXRQ_Msk) >> CAN_MO_MOSTAT_TXRQ_Pos;
}

XMC_CAN_STATUS_t sendCanMsg(uint32_t can_identifier, uint8_t can_data_length,
		uint32_t can_data[2])
{
	XMC_CAN_MO_t* mo_ptr = CAN_NODE_0_LMO_02_Config.mo_ptr;
	XMC_CAN_STATUS_t result;

	/* Do not touch MO during transmission */
	if (XMC_CAN_MO_Transmission_Ongoing(mo_ptr) == 1U)
	{
		return XMC_CAN_STATUS_BUSY;
	}

	XMC_CAN_MO_SetIdentifier(mo_ptr, can_identifier);

	mo_ptr->can_data[0] = can_data[0];
	mo_ptr->can_data[1] = can_data[1];
	mo_ptr->can_data_length = can_data_length;
	XMC_CAN_MO_UpdateData(mo_ptr);

	XMC_GPIO_SetOutputHigh(LED_INFO);
	result = XMC_CAN_MO_Transmit(mo_ptr);
	XMC_GPIO_SetOutputLow(LED_INFO);

	return result;
}

USB_CAN_STATUS_t executeSurgeCommand()
{
	USB_CAN_STATUS_t result = USB_CAN_STATUS_SUCCESS;
	XMC_GPIO_SetOutputHigh(LED_INFO);
	for (int messageObject = 3; messageObject < messageObjectsMax;
			messageObject++)
	{
		result |= CAN_NODE_MO_Transmit(messageObjects[messageObject]);
	}
	XMC_GPIO_SetOutputLow(LED_INFO);
	return result;
}

USB_CAN_STATUS_t executeTriggertSendCommand(SendCommand_t *sendCommand)
{
	XMC_CAN_STATUS_t result;

	if (sendCommand == NULL)
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if (sendCommand->kind == SendCommand_Kind_CanMsg)
	{
		result = sendCanMsg(sendCommand->can_identifier,
				sendCommand->can_data_length, sendCommand->can_data.i);

		if ((result == XMC_CAN_STATUS_SUCCESS))
		{
			if (sendCommand->count > 0)
			{
				sendCommand->count--;
			}

			if (sendCommand->count == 0)
			{
				sendCommand->condition = Condition_Passive;
			}

			return USB_CAN_STATUS_SUCCESS;
		}
		else if (result == XMC_CAN_STATUS_BUSY)
		{
			return USB_CAN_STATUS_SUCCESS;
		}
		else
		{
			return USB_CAN_STATUS_FAILURE;
		}
	}
	else if (sendCommand->kind == SendCommand_Kind_Surge)
	{
		if (sendCommand->count > 0)
		{
			sendCommand->count--;
		}

		if (sendCommand->count == 0)
		{
			sendCommand->condition = Condition_Passive;
		}

		return executeSurgeCommand();
	}
	else
	{
		return USB_CAN_STATUS_FAILURE;
	}
}

USB_CAN_STATUS_t executeSendCommand(SendCommand_t *sendCommand)
{
	if (sendCommand == NULL)
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if (sendCommand->condition == Condition_Active)
	{
		return executeTriggertSendCommand(sendCommand);
	}

	return USB_CAN_STATUS_SUCCESS;
}

USB_CAN_STATUS_t processSendCommand(char* commandLine)
{
	uint32_t can_identifier = 0x400;
	uint8_t can_data_length = 0;
	can_data_t can_data;
	trigger_t trigger;
	SendCommand_Kind_t kind = SendCommand_Kind_CanMsg;

	char *token;

	condition_t condition;
	int32_t count;

	if (commandLine == NULL)
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if ((token = strsep(&commandLine, CommandSeperator)) != NULL)
	{
		trigger.is_valid = false;
		if (!strcmp(token, "active"))
		{
			condition = Condition_Active;
		}
		else if (!strcmp(token, "passive"))
		{
			condition = Condition_Passive;
		}
		else
		{
			char* innerToken = NULL;
			if ((innerToken = strsep(&token, ":")) != NULL)
			{
				trigger.can_identifier = strtoul(innerToken, NULL, 0);
			}
			else
			{
				return USB_CAN_STATUS_FAILURE;
			}
			if ((innerToken = strsep(&token, ":")) != NULL)
			{
				trigger.can_bit = (uint8_t) strtoul(innerToken, NULL, 0);
			}
			else
			{
				return USB_CAN_STATUS_FAILURE;
			}
			condition = Condition_Passive;
			trigger.is_valid = true;

		}
	}
	else
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if ((token = strsep(&commandLine, CommandSeperator)) != NULL)
	{
		if (!strcmp(token, "surge"))
		{
			SendCommand.condition = Condition_Passive;
			SendCommand.trigger = trigger;
			SendCommand.kind = SendCommand_Kind_Surge;
			SendCommand.count = 1;
			SendCommand.condition = condition;
			return USB_CAN_STATUS_SUCCESS;
		}
		else
		{
			count = strtol(token, NULL, 0);
		}
	}
	else
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if ((token = strsep(&commandLine, CommandSeperator)) != NULL)
	{
		can_identifier = strtoul(token, NULL, 0);
	}
	else
	{
		return USB_CAN_STATUS_FAILURE;
	}

	for (int i = 0; i < 8; i++)
	{
		if ((token = strsep(&commandLine, CommandSeperator)) != NULL)
		{
			can_data.b[i] = strtoul(token, NULL, 0);
			can_data_length++;
		}
	}

	SendCommand.condition = Condition_Passive;
	SendCommand.trigger = trigger;
	SendCommand.kind = kind;
	SendCommand.count = count;
	SendCommand.can_identifier = can_identifier;
	SendCommand.can_data_length = can_data_length;
	SendCommand.can_data = can_data;
	SendCommand.condition = condition;

	return USB_CAN_STATUS_SUCCESS;
}

USB_CAN_STATUS_t processSurgeCommand()
{
	return executeSurgeCommand();
}

USB_CAN_STATUS_t processEchoCommand(char* commandLine)
{
	if (!strcmp(commandLine, "on"))
	{
		echoEnabled = Echo_On;
	}
	else
	{
		echoEnabled = Echo_Off;
	}

	return USB_CAN_STATUS_SUCCESS;
}


USB_CAN_STATUS_t processAnalyzerCommand(char* commandLine)
{
	XMC_CAN_NODE_t *const node_ptr = CAN_NODE_0.node_ptr;
	bool enable = false;

	if (!strcmp(commandLine, "on"))
	{
		enable = true;
	}

	return executeAnalyserCommand(node_ptr, enable);
}

USB_CAN_STATUS_t processCommand(int8_t usb_rx_buffer[])
{
	USB_CAN_STATUS_t result = USB_CAN_STATUS_FAILURE;
	char* commandLine = (char*) usb_rx_buffer;
	const char* cmd = NULL;
	char *token;

	if (usb_rx_buffer == NULL)
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if ((token = strsep(&commandLine, CommandSeperator)) != NULL)
	{
		cmd = token;
		if (!strcmp(cmd, "Send"))
		{
			result = processSendCommand(commandLine);
		}
		else if (!strcmp(cmd, "Echo"))
		{
			result = processEchoCommand(commandLine);
		}
		else if (!strcmp(cmd, "Surge"))
		{
			result = processSurgeCommand();
		}
		else if (!strcmp(cmd, "Analyzer"))
		{
			result = processAnalyzerCommand(commandLine);
		}
		else
		{
			USB_CAN_printf("\r\nUnknown command '%s'\r\n", cmd);
		}

		if (result == USB_CAN_STATUS_SUCCESS)
		{
			USB_CAN_printf("\r\nResult: Success\r\n");
		}
		else
		{
			USB_CAN_printf("\r\nResult: Failure\r\n");
		}
	}
	return result;
}

USBD_VCOM_STATUS_t USBD_VCOM_SendPromt(int8_t usb_rx_buffer[])
{
	return USB_CAN_printf("\r> %s", usb_rx_buffer);
}

USBD_VCOM_STATUS_t processTrigger(XMC_CAN_MO_t* message,
		SendCommand_t *sendCommand)
{
	if (message == NULL)
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if (sendCommand == NULL)
	{
		return USB_CAN_STATUS_FAILURE;
	}

	if (sendCommand->trigger.is_valid)
	{
		if (sendCommand->trigger.can_identifier == message->can_identifier)
		{
			int bitNumber = sendCommand->trigger.can_bit % 8;
			int byteNumber = (sendCommand->trigger.can_bit % 63) / 8;
			if ((message->can_data_byte[byteNumber] >> bitNumber) & 0x1)
			{
				sendCommand->condition = Condition_Active;
			}
			else
			{
				sendCommand->condition = Condition_Passive;
			}
		}
	}

	return USB_CAN_STATUS_SUCCESS;
}

int main(void)
{
	const uint32_t Button_NotPressed = 1;
	uint32_t Button1OldValue = Button_NotPressed;

#ifdef XMC_DEBUG_ENABLE
	/* TODO check influence on USBD_VCOM */
	initialise_monitor_handles();
#endif

	info("Initializing ...");

	assert(DAVE_Init() != DAVE_STATUS_FAILURE);

	// Set LED pins to push-pull
	XMC_GPIO_SetMode(LED_INFO, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetMode(LED_ERROR, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_EnableDigitalInput(BUTTON1);

	// Switch off LEDs
	XMC_GPIO_SetOutputLow(LED_INFO);
	XMC_GPIO_SetOutputLow(LED_ERROR);

	assert(USBD_VCOM_Connect() == USBD_VCOM_STATUS_SUCCESS)

	XMC_GPIO_SetOutputHigh(LED_ERROR);
	while (!USBD_VCOM_IsEnumDone())
		;
	XMC_GPIO_SetOutputLow(LED_ERROR);

	info("Initializing ... done.");

	USBD_VCOM_SendPromt(usb_rx_buffer);

	int usb_rx_buffer_idx = 0;
	while (1U)
	{
		/* consume only last received message, discard other received messages */
		if (ReceivedMsg != NULL)
		{
			XMC_CAN_MO_t* tmp = ReceivedMsg;
			ReceivedMsg = NULL;

			consumeCanMsg(tmp);
		}

		if (executeSendCommand(&SendCommand) != USB_CAN_STATUS_SUCCESS)
		{
			USB_CAN_printf("\r\nResult: Failure sending CAN\r\n");
		}

		{
			uint32_t Button1Value = XMC_GPIO_GetInput(BUTTON1);

			/* react on edge */
			if ((Button1Value != Button_NotPressed)
					&& (Button1Value != Button1OldValue))

			{
				if (SendCommand.condition != Condition_Active)
				{
					SendCommand.condition = Condition_Active;
					USB_CAN_printf("\r\nActivation by button\r\n");
				}
				else
				{
					SendCommand.condition = Condition_Passive;
					USB_CAN_printf("\r\nDeactivation by button\r\n");
				}
			}
			Button1OldValue = Button1Value;
		}

		{
			uint16_t bytesReceived = USBD_VCOM_BytesReceived();

			if (bytesReceived)
			{
				USBD_VCOM_STATUS_t status;
				int8_t data_byte = 0;
				const int8_t newLine = '\r';
				status = USBD_VCOM_ReceiveByte(&data_byte);
				if (status == USBD_VCOM_STATUS_SUCCESS)
				{
					if (data_byte == newLine)
					{
						processCommand(usb_rx_buffer);

						usb_rx_buffer_idx = 0;
						memset(usb_rx_buffer, 0, USBD_VCOM_BUFFSIZE);
					}
					else if (data_byte == '\177')
					{
						if (usb_rx_buffer_idx > 0)
						{
							usb_rx_buffer_idx--;
							usb_rx_buffer[usb_rx_buffer_idx] = ' ';
							USBD_VCOM_SendPromt(usb_rx_buffer);
							usb_rx_buffer[usb_rx_buffer_idx] = 0;
						}
					}
					else if (is_printable(data_byte)
							&& (usb_rx_buffer_idx < (USBD_VCOM_BUFFSIZE - 1)))
					{
						usb_rx_buffer[usb_rx_buffer_idx++] = data_byte;
					}
					USBD_VCOM_SendPromt(usb_rx_buffer);
				}
			}

			/* Do not spend time in USBTask during flooding the CAN */
			if (SendCommand.condition != Condition_Active)
			{
				CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
			}
		}
	}

	error("never reach here");
}

void CanRxInterruptHandler(void)
{
	/* Check for Node error */
	if (CAN_NODE_GetStatus(&CAN_NODE_0) & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
	{
		XMC_DEBUG("Something failed\n");
	}
	else if (CAN_NODE_MO_GetStatus(&CAN_NODE_0_LMO_01_Config)
			& XMC_CAN_MO_STATUS_RX_PENDING)
	{
		/* Read the received Message object and stores in Request_Node_LMO_02_Config*/
		CAN_NODE_MO_Receive(&CAN_NODE_0_LMO_01_Config);
		ReceivedMsg = CAN_NODE_0_LMO_01_Config.mo_ptr;
		processTrigger(CAN_NODE_0_LMO_01_Config.mo_ptr, &SendCommand);
	}
}
