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
	TCP_SERV_CONN *conn = tcp2uart_conn;
	if (conn == NULL) return ERR_CONN;

	// Нельзя так вызывать, ибо там указатель на буфер
	// после отправки переставляется на конец этого pblk,
	// который, по результату, пальцем тычет в анус негра.
	// return tcpsrv_int_sent_data(conn, pblk, len);

	// Надо: засунуть данные в буфер отправки соединения pbufo
	// и уж тогда и вызывать tcpsrv_int_sent_data - списать с
	// tcp2uart.c:loading_rx_buf(). Во гемор-то...

	ETS_UART_INTR_DISABLE();
	conn->flag.user_flg1 = 1;

	err_t result;
	if (conn->flag.wait_sent) {
		// LwIP занят отсылкой, буфер не трогать
		result = ERR_INPROGRESS;
	} else {
		uint16 size = len;
		uint8 *pend = conn->pbufo + conn->sizeo;
		while((conn->ptrtx + conn->cntro) < pend && size) {
			conn->ptrtx[conn->cntro++] = *pblk;
			size--;  pblk++;
		}
		result = tcpsrv_int_sent_data(conn, conn->pbufo, conn->cntro);
		if (result == ERR_OK) {
			// удалить из буфера переданные данные
			if(conn->ptrtx != conn->pbufo && conn->cntro != 0)
				os_memcpy(conn->pbufo, conn->ptrtx, conn->cntro);
			conn->ptrtx = conn->pbufo; // указатель на начало не переданных данных (начало буфера)
		}
	}

	conn->flag.user_flg1 = 0;
	ETS_UART_INTR_ENABLE();

	return result;
}

#define BUTTON_GPIO 4	// GPIO4
#define BUTTON_TASK_PRIO (USER_TASK_PRIO_1)
#define BUTTON_TASK_QUEUE_LEN 2
const uint32 BUTTON_CLICK_DELAY = 200*1000;	// Next click not earlier than after, ms
const GPIO_INT_TYPE BUTTON_INTR = GPIO_PIN_INTR_POSEDGE;

os_event_t _queue[BUTTON_TASK_QUEUE_LEN] DATA_IRAM_ATTR;
uint32 _click DATA_IRAM_ATTR = 0;
uint32 _count DATA_IRAM_ATTR = 0;

static void intr_brother_link(void) {
	uint32 gpio_status = GPIO_STATUS;
	GPIO_STATUS_W1TC = gpio_status;
	if (gpio_status & (1 << BUTTON_GPIO)) {
		system_os_post(BUTTON_TASK_PRIO, 0, 0);
	    gpio_pin_intr_state_set(BUTTON_GPIO, BUTTON_INTR);
	}
}

static void ICACHE_FLASH_ATTR task_brother_link(os_event_t *e)
{
	uint32 clock = system_get_time();
	//	if (clock < _click) {
	//		// System time overflow?
	//		_click = 0;
	//	}
	if (e->par || clock >= _click) {	// Forced? or time to click?
		_click = clock + BUTTON_CLICK_DELAY;	// Next click not sooner
		uint8 letter = (_count % 10) + '0';
		if (ERR_INPROGRESS == tcp2uart_write(&letter, sizeof(letter)))
			system_os_post(BUTTON_TASK_PRIO, 0, 1);
		else
			_count++;
	}
}

void ICACHE_FLASH_ATTR init_brother_link(void)
{
 	os_printf("Brother link init...\n");
	_click = system_get_time();
	system_os_task(task_brother_link, BUTTON_TASK_PRIO, _queue, BUTTON_TASK_QUEUE_LEN);
	uint32 pins_mask = (1 << BUTTON_GPIO);
	gpio_output_set(0, 0, 0, pins_mask);	// настроить GPIOx на ввод
	set_gpiox_mux_func_ioport(BUTTON_GPIO);	// установить функцию GPIOx в режим порта i/o
	ETS_GPIO_INTR_ATTACH(intr_brother_link, NULL);
	gpio_pin_intr_state_set(BUTTON_GPIO, BUTTON_INTR);
	// разрешить прерывания GPIOs
	GPIO_STATUS_W1TC = pins_mask;
	ETS_GPIO_INTR_ENABLE();
}

// Custom initialization sequence implementation
void ICACHE_FLASH_ATTR custom_init() {
	init_brother_link();
}

#endif
