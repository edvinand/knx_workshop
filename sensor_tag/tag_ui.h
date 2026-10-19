#ifndef SENSOR_TAG_UI_H_
#define SENSOR_TAG_UI_H_

#include <stdbool.h>
#include <knx/knx_transport.h>

enum tag_ui_event {
	TAG_UI_SHORT_PRESS,
	TAG_UI_PROGRAMMING_PRESS,
	TAG_UI_RESET_PRESS,
};

typedef void (*tag_ui_handler_t)(enum tag_ui_event event);

int tag_ui_init(tag_ui_handler_t handler);
void tag_ui_start(void);
void tag_ui_set_status(enum knx_network_state network, bool commissioned, bool programming);

#endif