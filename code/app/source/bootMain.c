/*******************************************************************************
文件名称：   bootMain.c
项目名称：   事业一部技术平台
创建者：     xuqiang
创建日期：   2020.8.8
程序用途：   用于编译atm32 iar的boot代码

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：lixx
修改内容：增加在线升级功能
日期：
*******************************************************************************/
//#define SYS_FILE 1

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devinit.h"
#include "bootMain_Cfg.h"
#include "FlashDrive.h"
#if SYS_FILE
#include "ff.h"
#else
//  #include "bsp_can.h"
#endif

/*******************************************************************************
Constant Definition
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
内部函数申明
*******************************************************************************/
void bootMain(void);
//void main(void);
void ERROR_Handler(void);

/*******************************************************************************
外部变量定义
*******************************************************************************/
#if(PLAT_STM32_F1)
extern __weak void CAN1_RX1_IRQHandler();
extern __weak void CAN1_SCE_IRQHandler();
extern __weak void USB_HP_CAN1_TX_IRQHandler();
extern __weak void USB_LP_CAN1_RX0_IRQHandler();
extern __weak void TIM2_IRQHandler();
#elif PLAT_STM32_F4
extern __weak void CAN1_TX_IRQHandler();
extern __weak void CAN1_RX0_IRQHandler();
extern __weak void CAN1_RX1_IRQHandler();
extern __weak void CAN1_SCE_IRQHandler();
extern __weak void TIM2_IRQHandler();
#endif
extern __weak void __iar_init_core( void );
extern __weak void __iar_init_vfp( void );
extern __weak void __low_level_init(void);
extern __weak void __iar_data_init2(void);
extern void bootXcanProtocolInit(void);

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef  void (*pFunction)(void);
typedef union { pFunction __fun; void * __ptr;} intvec_elem;
pFunction JumpToApplication;

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
__no_init static unsigned long bootfirmware_update_flag@0x20000000;

#if SYS_FILE
static FATFS               boot_fs;             // 文件系统对象,必须使用全局变量或者静态全局变量，跟存储位置相关，主要是FATFS对象中保存了相关文件系统的信息
static FIL                 boot_g_fd;           // 文件描述符
static const char DownloadPath[]="UPDATA.BIN";
static const char DownloadFinishPath[]="UPFLG";
#endif

static unsigned long firmware_mcu_flash_addr = APP_START_ADDR;

#if(PLAT_STM32_F1)
#pragma segment = "CSTACK"
#pragma location = ".intvec_boot"
const intvec_elem BootIntvec[] ={
    //  (void (*)(void))0x20030000,
    {.__ptr = __sfe( "CSTACK" )},
    bootMain,
    //main,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    0,0,0,0,
    ERROR_Handler,
    ERROR_Handler,
    0,
    ERROR_Handler,
    ERROR_Handler,

    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,

    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,

    USB_HP_CAN1_TX_IRQHandler,
    USB_LP_CAN1_RX0_IRQHandler,
    CAN1_RX1_IRQHandler,
    CAN1_SCE_IRQHandler,

    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    TIM2_IRQHandler
};

#elif PLAT_STM32_F4
#pragma segment = "CSTACK"
#pragma location = ".intvec_boot"
const intvec_elem BootIntvec[] ={
    {.__ptr = __sfe( "CSTACK" )},
    bootMain,
    //main,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    0,0,0,0,
    ERROR_Handler,
    ERROR_Handler,
    0,
    ERROR_Handler,
    ERROR_Handler,

    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,

    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    CAN1_TX_IRQHandler ,
    CAN1_RX0_IRQHandler,
    CAN1_RX1_IRQHandler,
    CAN1_SCE_IRQHandler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    ERROR_Handler,
    TIM2_IRQHandler
};
#endif

/*******************************************************************************
函数功能：
*   @ 错误接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：出错死循环
*   @ 无
*******************************************************************************/
void ERROR_Handler(void)
{
    while(1) {}
}

