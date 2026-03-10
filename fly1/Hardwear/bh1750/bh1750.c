#include "bh1750.h"
#include "I2C.h"
#include "LED.h"
#include "FreeRTOS.h"
#include "task.h"

uint8_t DataBuff[2];//存储数组
float LightData;//光照强度（含小数位，单位lx）
uint32_t LightData_Hex;//光照强度（整数，单位lx)

void BH1750_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	HAL_I2C_Mem_Write(&hi2c3,0x46,reg_add,1,&reg_dat,1,1000);
}

//读取BH1750数据,设备地址0x46
void BH1750_ReadData(uint8_t reg_add,unsigned char*Read,uint8_t num)
{
 HAL_I2C_Mem_Read(&hi2c3,0x46,reg_add,1,Read,num,1000);
}

void BH1750_init(void)
{
		BH1750_WriteReg(0x01,0x00);// power on
		BH1750_WriteReg(0x10,0x00);//H- resolution mode
}
void get_LightData_Hex(void )
{
		BH1750_WriteReg(0x01,0x00);// power on
		BH1750_WriteReg(0x10,0x00);//H- resolution mode
		vTaskDelay(180);//等待180ms
		BH1750_ReadData(0,DataBuff,2);//读取数据
		LightData=((DataBuff[0]<<8)+DataBuff[1])/1.2f;//数据转换成光强度，单位lx
    LightData_Hex=LightData;//float转换成整数
}
