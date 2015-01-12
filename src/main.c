/**
 * @file main.c
 * @brief Transmit received IR via USB
 *
 * Program which receives NEC IR commands and transmits
 * them to host device via USB HID interface
 *
 * Adapted from signal11's m-stack main.c
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#include "usb.h"
#include <xc.h>
#include <string.h>
#include <stdint.h>
#include "usb_config.h"
#include "usb_ch9.h"
#include "usb_hid.h"

#include <stdio.h>

#include <system.h>
#include <uart.h>
#include <irq.h>

#include "ir.h"
#include "timer.h"

void uart_dump_uint16_t(uint16_t val)
{
	char uart_buf[6];
	uart_buf[4] = (val/1)%10 +'0';
	uart_buf[3] = (val/10)%10 +'0';
	uart_buf[2] = (val/100)%10 +'0';
	uart_buf[1] = (val/1000)%10 +'0';
	uart_buf[0] = (val/10000)%10 +'0';
	uart_buf[5] = '\0';
	uart_puts(uart_buf);
}


#ifdef MULTI_CLASS_DEVICE
static uint8_t hid_interfaces[] = { 0 };
#endif

void print_uint8_t (uint8_t dVal)
{
	char buffer[10];
	uint16_t data = (uint16_t) dVal;
	uart_dump_uint16_t (data);
}

int main(void)
{
	uint8_t i;
	system_init();
	uart_init();

	// set Pin A5 to input mode and then read it to update Interrupt on Change latch
	TRISA5 = 1;
	//volatile uint8_t ra5 = RA5;

	TRISC2 = 0;
	TRISC3 = 0;
	TRISA4 = 0;

	// give clock/uart time to stabilise
	for (i=0; i<255; i++) {
	}

	// build interrupt table
	irq_init();
	irq_register_handler(IRQ_USB, usb_service);
	irq_register_handler(IRQ_INT_ON_CHANGE, ir_isr);

#if defined(_16F1454) || defined(_16F1455) || defined(_16F1459)
	// Enable Active clock-tuning from the USB
	ACTCONbits.ACTSRC = 1; // 1=USB
	ACTCONbits.ACTEN = 1;
#endif

	//Interrupts were doing weird things until I set this. No idea why :S
	INTCON = 0x00;

	timer_init();
	ir_init();

// Configure interrupts, per architecture
#if defined (_PIC18) || defined(_PIC14E)
	INTCONbits.IOCIE = 1;
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;
#endif

#ifdef MULTI_CLASS_DEVICE
	hid_set_interface_list(hid_interfaces, sizeof(hid_interfaces));
#endif

	usb_init();

	// give everything time to chill
	for (i=0; i<255; i++) {
	}

	/* Setup mouse movement. This implementation sends back data for every
	 * IN packet, but sends no movement for all but every delay-th frame.
	 * Adjusting delay will slow down or speed up the movement, which is
	 * also dependent upon the rate at which the host sends IN packets,
	 * which varies between implementations.
	 *
	 * In real life, you wouldn't want to send back data that hadn't
	 * changed, but since there's no real hardware to poll, and since this
	 * example is about showing the HID class, and not about creative ways
	 * to do timing, we send back data every frame. The interested reader
	 * may want to modify it to use the start-of-frame callback for
	 * timing.
	 */
	uint8_t x_count = 100;
	uint8_t delay = 7;
	int8_t x_direc = 1;

	while (1) {
		// NOTE: Code in here will not run until USB is plugged in!
		if (usb_is_configured() &&
		    !usb_in_endpoint_halted(1) &&
		    !usb_in_endpoint_busy(1)) {

			if (flag) {
				uart_putc(bit_data[0]);
				uart_putc(bit_data[1]);
				uart_putc(bit_data[2]);
				uart_putc(bit_data[3]);

				bit_data_len = 0;
				flag = 0;
			}

			/*if (ir_buffer_swapped()) {
				uart_puts("IR Data: ");
				for (uint8_t i=0; i<ir_process_buf->len; i++) {
					//process the data
					uart_putc(ir_process_buf->buf[i]);
				}
				uart_puts("\r\n");

				//request a swap
				ir_buffer_request_swap();
			}*/

//			unsigned char *buf = usb_get_in_buffer(1);
			struct remote_buf_t* remote_buf = (struct remote_buf_t*) usb_get_in_buffer(1);
			unsigned char *buf = (unsigned char *) remote_buf;
			memset(remote_buf, 0, 2);
			// buf[0-2] = mouse
			// buf[3] = keypad and volume/chan control
			// buf[4] = mute/etc, buttons
			--delay;
			//buf[0] = (--delay)? 0: x_direc;

			if (delay <= 0) {
				if (--x_count == 0) {
					x_count = 100;
					//x_direc *= -1;
					// volume up/down
					//buf[3] = (x_direc==1)?(1<<2):(3<<2);
					// mute toggle
					//buf[3] = 0x04;
					buf[1] = 1;
				}
				delay = 7;
			}

			usb_send_in_buffer(1, 2);
		}

		#ifndef USB_USE_INTERRUPTS
		usb_service();
		#endif
	}

	return 0;
}

