#include <zephyr.h>
#include <kernel.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/can.h>
#include <drivers/gpio.h>
#include <sys/byteorder.h>

struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});

void rx_callback_function(struct zcan_frame *frame, void *arg);
void tx_irq_callback(unsigned int error, void *arg);

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
        .id_mask = 0x00
};


const struct device *can_dev;
struct zcan_work rx_work;
int filter_id;
int ret;

void CAN_SendMsg(void)
{
    gpio_pin_set(led.port, led.pin, 1);
    ret = can_send(can_dev, &frame, K_MSEC(100), tx_irq_callback, "Sender 2");
}   

void CAN_Init(void)
{
    can_set_mode(can_dev, CAN_LOOPBACK_MODE);
    // can_set_mode(can_dev, CAN_NORMAL_MODE);
    filter_id = can_attach_isr(can_dev, rx_callback_function, NULL, &can_filter);
    if (!device_is_ready(can_dev)) {
		printk("CAN: Device %s not ready.\n", can_dev->name);
		return;
	}
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_HIGH);
}

void main(void)
{
    can_dev = device_get_binding("CAN_1");
    CAN_Init();

    while (1)
    {
        // gpio_pin_set(led.port, led.pin, 1);
        CAN_SendMsg();
        k_msleep(1000);
        // gpio_pin_set(led.port, led.pin, 0);
    }
    
}


void rx_callback_function(struct zcan_frame *frame, void *arg)
{
    printk("CAN message received...");
    const struct zcan_frame *rx_frame = frame;
    printk("Received CAN frame ID 0x%08X, DLC %d\n", rx_frame->id, rx_frame->dlc);
    // Process received data here
    for (size_t i = 0; i < rx_frame->dlc; i++)
    {
        printk("0x%02X\n", rx_frame->data[i]);
    }
    gpio_pin_set(led.port, led.pin, 0);
}

void tx_irq_callback(unsigned int error, void *arg)
{
        char *sender = (char *)arg;

        if (error != 0) {
                printk("Sendig failed [%d]\nSender: %s\n", error, sender);
        }
}