/*******************************************************************************
函数功能：
*   @ 声明IAR初始化函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说   明：
*   @ 无
*******************************************************************************/
#if SYS_FILE
static int bootMain_SysPareInit(void)
{
    f_mount(0, &boot_fs);   // 为逻辑驱动器注册工作区
    return 0;
}

/*******************************************************************************
函数功能：
*   @ flash初始化
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：无
*******************************************************************************/
static unsigned char bootMain_SpiFlash_Init(void)
{
    DevIo->Config(PB, 3, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);      // SPI1_SCK
    DevIo->Config(PB, 4, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);      // SPI1_SO
    DevIo->Config(PB, 5, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);      // SPI1_SI
    DevIo->Config(PA, 15, 1, Mode_OUT,OType_PP, Speed_100MHz,  PuPd_NOPULL);     // SPI1_CS
    DevIo->Write(PA, 15, 1);
    DevIo->PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);                   // SPI1_SCK
    DevIo->PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);                   // SPI1_SO
    DevIo->PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);                   // SPI1_SI

    At45InitTdf stAt45Init={
        0,//SPI1
        0,
        0,
        0,
        0,
        30*1000*1000,
        &BIT_GPIOA_ODR_ODR15,
    };
    DevAt45 = At45Init(&stAt45Init);
    return 0;
}