/* Callbacks. These function names are set in usb_config.h. */
void app_set_configuration_callback(uint8_t configuration)
{

}

uint16_t app_get_device_status_callback()
{
	return 0x0000;
}

void app_endpoint_halt_callback(uint8_t endpoint, bool halted)
{

}

int8_t app_set_interface_callback(uint8_t interface, uint8_t alt_setting)
{
	return 0;
}

int8_t app_get_interface_callback(uint8_t interface)
{
	return 0;
}

void app_out_transaction_callback(uint8_t endpoint)
{

}

void app_in_transaction_complete_callback(uint8_t endpoint)
{

}

int8_t app_unknown_setup_request_callback(const struct setup_packet *setup)
{
	/* To use the HID device class, have a handler for unknown setup
	 * requests and call process_hid_setup_request() (as shown here),
	 * which will check if the setup request is HID-related, and will
	 * call the HID application callbacks defined in usb_hid.h. For
	 * composite devices containing other device classes, make sure
	 * MULTI_CLASS_DEVICE is defined in usb_config.h and call all
	 * appropriate device class setup request functions here.
	 */
	return process_hid_setup_request(setup);
}

int16_t app_unknown_get_descriptor_callback(const struct setup_packet *pkt, const void **descriptor)
{
	return -1;
}

void app_start_of_frame_callback(void)
{

}

void app_usb_reset_callback(void)
{

}

/* HID Callbacks. See usb_hid.h for documentation. */

static uint8_t report_buf[3];

static void get_report_callback(bool transfer_ok, void *context)
{
	/* Nothing to do here really. It either succeeded or failed. If it
	 * failed, the host will ask for it again. It's nice to be on the
	 * device side in USB. */
}

int16_t app_get_report_callback(uint8_t interface, uint8_t report_type,
                                uint8_t report_id, const void **report,
                                usb_ep0_data_stage_callback *callback,
                                void **context)
{
	/* This isn't a composite device, so there's no need to check the
	 * interface here. Also, we know that there's only one report for
	 * this device, so there's no need to check report_type or report_id.
	 *
	 * Set report, callback, and context; and the USB stack will send
	 * the report, calling our callback (get_report_callback()) when
	 * it has finished.
	 */
	*report = report_buf;
	*callback = get_report_callback;
	*context = NULL;
	return sizeof(report_buf);
}

int8_t app_set_report_callback(uint8_t interface, uint8_t report_type,
                               uint8_t report_id)
{
	/* To handle Set_Report, call usb_start_receive_ep0_data_stage()
	 * here. See the documentation for HID_SET_REPORT_CALLBACK() in
	 * usb_hid.h. For this device though, there are no output or
	 * feature reports. */
	return -1;
}

uint8_t app_get_idle_callback(uint8_t interface, uint8_t report_id)
{
	return 0;
}

int8_t app_set_idle_callback(uint8_t interface, uint8_t report_id,
                             uint8_t idle_rate)
{
	return -1;
}

int8_t app_get_protocol_callback(uint8_t interface)
{
	return 1;
}

int8_t app_set_protocol_callback(uint8_t interface, uint8_t report_id)
{
	return -1;
}
