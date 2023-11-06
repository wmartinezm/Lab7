#include <zephyr.h>
#include <kernel.h>
#include <device.h>
#include <devicetree.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/can.h>
#include <drivers/gpio.h>
#include <sys/byteorder.h>
#include "CAN_periodic_msg.h"
#include "CAN_Msg_Q.h"


#define SET_LOOPBACK_MODE       // Comment this line if you dont want the loopback mode to be set
#define LED_MSG_ID 0x123
#define CTR_MSG_ID 0x124
// Only keep uncomment one of the following three activity definitions to avoid 
// unspected behavior
#define ACTIVITY_0
#define ACTINITY_1
#define ACTIVITY_3

typedef struct k_thread thread;
#define TX_THREAD_STACK_SIZE 512
K_THREAD_STACK_DEFINE(tx_thread_stack, TX_THREAD_STACK_SIZE);

struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});

void rx_callback_function(struct zcan_frame *frame, void *arg);
void tx_irq_callback(unsigned int error, void *arg);
void rx_thread(void *arg1, void * arg2, void * arg3);

struct zcan_frame led_frame = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = LED_MSG_ID,
        .dlc = 8,
        .data = {8,7,6,5,4,3,2,1}
};

struct zcan_frame ctr_frame = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = CTR_MSG_ID,
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
    ret = can_send(can_dev, &led_frame, K_MSEC(100), tx_irq_callback, "Sender 2");
}   

void CAN_Init(void)
{
    #if defined(ACTIVITY_0) || defined(ACTINITY_1)
    can_set_mode(can_dev, CAN_LOOPBACK_MODE);
    #else
    can_set_mode(can_dev, CAN_NORMAL_MODE);
    #endif
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

    #ifdef ACTIVITY_0
    thread tx_thread;
    k_thread_create(&tx_thread,
                    tx_thread_stack,
                    TX_THREAD_STACK_SIZE,
                    (k_thread_entry_t) TxPeriodicMsg,
                    (void *) can_dev,
                    NULL,
                    NULL,
                    K_PRIO_PREEMPT(7),
                    0,
                    K_NO_WAIT);
    k_sleep(K_FOREVER);
    #endif
    #ifdef ACTIVITY_3
    CAN_SetWorkQueue(can_dev);
    #endif

    while (1)
    {
        #ifdef ACTIVITY_0
        CAN_SendMsg();
        k_msleep(1000);
        #endif
        #ifdef ACTINITY_1
        CAN_PeriodicSendMsg(can_dev, 0x123);
        k_sleep(K_MSEC(500));
        CAN_PeriodicSendMsg(can_dev, 0x124);
        k_sleep(K_MSEC(500));
        #endif
        #ifdef ACTIVITY_3
        CAN_SenMsgHighPriority(can_dev);
        CAN_SenMsgLowPriority(can_dev, 500);
        #endif
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

void rx_thread(void *arg1, void * arg2, void * arg3)
{

}