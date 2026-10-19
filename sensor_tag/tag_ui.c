#include "tag_ui.h"

#include <errno.h>
#include <knx/knx_app.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(tag_ui, LOG_LEVEL_INF);

#define DEBOUNCE_MS 30
#define UI_PERIOD_MS 100
#define BLINK_PERIOD_MS 500
#define PRESS_FLASH_MS 150
#define PROGRAMMING_PRESS_MS 3000
#define RESET_PRESS_MS 6000

#define STATUS_ATTACHING BIT(0)
#define STATUS_ATTACHED BIT(1)
#define STATUS_COMMISSIONED BIT(2)
#define STATUS_PROGRAMMING BIT(3)

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_NODELABEL(led1_red), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_NODELABEL(led1_green), gpios);
static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_NODELABEL(led1_blue), gpios);

static struct gpio_callback button_callback;
static struct k_work_delayable debounce_work;
static struct k_work_delayable display_work;
static tag_ui_handler_t event_handler;
static atomic_t ready;
static atomic_t status;
static bool pressed;
static int64_t pressed_at;
static int64_t flash_until;

void tag_ui_set_status(enum knx_network_state network, bool commissioned, bool programming)
{
	atomic_val_t value = 0;

	if (network == KNX_NETWORK_ATTACHING) {
		value |= STATUS_ATTACHING;
	} else if (network == KNX_NETWORK_ATTACHED) {
		value |= STATUS_ATTACHED;
	}
	if (commissioned) {
		value |= STATUS_COMMISSIONED;
	}
	if (programming) {
		value |= STATUS_PROGRAMMING;
	}
	atomic_set(&status, value);
}

static void set_led(const struct gpio_dt_spec *led, bool on)
{
	int err = gpio_pin_set_dt(led, on);

	if (err < 0) {
		LOG_ERR("Failed to update LED: %d", err);
	}
}

static void update_display(struct k_work *work)
{
	ARG_UNUSED(work);
	int64_t now = k_uptime_get();
	bool blink = (now / BLINK_PERIOD_MS) % 2 == 0;
	atomic_val_t state = atomic_get(&status);
	bool attached = (state & STATUS_ATTACHED) != 0;
	bool red_on = now < flash_until;

	if (pressed && now - pressed_at >= PROGRAMMING_PRESS_MS) {
		red_on = now - pressed_at >= RESET_PRESS_MS || blink;
	}

	set_led(&blue, attached || ((state & STATUS_ATTACHING) != 0 && blink));
	set_led(&green, attached && ((state & STATUS_PROGRAMMING) != 0
					   ? blink : (state & STATUS_COMMISSIONED) != 0));
	set_led(&red, red_on);
	knx_app_post_work();
	k_work_schedule(&display_work, K_MSEC(UI_PERIOD_MS));
}

static void debounce_button(struct k_work *work)
{
	ARG_UNUSED(work);
	int level = gpio_pin_get_dt(&button);

	if (level < 0) {
		LOG_ERR("Failed to read button: %d", level);
		return;
	}
	if (!atomic_get(&ready) || (level != 0) == pressed) {
		return;
	}

	pressed = level != 0;
	if (pressed) {
		pressed_at = k_uptime_get();
		flash_until = pressed_at + PRESS_FLASH_MS;
		return;
	}

	int64_t duration = k_uptime_get() - pressed_at;
	enum tag_ui_event event = TAG_UI_SHORT_PRESS;

	if (duration >= RESET_PRESS_MS) {
		event = TAG_UI_RESET_PRESS;
	} else if (duration >= PROGRAMMING_PRESS_MS) {
		event = TAG_UI_PROGRAMMING_PRESS;
	}
	event_handler(event);
}

static void button_changed(const struct device *port, struct gpio_callback *callback,
			   gpio_port_pins_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(callback);
	ARG_UNUSED(pins);
	k_work_reschedule(&debounce_work, K_MSEC(DEBOUNCE_MS));
}

int tag_ui_init(tag_ui_handler_t handler)
{
	const struct gpio_dt_spec *leds[] = {&red, &green, &blue};

	if (handler == NULL) {
		return -EINVAL;
	}
	if (!gpio_is_ready_dt(&button)) {
		return -ENODEV;
	}
	for (size_t index = 0; index < ARRAY_SIZE(leds); index++) {
		if (!gpio_is_ready_dt(leds[index])) {
			return -ENODEV;
		}
		int err = gpio_pin_configure_dt(leds[index], GPIO_OUTPUT_INACTIVE);

		if (err < 0) {
			return err;
		}
	}

	int err = gpio_pin_configure_dt(&button, GPIO_INPUT);

	if (err < 0) {
		return err;
	}
	event_handler = handler;
	k_work_init_delayable(&debounce_work, debounce_button);
	k_work_init_delayable(&display_work, update_display);
	gpio_init_callback(&button_callback, button_changed, BIT(button.pin));
	err = gpio_add_callback(button.port, &button_callback);
	if (err < 0) {
		return err;
	}
	err = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH);
	if (err < 0) {
		gpio_remove_callback(button.port, &button_callback);
	}
	return err;
}

void tag_ui_start(void)
{
	atomic_set(&ready, 1);
	k_work_schedule(&debounce_work, K_MSEC(DEBOUNCE_MS));
	k_work_schedule(&display_work, K_NO_WAIT);
}