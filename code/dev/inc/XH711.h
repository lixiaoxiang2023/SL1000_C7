#ifndef __HX711_H
#define __HX711_H

#include "stm32f10x.h"

#define HX711_SCK_SET DevIo->Write(1, 8, 1)
#define HX711_SCK_RESET DevIo->Write(1, 8, 0)
#define HX711_DOUT_SET DevIo->Write(1, 9, 1)
#define HX711_DOUT_RESET DevIo->Write(1, 9, 0)


extern void Init_HX711pin(void);
u32 HX711_Read(void);
extern u32 Get_Maopi(void);
extern u32 Get_Weight(u32 wTraeData);

//extern u32 HX711_Buffer;
//extern u32 Weight_Maopi;
//extern s32 Weight_Shiwu;
//extern u8 Flag_Error;

#endif