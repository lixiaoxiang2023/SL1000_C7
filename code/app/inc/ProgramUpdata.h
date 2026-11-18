#ifndef __PROGRAM_UPDATA_H__
#define __PROGRAM_UPDATA_H__

uint8_t Program_Ready_Updata(void);
uint8_t Program_Updata_Data(void);
void Program_Updata_Start(void);

typedef  void (*pFunction)(void);

extern pFunction Jump_To_Application;

#endif