#include "main.h"
#include "sys.h"
void BH1750_init(void);
void BH1750_WriteReg(uint8_t reg_add,uint8_t reg_dat);
void BH1750_ReadData(uint8_t reg_add,unsigned char*Read,uint8_t num);
void get_LightData_Hex(void );
extern uint32_t LightData_Hex;