#endif
//基于文件系统的固件升级处理
#if SYS_FILE
unsigned char bootMain_Updata(void)
{
    unsigned int read_bytes,wr_bytes;
    unsigned char ucstrReadBuffer[1024]={0};
    unsigned int uifirmware_size=0;
	unsigned int firmware_valid_spi_flag;

    FRESULT res;
    UINT br;
    //查看是否有固件升级完整文件
    res = f_open(&boot_g_fd,DownloadFinishPath,FA_READ);
    //无升级文件跳转到APP
    if(res != FR_OK)
    {
        bootfirmware_update_flag = 0;//清除固件更新标志避免陷入更新死循环
        JumpToApplication  = (pFunction)(*(vu32*) (APP_START_ADDR + 4));
        __set_MSP(*(vu32*) APP_START_ADDR);
        __set_PSP(*(vu32*) APP_START_ADDR);
        JumpToApplication();

        return 1;
    }
    res = f_read(&boot_g_fd, &firmware_valid_spi_flag,4,(void *)&br);       //查看文件数据是否正确
    f_close(&boot_g_fd);

    if(firmware_valid_spi_flag == 0x55AA55AA)
    {
        res = f_open(&boot_g_fd,DownloadPath,FA_READ);
        //无升级文件跳转到APP
        if(res != FR_OK)
        {
            bootfirmware_update_flag = 0;//清除固件更新标志避免陷入更新死循环
            JumpToApplication  = (pFunction)(*(vu32*) (APP_START_ADDR + 4));
            __set_MSP(*(vu32*) APP_START_ADDR);
            __set_PSP(*(vu32*) APP_START_ADDR);
            JumpToApplication();

            return 1;
        }
        res = f_read(&boot_g_fd, &uifirmware_size,4,(void *)&br);       //读固件升级文件大小

        if((uifirmware_size<=USER_APP_MAX_SIZE)&&(uifirmware_size>=USER_APP_MIN_SIZE))
        {
            /* Unlock the Flash Bank1 Program Erase controller */
            FLASH_If_Init();
            FLASH_If_Erase(firmware_mcu_flash_addr,USER_APP_MAX_SIZE/FLASH_PAGE_SIZE);//清存储空间

            for(unsigned int i=0;i<uifirmware_size;)
            {
                read_bytes = (uifirmware_size - i);
                if(read_bytes >  1024) read_bytes = 1024;
                res = f_read(&boot_g_fd, ucstrReadBuffer,read_bytes,(void *)&br);       //查看文件数据是否正确
                if(read_bytes%4 != 0)//never happen
                    wr_bytes = read_bytes + 4;//编程按4字节的整数倍
                else
                    wr_bytes = read_bytes;

                if(0 == FLASH_If_Write((__IO unsigned int*)&firmware_mcu_flash_addr, (unsigned int*)(ucstrReadBuffer) ,(wr_bytes>>2)))
                {
                    i += read_bytes;//每次读取1KB
                }
                else//FLASH写异常，放弃更新-
                {
                    bootfirmware_update_flag = 0;//清除固件更新标志避免陷入更新死循环
                    f_close(&boot_g_fd);
                    //清除SPI FLASH中对应的新固件标志---避免重复更新
                    f_unlink(DownloadPath);
                    f_unlink(DownloadFinishPath);
                    //软复位
                    __set_FAULTMASK(1); // 关闭所有中断
                    NVIC_SystemReset();//软复位后接下来进入bootloader
                }
            }
            FLASH_Unlock();
            //写入用户固件下载成功标记
            FLASH_ProgramWord(USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS,0x55AA55AA);//
            FLASH_Lock();
        }
    }
    bootfirmware_update_flag = 0;//清除固件更新标志避免陷入更新死循环
    //清除文件避免循环使用
    f_unlink(DownloadPath);
    f_unlink(DownloadFinishPath);
    //软复位
    __set_FAULTMASK(1); // 关闭所有中断
    NVIC_SystemReset();//软复位后接下来进入bootloader

    return 0 ;
}
#else
//无文件系统方式在线升级MCU
unsigned char UpdataWrite(_c7_src_dev *stCanData)
{
    unsigned int read_bytes,wr_bytes;
    unsigned char ucstrReadBuffer[512]={0};
    unsigned int uifirmware_size=0;
    static unsigned int read_bytesAll=0;
    if(stCanData->stOut.byHandFlg > stCanData->stDownload.uiFrameID)
    {
        stCanData->stOut.byHandFlg = 0;
        stCanData->stOut.byUpdataFinish = 0;
        stCanData->stOut.byUpdataReady = 0;
        read_bytesAll = 0;
        firmware_mcu_flash_addr = APP_START_ADDR;
    }
    if(stCanData->stOut.byUpdataReady ==0)
    {
        FLASH_If_Init();
        //  FLASH_If_Erase(firmware_mcu_flash_addr,10);//清存储空间
        FLASH_If_Erase(firmware_mcu_flash_addr,USER_APP_MAX_SIZE/FLASH_PAGE_SIZE);//清存储空间
        stCanData->stOut.byUpdataReady = 1;
    }

    uifirmware_size = stCanData->stDownload.uiFilesize;

    if((uifirmware_size<=USER_APP_MAX_SIZE)&&(uifirmware_size>=USER_APP_MIN_SIZE))
    {

        if((stCanData->stOut.byHandFlg != stCanData->stDownload.uiFrameID)
           && (uifirmware_size>read_bytesAll))
        {
            read_bytes = (uifirmware_size - read_bytesAll);
            if(read_bytes >  512) read_bytes = 512;
            memcpy(&ucstrReadBuffer,&stCanData->stDownload.byCanRx,read_bytes);
            if(read_bytes%4 != 0)//never happen
                wr_bytes = read_bytes + 4;//编程按4字节的整数倍
            else
                wr_bytes = read_bytes;

            if(0 == FLASH_If_Write((__IO unsigned int*)(&firmware_mcu_flash_addr), (unsigned int*)(ucstrReadBuffer) ,(wr_bytes>>2)))
            {
                read_bytesAll += read_bytes;//每次读取1KB
                stCanData->stOut.byHandFlg = stCanData->stDownload.uiFrameID;
            }
            else//FLASH写异常，放弃更新-
            {
                bootfirmware_update_flag = 0;//清除固件更新标志避免陷入更新死循环
                stCanData->stOut.byUpdataReady = 0;
                //软复位
                __set_FAULTMASK(1); // 关闭所有中断
                NVIC_SystemReset();//软复位后接下来进入bootloader
            }
        }

        if(uifirmware_size<=read_bytesAll)
        {
            FLASH_Unlock();
            //写入用户固件下载成功标记
            FLASH_If_Erase(USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS,(unsigned char)(USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS+0x800)/FLASH_PAGE_SIZE);//清存储空间
            FLASH_ProgramWord(USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS,0x55AA55AA);//
            FLASH_Lock();
            bootfirmware_update_flag = 0;
            stCanData->stOut.byUpdataFinish = 1;
            stCanData->stOut.byUpdataReady = 0;
            return 1;
        }
    }
    else if(uifirmware_size !=0)
    {
        bootfirmware_update_flag = 0;//清除固件更新标志避免陷入更新死循环
        //软复位
        __set_FAULTMASK(1); // 关闭所有中断
        NVIC_SystemReset();//软复位后接下来进入bootloader
    }
    return 0;
}

