#include "vl53l0.h"



VL53L0X_Dev_t vl53l0x_dev;
VL53L0X_DeviceInfo_t vl53l0x_dev_info;
VL53L0X_RangingMeasurementData_t vl53l0x_data;
VL53L0X_Error vl53l0x_status;

/***************************************************************************************/

void print_pal_error(VL53L0X_Error Status)
{
    char buf[VL53L0X_MAX_STRING_LENGTH];
    VL53L0X_GetPalErrorString(Status, buf);  //得到错误码与字符串
    printf("API Status: %i : %s\n", Status, buf);
}

void print_range_status(VL53L0X_RangingMeasurementData_t* pdata)
{
    char buf[VL53L0X_MAX_STRING_LENGTH];
    uint8_t RangeStatus;
    // New Range Status: data is valid when pdata->RangeStatus = 0
    RangeStatus = pdata->RangeStatus;

    VL53L0X_GetRangeStatusString(RangeStatus, buf);
    printf("Range Status: %i : %s\n", RangeStatus, buf);
}

VL53L0X_Error vl53l0x_start_single_test(VL53L0X_Dev_t *pdev, \
                            VL53L0X_RangingMeasurementData_t *pdata)
{
    VL53L0X_Error status = VL53L0X_ERROR_NONE;
    
    if(vl53l0x_status != VL53L0X_ERROR_NONE)
        return vl53l0x_status;

    status = VL53L0X_PerformSingleRangingMeasurement(pdev, pdata);   //VL53L0X执行单一测量范围
    if(status != VL53L0X_ERROR_NONE){
        printf("error:Call of VL53L0X_PerformSingleRangingMeasurement\n");
        return status;
    }
    print_range_status(pdata);
    printf("Measured distance: %i\n\n", pdata->RangeMilliMeter);
    return status;
}


VL53L0X_Error vl53l0x_measure_init(VL53L0X_Dev_t *pMyDevice)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    //FixPoint1616_t LimitCheckCurrent;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;
    // Device Initialization
    Status = VL53L0X_StaticInit(pMyDevice);   //基本设备初始化
    if(Status != VL53L0X_ERROR_NONE){         //判断是否初始化成功 失败打印错误信息
        printf ("Call of VL53L0X_StaticInit\n");
        print_pal_error(Status);
        return Status;
    }
    // Device Initialization
    Status = VL53L0X_PerformRefCalibration(pMyDevice, &VhvSettings, &PhaseCal);    //执行参考校准
    if(Status != VL53L0X_ERROR_NONE){        //判断是否校准成功
        printf ("Call of VL53L0X_PerformRefCalibration\n");
        print_pal_error(Status);
        return Status;
    }

    // needed if a coverglass is used and no calibration has been performed
    Status = VL53L0X_PerformRefSpadManagement(pMyDevice,&refSpadCount, &isApertureSpads);
    if(Status != VL53L0X_ERROR_NONE){
        printf ("Call of VL53L0X_PerformRefSpadManagement\n");
        printf ("refSpadCount = %d, isApertureSpads = %d\n", refSpadCount, isApertureSpads);
        print_pal_error(Status);
        return Status;
    }

    // no need to do this when we use VL53L0X_PerformSingleRangingMeasurement  当我们使用VL53L0X_PerformSingleRangingMeasurement不需要
    Status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode 设置单一测量模式
    if(Status != VL53L0X_ERROR_NONE){
        printf ("Call of VL53L0X_SetDeviceMode\n");
        print_pal_error(Status);
        return Status;
    }

    // Enable/Disable Sigma and Signal check
    if(Status == VL53L0X_ERROR_NONE){
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice,  //启用/禁用特定限制检查
        		VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
    }
    if(Status == VL53L0X_ERROR_NONE){
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
        		VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
    }
				
    if(Status == VL53L0X_ERROR_NONE){
        Status = VL53L0X_SetLimitCheckValue(pMyDevice,
        		VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
        		(FixPoint1616_t)(0.25*65536));
	}			
    if(Status == VL53L0X_ERROR_NONE){
        Status = VL53L0X_SetLimitCheckValue(pMyDevice,
        		VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
        		(FixPoint1616_t)(18*65536));			
    }
    if(Status == VL53L0X_ERROR_NONE){
        Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pMyDevice,   //设置微秒时间
        		200000);
    }
    if(Status != VL53L0X_ERROR_NONE){
        printf ("Sigma and Signal check error\n");
        print_pal_error(Status);
        return Status;
    }
    /*
     *  Step  4 : Test ranging mode
     */
  /*  for(i=0;i<10;i++){
            printf ("Call of VL53L0X_PerformSingleRangingMeasurement\n");
            Status = VL53L0X_PerformSingleRangingMeasurement(pMyDevice,
            		&vl53l0x_data);

            print_pal_error(Status);
            print_range_status(&vl53l0x_data);

            VL53L0X_GetLimitCheckCurrent(pMyDevice,
            		VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, &LimitCheckCurrent);

            printf("RANGE IGNORE THRESHOLD: %f\n\n", (float)LimitCheckCurrent/65536.0);


            if (Status != VL53L0X_ERROR_NONE) break;

            printf("Measured distance: %i\n\n", vl53l0x_data.RangeMilliMeter);

    }
*/
    return Status;   //返回0
}

