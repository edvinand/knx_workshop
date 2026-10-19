#include "app.h"

#include <knx/knx_app.h>
#include <knx/knx_device.h>
#include <knx/knx_transport.h>
#include <openthread.h>
#include <openthread/dataset.h>
#include <openthread/joiner.h>
#include <openthread/thread.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/util.h>

#include "oc_core_res.h"
#include "oc_knx.h"
#include "oc_knx_dev.h"
#include "oc_knx_fp.h"

#if defined(CONFIG_KNX_HARDCODED_COMMISSIONING)
#include "knx_hardcoded.h"
#include <knx/knx_presets.h>
#endif

LOG_MODULE_REGISTER(knx_sensor_tag, LOG_LEVEL_INF);

#define SOO 0
#define IOO 1

static atomic_t short_presses;
static atomic_t programming_requested;
static atomic_t reset_requested;

const char sn_lower_case[] = "00fa10020700";
const char application_name[] = "KNX Tag sensor (LSSB)";

static const knx_identity_t sensor_identity = {
	.serialnumber = sn_lower_case,
	.application_name = application_name,
	.hw_type = "000102030405",
	.dev_model = "6800",
	.mid = 0x00fa,
};

static knx_datapoint_t sensor_parameters[] = {
	{
		.path = "/p/globalTestParameter",
		.dpa = "urn:knx:dpa.65500.201",
		.dpt = ":dpt.value2Ucount",
		.id = KNX_PARAMETER_ID(0),
		.methods = KNX_DP_GET | KNX_DP_PUT,
		.properties = OC_DISCOVERABLE | OC_OBSERVABLE | OC_WRITE_AFFECTS_FP,
		.get_acl = OC_ACL_D,
		.get_iface = OC_IF_D,
		.put_acl = OC_ACL_P,
		.put_iface = OC_IF_P,
		.mirror_to = KNX_DP_NONE,
		.value = {.kind = KNX_DPT_VALUE_2_UCOUNT},
	},
};

static knx_datapoint_t sensor_datapoints[] = {
	{
		.path = "/p/lssb/0/soo",
		.dpa = "urn:knx:dpa.421.61",
		.dpt = ":dpt.switch",
		.id = KNX_DP_ID(0, SOO),
		.methods = KNX_DP_GET,
		.properties = OC_DISCOVERABLE | OC_OBSERVABLE,
		.get_acl = OC_ACL_O,
		.get_iface = OC_IF_O,
		.mirror_to = KNX_DP_NONE,
		.value = {.kind = KNX_DPT_BOOL},
	},
	{
		.path = "/p/lssb/0/ioo",
		.dpa = "urn:knx:dpa.421.53",
		.dpt = ":dpt.switch",
		.id = KNX_DP_ID(0, IOO),
		.methods = KNX_DP_GET | KNX_DP_PUT,
		.properties = OC_DISCOVERABLE | OC_OBSERVABLE,
		.get_acl = OC_ACL_I,
		.get_iface = OC_IF_I,
		.put_acl = OC_ACL_I,
		.put_iface = OC_IF_I,
		.mirror_to = KNX_DP_NONE,
		.value = {.kind = KNX_DPT_BOOL},
	},
	{
		.path = "/p/lssb/1/soo",
		.dpa = "urn:knx:dpa.421.61",
		.dpt = ":dpt.switch",
		.id = KNX_DP_ID(1, SOO),
		.methods = KNX_DP_GET,
		.properties = OC_DISCOVERABLE | OC_OBSERVABLE,
		.get_acl = OC_ACL_O,
		.get_iface = OC_IF_O,
		.mirror_to = KNX_DP_NONE,
		.value = {.kind = KNX_DPT_BOOL},
	},
	{
		.path = "/p/lssb/1/ioo",
		.dpa = "urn:knx:dpa.421.53",
		.dpt = ":dpt.switch",
		.id = KNX_DP_ID(1, IOO),
		.methods = KNX_DP_GET | KNX_DP_PUT,
		.properties = OC_DISCOVERABLE | OC_OBSERVABLE,
		.get_acl = OC_ACL_I,
		.get_iface = OC_IF_I,
		.put_acl = OC_ACL_I,
		.put_iface = OC_IF_I,
		.mirror_to = KNX_DP_NONE,
		.value = {.kind = KNX_DPT_BOOL},
	},
};

static const knx_functional_block_t sensor_blocks[] = {
	{
		.number = 421,
		.instance = 1,
		.datapoints = &sensor_datapoints[0],
		.num_datapoints = 2,
	},
	{
		.number = 421,
		.instance = 2,
		.datapoints = &sensor_datapoints[2],
		.num_datapoints = 2,
	},
};

