#include "CAN_periodic_msg.h"

void TxPeriodicMsg(const struct device *dev)
{
    struct zcan_frame msg = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x123,
        .dlc = 8,
        .data = {1,2,3,4,5,6,7,8}
    };

    struct k_timer t;
    k_timer_init(&t, NULL, NULL);
    
    while(1){
        
        can_send(dev, &msg, K_MSEC(100), NULL, NULL);
        k_timer_start(&t, K_MSEC(1000), K_NO_WAIT);
        k_timer_status_sync(&t);
    }  
}

void CAN_PeriodicSendMsg(const struct device *dev, uint32_t msg_id)
{    
    struct zcan_frame msg = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = msg_id,
        .dlc = 8,
        .data = {8,7,6,5,4,3,2,1}
    };
    can_send(dev, &msg, K_MSEC(100), NULL, NULL);
}  