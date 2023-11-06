#ifndef CAN_MSG_Q_H_
#define CAN_MSG_Q_H_

#include <drivers/can.h>
#include <drivers/gpio.h>

void CAN_SenMsgLowPriority(const struct device *dev, uint32_t delay);
void CAN_SenMsgHighPriority(const struct device *dev);

void Control_LED(struct zcan_frame *frame, void *arg0);
void CAN_SetWorkQueue(const struct device *dev);

#endif