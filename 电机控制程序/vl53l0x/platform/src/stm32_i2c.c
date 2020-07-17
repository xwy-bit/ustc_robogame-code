#include "stm32_i2c.h"
#include "delay.h"
#include "stdio.h"
/*********************

这是stm32的i2c接口，使用i2c相关的函数请在这里找
.h文件定义了i2c的接口，SCL为PA3，SDA为PA2；X未使用请无视
——Gabonlee

*********************/
void i2c_delay(void)//延时函数，似乎没什么用
{
    //delay_ms(2);
    int i = 30;
    while(i--);
}
//1:fail  0:success
uint8_t i2c_start(void)//开始i2c通信
{
    I2C_SDA_HIGH();
    I2C_SCL_HIGH();
    i2c_delay();
    //i2c_delay();
    if(!I2C_SDA_STATE)
        return 1;
    I2C_SDA_LOW();
    i2c_delay();
    if(I2C_SDA_STATE)
        return 1;
    I2C_SCL_LOW();
    i2c_delay();
    return 0;
}

void i2c_stop(void)//停止i2c通信
{
    I2C_SCL_LOW();
    i2c_delay();
    I2C_SDA_LOW();
    i2c_delay();
    I2C_SCL_HIGH();
    i2c_delay();
    I2C_SDA_HIGH();
    delay_ms(2);
    //i2c_delay();
}

void i2c_ack(void)
{
    I2C_SCL_LOW();
    i2c_delay();
    I2C_SDA_LOW();
    i2c_delay();
    I2C_SCL_HIGH();
    i2c_delay();
    I2C_SCL_LOW();
    i2c_delay();
}

void i2c_no_ack(void)
{
    I2C_SCL_LOW();
    i2c_delay();
    I2C_SDA_HIGH();
    i2c_delay();
    I2C_SCL_HIGH();
    i2c_delay();
    I2C_SCL_LOW();
    i2c_delay();
}

uint8_t i2c_wait_ack(void)
{
    I2C_SCL_LOW();
    i2c_delay();
    I2C_SDA_HIGH();
    i2c_delay();
    I2C_SCL_HIGH();
    i2c_delay();
    if(I2C_SDA_STATE)
    {
        I2C_SCL_LOW();
        return 1;
    }
    I2C_SCL_LOW();
    return 0;
}

//0:success 1:fail
void i2c_send_byte(uint8_t byte)
{
    uint8_t i = 8;

    while(i--)
    {
        I2C_SCL_LOW();
        i2c_delay();
        if(byte & 0x80)
        {
            I2C_SDA_HIGH();
        }
        else
        {
            I2C_SDA_LOW();
        }
        byte <<= 1;
        i2c_delay();
        I2C_SCL_HIGH();
        i2c_delay();
    }
    I2C_SCL_LOW();
}

uint8_t i2c_receive_byte(void)
{
    uint8_t i = 8;
    uint8_t byte = 0;

    I2C_SDA_HIGH();
    while(i--)
    {
        byte <<= 1;
        I2C_SCL_LOW();
        i2c_delay();
        I2C_SCL_HIGH();
        i2c_delay();
        if(I2C_SDA_STATE)
        {
            byte |= 0x01;
        }
    }
    I2C_SCL_LOW();
    return byte;
}

void i2c_init(void)//STM32的i2c初始化，端口定义在.h文件里面
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//注意修改使能时钟的端口（目前是GPIOA）
    
    GPIO_InitStructure.GPIO_Pin = I2C_PIN_SCL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//不需要50MHz的输出速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//SCL推挽输出
    GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);

   /* GPIO_InitStructure.GPIO_Pin = I2C_PIN_X;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(I2C_X_GPIO, &GPIO_InitStructure);
    */
    GPIO_InitStructure.GPIO_Pin = I2C_PIN_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//同上
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//SDA开漏输出
    GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);
    
	/*关于SCL和SDA推挽还是开漏的问题：
	单主机的情况下，SCL可以用开漏，也可以用推挽；
	无论什么情况，SDA必须开漏。
	开漏要外接上拉电阻拉到VCC
	测距模块实测SCL、SDA均有上拉电阻，两端口悬空时会把电平拉到2.1V左右，单片机会识别为高电平
	因此直接开漏就可以了
	参考：https://blog.csdn.net/qq_31073871/article/details/86593925
	*/
    /*I2C_X_LOW();
    delay_ms(2);
    I2C_X_HIGH();
    delay_ms(1);*/
}

uint8_t i2c_write_buffer(uint8_t addr, uint8_t reg, uint32_t len, uint8_t *data)
{
    uint32_t i;
    
    if(i2c_start())
        return 1;
    
    i2c_send_byte(addr | I2C_Direction_Transmitter);
    
    if (i2c_wait_ack()) {
        i2c_stop();
        return 1;
    }
    
    i2c_send_byte(reg);
    i2c_wait_ack();
    for (i = 0; i < len; i++) {
        i2c_send_byte(data[i]);
        if (i2c_wait_ack()) {
            i2c_stop();
            return 1;
        }
    }
    i2c_stop();
    return 0;
}

uint8_t i2c_read_buffer(uint8_t addr, uint8_t reg, uint32_t len, uint8_t *buf)
{
    if(i2c_start())
        return 1;
    
    i2c_send_byte(addr | I2C_Direction_Transmitter);
    if (i2c_wait_ack()) {
        i2c_stop();
        return 1;
    }
    
    i2c_send_byte(reg);
    i2c_wait_ack();
    i2c_delay();
    i2c_start();
    i2c_send_byte(addr | I2C_Direction_Receiver);
    i2c_wait_ack();
    
    while(len) {
        *buf = i2c_receive_byte();
        if(len == 1)
            i2c_no_ack();
        else
            i2c_ack();
        buf++;
        len--;
    }
    i2c_stop();
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////
//0:success 1:fail
uint8_t i2c_write(uint8_t addr, uint8_t reg, uint32_t len, uint8_t * data)
{
	if(i2c_write_buffer(addr,reg,len,data))
	{
		return 1;
	}

	return 0;
}
uint8_t i2c_read(uint8_t addr, uint8_t reg, uint32_t len, uint8_t *buf)
{
	if(i2c_read_buffer(addr,reg,len,buf))
	{
		return 1;
	}

	return 0;
}


uint16_t i2cGetErrorCounter(void)
{
    // TODO maybe fix this, but since this is test code, doesn't matter.
    return 0;
}
