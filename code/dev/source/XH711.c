/************************************************************************************

*************************************************************************************/
#include "XH711.h"
#include "devinit.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

u32 HX711_Buffer;
u32 Weight_Maopi;
static s32 Weight_Shiwu;
u8 Flag_Error = 0;

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
//#define GapValue 106.5
#define GapValue 1100//1840

void Init_HX711pin(void)
{				  
    DevIo->Config(PB, 8, 1, Mode_Out_PP); //scl
    DevIo->Config(PB, 9, 1, Mode_IPU); //dout
    DevIo->Write(PB, 8, 0);//初始化设置为0
}

static void delay_us(void)
{
    for (__IO uint8_t i = 0; i < 16; i++) {
        ;
    }
}

//****************************************************
//读取HX711
//****************************************************
u32 HX711_Read(void)	//增益128
{
	unsigned long count; 
	unsigned char i; 
  	//HX711_DOUT_SET; 
    DevIo->Write(1, 9, 1);
	delay_us();
  	//HX711_SCK_RESET; 
    DevIo->Write(1, 8, 0);
  	count = 0; 
    //  	while(HX711_DOUT_SET); 
    while( DevIo->Read(1, 9)); 
  	for(i = 0; i < 24; i++)
	{ 
	  	//HX711_SCK_SET; 
        DevIo->Write(1, 8, 1);
	  	count = count<<1; 
		delay_us();
		//HX711_SCK_RESET; 
        DevIo->Write(1, 8, 0);
	  	//if(HX711_DOUT_SET)
        if(DevIo->Read(1, 9))
			count++; 
		delay_us();
	} 
 	//HX711_SCK_SET; 
    DevIo->Write(1, 8, 1);
    count = count^0x800000;//第25个脉冲下降沿来时，转换数据
	delay_us();
	//HX711_SCK_RESET;  
    DevIo->Write(1, 8, 0);
	return(count);
}

//****************************************************
//获取毛皮重量
//****************************************************
u32 Get_Maopi(void)
{
	return Weight_Maopi = HX711_Read();	
} 

//****************************************************
//称重
//****************************************************
u32 Get_Weight(u32 wTraeData)
{
	HX711_Buffer = HX711_Read();
	if(HX711_Buffer < wTraeData)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = wTraeData - Weight_Shiwu;				//获取实物的AD采样数值。
		Weight_Shiwu = (s32)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量//因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。																	//当发现测试出来的重量偏大时，增加该数值。	//如果测试出来的重量偏小时，减小改数值。
	}	
    return Weight_Shiwu;
}
