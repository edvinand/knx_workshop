#include "app.h"

#include <knx/knx_app.h>
#include <stdlib.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(light_switch_sensor_tag, LOG_LEVEL_INF);

int main(void)
{
	int err = tag_ui_init(app_ui_event);

	if (err < 0) {
		LOG_ERR("Failed to initialize Tag UI: %d", err);
		return EXIT_FAILURE;
	}

	LOG_INF("KNX IoT Tag light switch sensor");
	app_register_device();
	err = knx_app_start();
	if (err < 0) {
		LOG_ERR("Failed to start KNX application: %d", err);
		return EXIT_FAILURE;
	}

	tag_ui_start();
	return EXIT_SUCCESS;
}