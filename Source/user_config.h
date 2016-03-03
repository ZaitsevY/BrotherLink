#ifndef _user_config_h_
#define _user_config_h_

// Substitutes default esp8266web/include/user_config.h with a customized one

#include "sdk/sdk_config.h"

#define SYS_VERSION "0.5.9a"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define WEB_DEFAULT_SOFTAP_IP	DEFAULT_SOFTAP_IP // ip 192.168.4.1
#define WEB_DEFAULT_SOFTAP_MASK	DEFAULT_SOFTAP_MASK // mask 255.255.255.0
#define WEB_DEFAULT_SOFTAP_GW	DEFAULT_SOFTAP_IP // gw 192.168.4.1
#define WEB_DEFAULT_STATION_IP    0x3201A8C0 // ip 192.168.1.50
#define WEB_DEFAULT_STATION_MASK  0x00FFFFFF // mask 255.255.255.0
#define WEB_DEFAULT_STATION_GW    0x0101A8C0 // gw 192.168.1.1
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Customized definitions

#define USE_CPU_SPEED  160 // установить частоту CPU по умолчанию 80 или 160 MHz

#define USE_WEB		80	// включить в трансялцию порт Web, если =0 - по умолчанию выключен

#define WEBSOCKET_ENA	// включить WEBSOCKET

// Enable TCP2UART on port 12345
#define USE_TCP2UART 12345	// включить в трансялцию драйвер TCP2UART, номер порта по умолчанию (=0 - отключен)

#define UDP_TEST_PORT	1025	// включить в трансялцию контрольный порт UDP, (=0 - отключен)

#define USE_NETBIOS	1 // включить в трансялцию драйвер NETBIOS, если =0 - по умолчанию выключен.

#define USE_SNTP	1 // включить в трансялцию драйвер SNTP, если =0 - по умолчанию выключен, = 1 - по умолчанию включен.

#define USE_CAPTDNS	1	// включить в трансялцию NDS отвечающий на всё запросы клиента при соединении к AP модуля
						// указанием на данный WebHttp (http://aesp8266/), если =0 - по умолчанию выключен

#endif // _user_config_h_
