#include <zephyr.h>
#include <kernel.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/can.h>
#include <drivers/gpio.h>
#include <sys/byteorder.h>

void rx_callback_function(struct zcan_frame *frame, void *arg);

struct zcan_frame frame = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x123,
        .dlc = 8,
        .data = {1,2,3,4,5,6,7,8}
};

const struct zcan_filter can_filter = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x124,
        .rtr_mask = 1,
        .id_mask = 0x7FF
};


const struct device *can_dev;
struct zcan_work rx_work;
int filter_id;
int ret;

void CAN_SendMsg(void)
{
    ret = can_send(can_dev, &frame, K_MSEC(100), NULL, NULL);
    if (ret != CAN_TX_OK) {
        printk("Sending failed [%d]", ret);
    }
}   

void CAN_Init(void)
{
    can_set_mode(can_dev, CAN_LOOPBACK_MODE);
    filter_id = can_attach_isr(can_dev, rx_callback_function, NULL, &can_filter);
}

void main(void)
{
    can_dev = device_get_binding("CAN_1");
    CAN_Init();

    while (1)
    {
        CAN_SendMsg();
        k_msleep(1000);
    }
    
}


void rx_callback_function(struct zcan_frame *frame, void *arg)
{
    printk("CAN message received...");
}