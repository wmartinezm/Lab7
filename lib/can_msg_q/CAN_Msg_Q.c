#include "CAN_Msg_Q.h"
#include <sys/printk.h>

struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {0});

void CAN_SenMsgLowPriority(const struct device *dev, uint32_t delay)
{
    struct zcan_frame lp_frame = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x010,
        .dlc = 8,
        .data = {8,7,6,5,4,3,2,1}
    };
    struct k_timer timer;
    k_timer_init(&timer, NULL, NULL);

    while(1){
        can_send(dev, &lp_frame, K_MSEC(100), NULL, NULL);
        k_timer_start(&timer, K_MSEC(delay), K_NO_WAIT);
        k_timer_status_sync(&timer);
    }
}

void CAN_SenMsgHighPriority(const struct device *dev)
{
    struct zcan_frame hp_frame = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x001,
        .dlc = 8,
        .data = {8,7,6,5,4,3,2,1}
    };

    while(1){
        can_send(dev, &hp_frame, K_MSEC(100), NULL, NULL);
    }
}

void Control_LED(struct zcan_frame *msg, void *arg0)
{
	ARG_UNUSED(arg0);

	if (led1.port == NULL) {
		printk("LED %s\n", msg->data[0] == 1 ? "ON" : "OFF");
		return;
	}

	switch (msg->data[0]) {
	case 1:
		gpio_pin_set(led1.port, led1.pin, 1);
		break;
	case 0:
		gpio_pin_set(led1.port, led1.pin, 0);
		break;
	}
}

void CAN_SetWorkQueue(const struct device *dev)
{
    const struct zcan_filter hp_filter = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x001,
        .rtr_mask = 1,
        .id_mask = CAN_EXT_ID_MASK
    };
    struct zcan_work rx_work;
    int filter_id;

    filter_id = can_attach_workq(dev, &k_sys_work_q, &rx_work, Control_LED, NULL, &hp_filter);
    if (filter_id < 0) {
        printk("Unable to attach isr [%d]", filter_id);
    }
}