#ifndef  __VL53L0X_H
#define  __VL53L0X_H


#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_i2c_platform.h"





VL53L0X_Error vl53l0x_start_single_test(VL53L0X_Dev_t *pdev, \
                            VL53L0X_RangingMeasurementData_t *pdata);
VL53L0X_Error vl53l0x_init(void);
#endif
