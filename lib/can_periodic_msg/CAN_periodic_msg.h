#ifndef CAN_PERIODIC_MSG_H_
#define CAN_PERIODIC_MSG_H_

#include <drivers/can.h>

void TxPeriodicMsg(const struct device *dev);
void CAN_PeriodicSendMsg(const struct device *dev, uint32_t msg_id);

#endif