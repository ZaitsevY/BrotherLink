#include "user_config.h"
#ifdef USE_TCP2UART
#include "c_types.h"
#include "os_type.h"
#include "tcp2uart.h"
#include "user_interface.h"
#include "bios/gpio.h"
#include "hw/esp8266.h"
#include "phy/phy.h"
#include "web_iohw.h"

//===============================================================================
// Application: Data -> TCP
// Передать данные текущему TCP соединению.
// Позволяет программно дополнить поток данных от UART.
// Внимание: в текущей реализации отсылка идет в обход
// буфера UART напрямую в LwIP стэк, т.е. нарушает
// целостность потока данных от UART (которая, собственно,
// и так нарушена наличием этого вызова).
// Нельзя! вызывать из обработчиков прерываний напрямую.
//-------------------------------------------------------------------------------
err_t tcp2uart_write(uint8 *pblk, uint16 len)
{
	uint8* buffer = uart_drv.uart_rx_buf;
	if (buffer == NULL) return ERR_CONN;

	// Going to touch buffers usually filled by UART
	// interrupt handler: disable interrupts temporary.
	ETS_INTR_LOCK();

	// Number of bytes in buffer so far
	uint32 length = uart_drv.uart_rx_buf_count;
	if (length >= UART_RX_BUF_MAX
			|| uart_drv.uart_nsnd_buf_count >= UART_RX_BUF_MAX
			|| len > UART_RX_BUF_MAX - length) {
		// No space in buffer
		ETS_INTR_UNLOCK();
		return ERR_INPROGRESS;
	}

	// Copy data into buffer
	while (length < UART_RX_BUF_MAX && len--)
		uart_drv.uart_rx_buf[length++] = *pblk++;
	uart_drv.uart_rx_buf_count = length;

	// Number of non-delivered bytes
	length -= uart_drv.uart_out_buf_count;
	if (length) {
		// Request TCP2UART to deliver to TCP peer
		system_os_post(UART_TASK_PRIO, UART_RX_CHARS, length);
		uart_drv.uart_nsnd_buf_count += length;
		uart_drv.uart_out_buf_count = uart_drv.uart_rx_buf_count;
	}

	// Finished
	ETS_INTR_UNLOCK();
	return ERR_OK;
}

#define BUTTON_GPIO 4	// GPIO4
#define BUTTON_TASK_PRIO (USER_TASK_PRIO_1)
#define BUTTON_TASK_QUEUE_LEN 2
static const uint32 BUTTON_CLICK_DELAY = 400*1000;	// Next click not earlier than after, ms
static const GPIO_INT_TYPE BUTTON_INTR = GPIO_PIN_INTR_NEGEDGE;

os_event_t _queue[BUTTON_TASK_QUEUE_LEN] DATA_IRAM_ATTR;
uint32 _click DATA_IRAM_ATTR = 0;
uint32 _count DATA_IRAM_ATTR = 0;

/**
 * GPIO interrupt handler.
 */
static void intr_brother_link(void) {
	// Read GPIO status
	uint32 gpio_status = GPIO_STATUS;
	// Acknowledge GPIO interrupt
	GPIO_STATUS_W1TC = gpio_status;
	// Is is from mine reed switch?
	if (gpio_status & (1 << BUTTON_GPIO)) {
		// Schedule task execution
		system_os_post(BUTTON_TASK_PRIO, 0, 0);
		// Call me again next time
	    gpio_pin_intr_state_set(BUTTON_GPIO, BUTTON_INTR);
	}
}

/**
 * Reed switch task handler.
 * Send "click" to DesignaKnit.
 */
static void ICACHE_FLASH_ATTR task_brother_link(os_event_t *e)
{
	uint32 clock = system_get_time();
	if (clock < _click) {
		// System time overflow
		_click = 0;
	}
	if (e->par || clock >= _click + BUTTON_CLICK_DELAY) {	// Forced? or time to click?
		uint8 letter = (_count % 10) + '0';		// Actually just any symbol works
		if (ERR_INPROGRESS == tcp2uart_write(&letter, sizeof(letter))) {
			// Could not send, retry again later (forced)
			system_os_post(BUTTON_TASK_PRIO, 0, 1);
		} else {
			// Fine, just remember that shiny moment
			_click = clock;
			_count++;
		}
	}
}

/**
 * BrotherLink initialization.
 */
void ICACHE_FLASH_ATTR init_brother_link(void)
{
 	os_printf("\nBrotherLink...\n");

 	// Store initialization time
	_click = system_get_time();

	// Register event queue
	system_os_task(task_brother_link, BUTTON_TASK_PRIO, _queue, BUTTON_TASK_QUEUE_LEN);

	// Configure GPIO for reed switch
	uint32 pins_mask = (1 << BUTTON_GPIO);
	ETS_GPIO_INTR_DISABLE();
	gpio_output_set(0, 0, 0, pins_mask);	// configure pin for input
	set_gpiox_mux_func_ioport(BUTTON_GPIO);	// set pin function to GPIO
	ETS_GPIO_INTR_ATTACH(intr_brother_link, NULL);	// attach interrupt handler
	gpio_pin_intr_state_set(BUTTON_GPIO, BUTTON_INTR);	// to fire on condition
	GPIO_STATUS_W1TC = pins_mask;			// clear any GPIO interrupts
	ETS_GPIO_INTR_ENABLE();
}

/**
 * Custom initialization sequence implementation.
 */
void ICACHE_FLASH_ATTR custom_init() {
	init_brother_link();
}

#endif
