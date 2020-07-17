#include "stm32_i2c.h"
#include "delay.h"
#include "stdio.h"
/*********************

����stm32��i2c�ӿڣ�ʹ��i2c��صĺ�������������
.h�ļ�������i2c�Ľӿڣ�SCLΪPA3��SDAΪPA2��Xδʹ��������
����Gabonlee

*********************/
void i2c_delay(void)//��ʱ�������ƺ�ûʲô��
{
    //delay_ms(2);
    int i = 30;
    while(i--);
}
//1:fail  0:success
uint8_t i2c_start(void)//��ʼi2cͨ��
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

void i2c_stop(void)//ֹͣi2cͨ��
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

void i2c_init(void)//STM32��i2c��ʼ�����˿ڶ�����.h�ļ�����
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ע���޸�ʹ��ʱ�ӵĶ˿ڣ�Ŀǰ��GPIOA��
    
    GPIO_InitStructure.GPIO_Pin = I2C_PIN_SCL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//����Ҫ50MHz������ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//SCL�������
    GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);

   /* GPIO_InitStructure.GPIO_Pin = I2C_PIN_X;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(I2C_X_GPIO, &GPIO_InitStructure);
    */
    GPIO_InitStructure.GPIO_Pin = I2C_PIN_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//ͬ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//SDA��©���
    GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);
    
	/*����SCL��SDA���컹�ǿ�©�����⣺
	������������£�SCL�����ÿ�©��Ҳ���������죻
	����ʲô�����SDA���뿪©��
	��©Ҫ���������������VCC
	���ģ��ʵ��SCL��SDA�����������裬���˿�����ʱ��ѵ�ƽ����2.1V���ң���Ƭ����ʶ��Ϊ�ߵ�ƽ
	���ֱ�ӿ�©�Ϳ�����
	�ο���https://blog.csdn.net/qq_31073871/article/details/86593925
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
