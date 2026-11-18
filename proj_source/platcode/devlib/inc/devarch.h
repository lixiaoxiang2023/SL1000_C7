/*******************************************************************************
文件名称： devarch.h
文件目录： $PROJ_DIR$/usr/Drive/inc
项目名称： 事业一部通用技术平台
创建者：   胥强
创建日期： 2021.07.09
程序用途： 归一化不同的平台

版权声明： Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __DEVARCH_H
#define __DEVARCH_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dev_config.h"
#include "configinit.h"

#if(PLAT_STM32_F1)
    #include "stm32f10x.h"
#endif

#if(PLAT_STM32_F2)
    #include "stm32f2xx.h"
#endif

#if(PLAT_STM32_F4)
    #include "stm32f4xx.h"
#endif

#if(PLAT_GD32_F30X)
    #include "gd32f30x.h"
#endif

#endif
