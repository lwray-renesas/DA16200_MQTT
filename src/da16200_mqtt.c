#include "hal.h"
#include "da16200_AT.h"
#include "sensor.h"
#include <string.h>
#include <stdio.h>

static void demo_err(void);
static void mqtt_msg_send(void);

void main(void)
{
	static fsp_err_t err;

	EI();

	/* Enable the UART*/
	Hal_uart_start();

	/* Initialise the sensor*/
	Sensor_init();

	/* Connect MCU with Wi-Fi module and initialize Wi-Fi module */
	err = wifi_con_init();
	if(FSP_SUCCESS != err)
	{
		demo_err();
	}

	/* Configure Wi-Fi module to station mode and let it connect to AP */
	err = wifi_con_routine();
	if(FSP_SUCCESS != err)
	{
		demo_err();
	}

	/* Configure Wi-Fi module to MQTT client function */
	err = mqtt_con_routine();
	if(FSP_SUCCESS != err)
	{
		demo_err();
	}

	Sensor_read(); /* Dummy read to populate structs with data*/

	while(1)
	{
		Sensor_read();

		mqtt_msg_send();
	}
}

static void mqtt_msg_send(void)
{
	static char temperature_value_str[10];
	static char humidity_value_str[10];
	static char mqtt_head[] = "AT+NWMQMSG=";
	static float l_temperature = 0.00f;
	static float l_humidity = 0.00f;
	fsp_err_t err;

	l_temperature = (float)Sensor_get_temp();
	l_humidity = (float)Sensor_get_humidity();

	sprintf(temperature_value_str, "%f", l_temperature);
	sprintf(humidity_value_str, "%f", l_humidity);

	g_da16200_cmd_set[DA16200_AT_CMD_INDEX_AT_NWMQMSG].p_cmd = (uint8_t *)mqtt_head;

	part_array[0] = (uint32_t)temperature_value_str;
	part_array[1] = (uint32_t)at_topic_t;
	part_array[2] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_NWMQMSG, part_array, 3);
	err = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQMSG);
	if(FSP_SUCCESS != err)
	{
		/* TODO: Handle Error*/
	}

	R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS);

	g_da16200_cmd_set[DA16200_AT_CMD_INDEX_AT_NWMQMSG].p_cmd = (uint8_t *)mqtt_head;

	part_array[0] = (uint32_t)humidity_value_str;
	part_array[1] = (uint32_t)at_topic_h;
	part_array[2] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_NWMQMSG, part_array, 3);
	err = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQMSG);
	if(FSP_SUCCESS != err)
	{
		/* TODO: Handle Error*/
	}

	R_BSP_SoftwareDelay(4000, BSP_DELAY_MILLISECS);
}

static void demo_err(void)
{
	while(1)
	{
		/* TODO: Blink LED*/
	}
}