#if defined(CONFIG_KNX_HARDCODED_COMMISSIONING)
static const knx_preset_t sensor_preset = {
	.fid = KNX_HC_FID,
	.iid = KNX_HC_IID,
	.ga = KNX_HC_GA,
	.grpid = KNX_HC_GRPID,
	.ia = 0x1102,
	.got_href = "/p/lssb/0/soo",
	.got_cflags = OC_CFLAG_TRANSMISSION,
	.is_publisher = false,
	.group_ms = knx_hc_group_ms,
	.group_ms_len = sizeof(knx_hc_group_ms),
	.group_kid = knx_hc_group_kid,
	.group_kid_len = sizeof(knx_hc_group_kid),
};
#endif

#if defined(CONFIG_OPENTHREAD_JOINER)
static void joiner_finished(otError error, void *context)
{
	ARG_UNUSED(context);
	otInstance *instance = openthread_get_default_instance();

	if (error != OT_ERROR_NONE || instance == NULL) {
		LOG_ERR("Thread Joiner failed: %d", error);
		return;
	}
	error = otThreadSetEnabled(instance, true);
	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to start Thread: %d", error);
	}
}
#endif

static void request_join(void)
{
	otInstance *instance = openthread_get_default_instance();

	if (instance == NULL) {
		LOG_ERR("OpenThread is unavailable");
		return;
	}

	openthread_mutex_lock();
	otError error = OT_ERROR_NONE;

	if (otDatasetIsCommissioned(instance)) {
		error = otThreadSetEnabled(instance, true);
	} else {
#if defined(CONFIG_OPENTHREAD_JOINER)
		if (otJoinerGetState(instance) == OT_JOINER_STATE_IDLE) {
			error = otJoinerStart(instance, CONFIG_OPENTHREAD_JOINER_PSKD, NULL, NULL,
					      NULL, NULL, NULL, joiner_finished, NULL);
		}
#else
		error = OT_ERROR_INVALID_STATE;
#endif
	}
	openthread_mutex_unlock();
	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to request Thread join: %d", error);
	}
}

static void sensor_toggle(void)
{
	bool value;
	int err = knx_datapoint_get_bool(KNX_DP_ID(0, SOO), &value);

	if (err < 0) {
		LOG_ERR("Failed to read sensor SOO: %d", err);
		return;
	}
	err = knx_datapoint_set_bool(KNX_DP_ID(0, SOO), !value);
	if (err < 0) {
		LOG_ERR("Failed to set sensor SOO: %d", err);
		return;
	}
	LOG_INF("Switch pressed: sending light %s", value ? "off" : "on");
	knx_datapoint_transmit(KNX_DP_ID(0, SOO));
}

void app_ui_event(enum tag_ui_event event)
{
	switch (event) {
	case TAG_UI_SHORT_PRESS:
		atomic_inc(&short_presses);
		break;
	case TAG_UI_PROGRAMMING_PRESS:
		atomic_set(&programming_requested, 1);
		break;
	case TAG_UI_RESET_PRESS:
		atomic_set(&reset_requested, 1);
		break;
	}
	knx_app_post_work();
}

static void sensor_process_ui(void)
{
	enum knx_network_state network = knx_get_network_state();
	bool attached = network == KNX_NETWORK_ATTACHED;
	atomic_val_t count = atomic_set(&short_presses, 0);
	bool programming = atomic_set(&programming_requested, 0) != 0;
	bool reset = atomic_set(&reset_requested, 0) != 0;

#if defined(CONFIG_KNX_ETS_COMMISSIONING)
	if (reset) {
		LOG_INF("KNX factory reset requested; Thread credentials are retained");
		oc_knx_device_reset(RESET_TO_DEFAULT_STATE);
		count = 0;
		programming = false;
	}
	if (programming && attached) {
		oc_knx_device_set_programming_mode(!oc_knx_device_in_programming_mode());
	}
#else
	ARG_UNUSED(programming);
	ARG_UNUSED(reset);
#endif
	while (count-- > 0) {
		if (!attached) {
			request_join();
		} else if (!oc_is_device_in_runtime()) {
#if defined(CONFIG_KNX_ETS_COMMISSIONING)
			oc_knx_device_set_programming_mode(!oc_knx_device_in_programming_mode());
#endif
		} else if (!oc_knx_device_in_programming_mode()) {
			sensor_toggle();
		}
	}
	tag_ui_set_status(network, oc_is_device_in_runtime(),
			  oc_knx_device_in_programming_mode());
}

static void sensor_on_init(void)
{
	knx_app_set_work_handler(sensor_process_ui);
}

static const knx_device_t sensor_device = {
	.identity = &sensor_identity,
	.parameters = sensor_parameters,
	.num_parameters = ARRAY_SIZE(sensor_parameters),
	.functional_blocks = sensor_blocks,
	.num_functional_blocks = ARRAY_SIZE(sensor_blocks),
	.on_init = sensor_on_init,
#if defined(CONFIG_KNX_HARDCODED_COMMISSIONING)
	.preset = &sensor_preset,
#else
	.preset = NULL,
#endif
};

void app_register_device(void)
{
	knx_device_register(&sensor_device);
}