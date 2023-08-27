/*
 * Copyright (c) 2023 Jamie M.
 *
 * All right reserved. This code is not apache or FOSS/copyleft licensed.
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(abe, CONFIG_APPLICATION_LOG_LEVEL);

const struct device *const dev = DEVICE_DT_GET_ANY(bosch_bme280);
const uint8_t device_number = '0';

int main(void)
{
	if (!device_is_ready(dev)) {
		LOG_ERR("Device not ready");
		return -1;
	}

	return 0;
}

#if defined(CONFIG_APP_OUTPUT_FORMAT_CUSTOM)
#error "Not supported"
/* Outputs ESS readings in the following format:
 * Start delimiter: ##
 * { for each device with data:
 *     Index number: e.g. 0
 *     Temperature reading: e.g. 25.12
 *     Pressure reading: e.g. 1000270
 *     Humidity reading: e.g. 52.04
 *     Dew point reading: e.g. 8
 * }
 * End delimiter: ^^
 *
 * Each value has a comma (,) separator.
 */
static int ess_readings_handler(const struct shell *sh, size_t argc, char **argv)
{
	uint8_t i = 0;
	uint8_t buffer[128] = {0};

	while (i < DEVICE_COUNT) {
		if (devices[i].state == STATE_ACTIVE &&
		    devices[i].readings.received == RECEIVED_ALL) {
			sprintf(&buffer[strlen(buffer)], "%d,"
#ifdef CONFIG_APP_ESS_TEMPERATURE
				"%.2f,"
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
				"%.0f,"
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
				"%.2f,"
#endif
#ifdef CONFIG_APP_ESS_DEW_POINT
				"%d,"
#endif
				, i
#ifdef CONFIG_APP_ESS_TEMPERATURE
				, devices[i].readings.temperature
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
				, devices[i].readings.humidity
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
				, devices[i].readings.pressure
#endif
#ifdef CONFIG_APP_ESS_DEW_POINT
				, devices[i].readings.dew_point
#endif
				);
			devices[i].readings.received = RECEIVED_NONE;
		}

		++i;
	}

	if (strlen(buffer) > 0) {
		buffer[(strlen(buffer) - 1)] = 0;
	}

	shell_print(sh, "##%s^^", buffer);

	return 0;
}
#elif defined(CONFIG_APP_OUTPUT_FORMAT_CSV)
/* Outputs ESS readings in CSV format, with headings */
static int ess_readings_handler(const struct shell *sh, size_t argc, char **argv)
{
	uint8_t buffer[256] = {0};
#ifdef CONFIG_APP_ESS_TEMPERATURE
	struct sensor_value temperature;
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
	struct sensor_value humidity;
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
	struct sensor_value pressure;
#endif
#ifdef CONFIG_APP_ESS_TEMPERATURE
	float temperature_float;
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
	float humidity_float;
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
	float pressure_float;
#endif

	sensor_sample_fetch(dev);

#ifdef CONFIG_APP_ESS_TEMPERATURE
	sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
	sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
	sensor_channel_get(dev, SENSOR_CHAN_PRESS, &pressure);
#endif

#ifdef CONFIG_APP_ESS_TEMPERATURE
	temperature_float = sensor_value_to_float(&temperature);
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
	humidity_float = sensor_value_to_float(&humidity);
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
	pressure_float = sensor_value_to_float(&pressure);
#endif

	sprintf(&buffer[0], "device,"
#ifdef CONFIG_APP_ESS_TEMPERATURE
		"temperature,"
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
		"humidity,"
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
		"pressure,"
#endif
#ifdef CONFIG_APP_ESS_DEW_POINT
		"dewpoint,"
#endif
		"\n");

	sprintf(&buffer[strlen(buffer)], "%c,"
#ifdef CONFIG_APP_ESS_TEMPERATURE
		"%.2f,"
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
		"%.2f,"
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
		"%.0f,"
#endif
#ifdef CONFIG_APP_ESS_DEW_POINT
		"%d,"
#endif
		"\n", device_number
#ifdef CONFIG_APP_ESS_TEMPERATURE
		, temperature_float
#endif
#ifdef CONFIG_APP_ESS_HUMIDITY
		, humidity_float
#endif
#ifdef CONFIG_APP_ESS_PRESSURE
		, pressure_float
#endif
#ifdef CONFIG_APP_ESS_DEW_POINT
		, TODO
#endif
		);

	shell_print(sh, "%s\n", buffer);

	return 0;
}
#else
#error "Invalid output format selected"
#endif

static int ess_status_handler(const struct shell *sh, size_t argc, char **argv)
{
	if (device_is_ready(dev)) {
		shell_print(sh, "Ready!");
	} else {
		shell_error(sh, "Error");
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(ess_cmd,
	/* Command handlers */
	SHELL_CMD(readings, NULL, "Output ESS values", ess_readings_handler),
	SHELL_CMD(status, NULL, "Show device status", ess_status_handler),

	/* Array terminator. */
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(ess, &ess_cmd, "ESS profile commands", NULL);
