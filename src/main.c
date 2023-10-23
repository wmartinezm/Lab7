#include <zephyr.h>
#include <kernel.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/can.h>
#include <drivers/gpio.h>
#include <sys/byteorder.h>

struct zcan_frame frame = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x123,
        .dlc = 8,
        .data = {1,2,3,4,5,6,7,8}
};

const struct device *can_dev;
int ret;

void CAN_SendMsg(void)
{
    ret = can_send(can_dev, &frame, K_MSEC(100), NULL, NULL);
    if (ret != CAN_TX_OK) {
        LOG_ERR("Sending failed [%d]", ret);
}
}

void main(void)
{
    can_dev = device_get_binding("CAN_1");

    while (1)
    {
        CAN_SendMsg();
        k_msleep(1000);
    }
    
}