VL53L0X_Error vl53l0x_init(void)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;   //初始值赋值为0

    VL53L0X_Dev_t *pMyDevice = &vl53l0x_dev;       //得到设备地址

    pMyDevice->I2cDevAddr      = 0x52;            //iic地址
    pMyDevice->comms_type      =  1;              //选择IIC还是SPI    iic=1；SPI=0
    pMyDevice->comms_speed_khz =  400;            //iic速率
    
    VL53L0X_i2c_init();          //IIC配置    

    Status = VL53L0X_DataInit(pMyDevice); // Data initialization  //VL53L0X_DataInit：一次设备的初始化，初始化成功返回0
    if(Status != VL53L0X_ERROR_NONE){     //判断如果状态不为0   打印错误信息
        print_pal_error(Status);
        return Status;        //  返回错误值 可通过此值DEBUG查找错误位置
    }

    Status = VL53L0X_GetDeviceInfo(pMyDevice, &vl53l0x_dev_info);   //读取给定设备的设备信息
    if(Status != VL53L0X_ERROR_NONE){
        print_pal_error(Status);
        return Status;
    }
    printf("VL53L0X_GetDeviceInfo:\n");
    printf("Device Name : %s\n", vl53l0x_dev_info.Name);     //设备名
    printf("Device Type : %s\n", vl53l0x_dev_info.Type);    //产品类型VL53L0X = 1, VL53L1 = 2 
    printf("Device ID : %s\n", vl53l0x_dev_info.ProductId);   // 设备ID
    printf("ProductRevisionMajor : %d\n", vl53l0x_dev_info.ProductRevisionMajor);
    printf("ProductRevisionMinor : %d\n", vl53l0x_dev_info.ProductRevisionMinor);

    if ((vl53l0x_dev_info.ProductRevisionMajor != 1) && (vl53l0x_dev_info.ProductRevisionMinor != 1)){
        printf("Error expected cut 1.1 but found cut %d.%d\n",
        vl53l0x_dev_info.ProductRevisionMajor, vl53l0x_dev_info.ProductRevisionMinor);
        Status = VL53L0X_ERROR_NOT_SUPPORTED;
        print_pal_error(Status);
        return Status;
    }

    Status = vl53l0x_measure_init(pMyDevice);   //测量配置
    vl53l0x_status = Status;
    if(Status != VL53L0X_ERROR_NONE){    //判断如果不为0打印错误信息
        print_pal_error(Status);
        return Status;
    }

    return Status;           //返回0
}