unsigned char bootMain_Updata(void)
{
    DevIo = IoInit();
    DevTim = Tim2Init();
    DevCan = CanInit2(CONFIG_CAN_BOUD_NEW_RATE);
    XcanTrsInit();
    bootXcanProtocolInit();                                                         // Xcan 初始化
    DEV_NAME.stIn.byCmd = FIRMWARE_UPDATA_START;
//    DEV_NAME.stIn.byCmdCnt = 1;
    while(1)
    {
        DevXcan->Service();
        DEV_NAME.stOut.stoutCmd.byCmd = DEV_NAME.stIn.byCmd;
        DEV_NAME.stOut.stoutCmd.byCmdCnt = DEV_NAME.stIn.byCmdCnt;
        if(DEV_NAME.stOut.stoutCmd.byCmd == FIRMWARE_UPDATA_START)
        {
            if(UpdataWrite(&DEV_NAME)==1)
            {
                __set_FAULTMASK(1); // 关闭所有中断
                NVIC_SystemReset();//软复位后接下来进入bootloader
            }
        }
    }
}
#endif

/*******************************************************************************
函数功能：
*   @ 升级主接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：主函数
*   @ 无
*******************************************************************************/
void bootMain(void)
//void main(void)
{
    if (BootIntvec[1].__fun == NULL) {
        ERROR_Handler();
    }
    SystemInit();
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
   // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);
    __iar_init_core();
    __iar_init_vfp();
    __low_level_init();
    __iar_data_init2();
#if SYS_FILE
    DevIo = IoInit();
    DevSpi = SpiInit();
    bootMain_SpiFlash_Init();    //SPI FLASH 初始化
    bootMain_SysPareInit();//挂载文件系统
#endif
    if( bootfirmware_update_flag != 0x55AA55AA) //无固件升级标志
    {
        if (((*(__IO uint32_t*)APP_START_ADDR) & 0x2FFE0000 ) == 0x20000000)
        { //无完整固件升级文件 跳转到APP
#if SYS_FILE
            if(f_open(&boot_g_fd,DownloadFinishPath,FA_READ) != FR_OK)
#else
                if((*(__IO uint32_t*)USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS) == 0x55AA55AA
                   || (*(__IO uint32_t*)USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS) == 0xFFFFFFFF)//用户程序下载成功标记有效才进入用户程序，避免烧写途中掉电导致不能启动用户程序
#endif
                {
                    if(((*(__IO uint32_t*)USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS) == 0xFFFFFFFF)
                       && (*(__IO uint32_t*)(USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS+4)) == 0xAA55AA55)
                    {
                        bootfirmware_update_flag = 0x55AA55AA;
                    }
                    else
                    {
                        JumpToApplication  = (pFunction)(*(vu32*) (APP_START_ADDR + 4));
                        __set_MSP(*(vu32*) APP_START_ADDR);
                        __set_PSP(*(vu32*) APP_START_ADDR);
                        JumpToApplication();
                    }
                }
                else
                {
                    bootfirmware_update_flag = 0x55AA55AA;
                }
        }
        else
        {
            bootfirmware_update_flag = 0x55AA55AA;
        }
    }
    if(bootfirmware_update_flag == 0x55AA55AA)//升级程序
    {
        bootMain_Updata();
    }
    while(1) {;}
}

