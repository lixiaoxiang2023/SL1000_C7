/*******************************************************************************
文件名称：   sys_updata.c
项目名称：   事业一部技术平台
创建者：     lixx
创建日期：   2020.10.7
程序用途：   用于SPI FLASH固件升级读写逻辑及处理

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devinit.h"
#include "sys_updata.h"
#include "ff.h"
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
Constant Definition
*******************************************************************************/
__no_init unsigned long firmware_update_flag@0x20000000;

//8 +4
static const char DownloadPath[]="UPDATA.BIN";
static const char DownloadFinishPath[]="UPFLG";


/*******************************************************************************
全局变量定义
*******************************************************************************/
extern FATFS               fs;             // 文件系统对象,必须使用全局变量或者静态全局变量，跟存储位置相关，主要是FATFS对象中保存了相关文件系统的信息
extern FIL                 g_fd;           // 文件描述符

/*******************************************************************************
内部函数申明
*******************************************************************************/


/*******************************************************************************
外部变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 写固件帧到SPI FLASH
参数描述：
*   @ unsigned char *buffer, //写入数据地址
*   @ unsigned short usLen,  //写入数据长度
*   @ unsigned int uiFilesize//写入数据 文件大小
返回值描述：
*   @ 无
说明：
*   @ 将buffer 传入的数据写入SPI FLASH中保存
*******************************************************************************/
DOWNLOAD_ST FirmWareWriteFrame(unsigned char *buffer,unsigned short usLen,unsigned int uiFilesize)
{
    FRESULT res =FR_OK;
    static UINT br=0;
    static unsigned int adr_cnt = 0;

    if(adr_cnt == 0)
    {
        f_unlink(DownloadPath);
        res = f_open(&g_fd, DownloadPath, FA_WRITE | FA_READ);
        if(res == FR_NO_FILESYSTEM)
        {
            res = f_mkfs(0, 0, 512);   //格式化磁盘
            res = f_open(&g_fd, DownloadPath, FA_WRITE | FA_READ);
        }

        if(res == FR_NO_FILE)
        {
            res = f_open(&g_fd,DownloadPath, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
        }
        else if(res != FR_OK)
        {
            return FILE_FORMAT_ERR;
        }

        if(res == FR_OK)
        {
            f_write(&g_fd,&uiFilesize,4,(void *)&br); //查看文件数据是否正确
        }

    }

    if(res == FR_OK)
    {
        res = f_write(&g_fd,buffer,usLen,(void *)&br); //查看文件数据是否正确
    }
    adr_cnt += br;

    if(adr_cnt>=FILE_MAX_LEN ||adr_cnt>=uiFilesize)
    {
        f_close(&g_fd);
        adr_cnt = 0;
        return FILE_DOWNLOAD_FINISH;
    }


    if(br==0 ||res != FR_OK)
    {
        f_close(&g_fd);
        adr_cnt = 0;
        return FILE_WRITE_ERR;
    }

    return FILE_OK;

}

/*******************************************************************************
函数功能：
*   @ 读SPI FLASH中固件程序
参数描述：
*   @ unsigned char *buffer, //读数据缓存地址
*   @ unsigned short usLen,  //读数据长度
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
DOWNLOAD_ST FirmWareReadFrame(unsigned char *buffer,unsigned short usLen)
{
    FRESULT res;
    static unsigned int cnt=0;
    static unsigned int uiFilesize=0;
    UINT br;

    taskENTER_CRITICAL();

    res = f_open(&g_fd,DownloadPath,FA_READ);

    if(res != FR_OK) return FILE_FORMAT_ERR;

    if(cnt !=0)
    {
        res = f_lseek(&g_fd, cnt*usLen); //移动文件指针
    }
    else
    {
        f_read(&g_fd,&uiFilesize,4,(void *)&br); //查看文件数据是否正确
    }

    res = f_read(&g_fd, buffer,usLen,(void *)&br);	    //查看文件数据是否正确
    f_close(&g_fd);
    taskEXIT_CRITICAL();

    cnt++;
    if(cnt>=(FILE_MAX_LEN/usLen)||cnt>=uiFilesize)
    {
        cnt = 0;

        return FILE_READ_FINISH;
    }
    return FILE_READ_OK;
}


/*******************************************************************************
函数功能：
*   @ 固件写入分帧处理
参数描述：
*   @ __StDownLoad strDownLoadData 固件信息
*   @ unsigned char* ucHandFlg     写文件数据帧
返回值描述：
*   @ 固件烧录完成返回1 否则返回0
说明：
*   @ 无
*******************************************************************************/
DOWNLOAD_ST DownLoadFile(__StDownLoad strDownLoadData,unsigned char* ucHandFlg)
{
    unsigned int uiPollCx;
    DOWNLOAD_ST ucret = FILE_OK;

    taskENTER_CRITICAL();

    uiPollCx = strDownLoadData.uiFilesize/LOAD_UNIT_LEN;

    if(strDownLoadData.uiFrameID<=uiPollCx)
    {
        if( *ucHandFlg != strDownLoadData.uiFrameID
           && strDownLoadData.uiFilesize>=LOAD_UNIT_LEN)
        {
            ucret = FirmWareWriteFrame(strDownLoadData.strBuffer,LOAD_UNIT_LEN
                                       ,strDownLoadData.uiFilesize);

            *ucHandFlg = strDownLoadData.uiFrameID;
        }
    }
    else if(strDownLoadData.uiFilesize%LOAD_UNIT_LEN !=0)
    {
        ucret = FirmWareWriteFrame(strDownLoadData.strBuffer
                                   ,strDownLoadData.uiFilesize%LOAD_UNIT_LEN,strDownLoadData.uiFilesize);
    }

    taskEXIT_CRITICAL();
    return ucret;
}

/*******************************************************************************
函数功能：
*   @ 读取固件
参数描述：
*   @ 测试用
返回值描述：
*   @ 无
说明：
*   @ unsigned char *buffer 读数据缓存 ,unsigned short usLen 度数据长度
*******************************************************************************/
unsigned char LoadFile(unsigned char *buffer,unsigned short usLen)
{
    DOWNLOAD_ST ucret=FILE_OK;

    ucret = FirmWareReadFrame(buffer,usLen);

    return (ucret==FILE_OK?2:0);//写成功继续 否则终止
}

/*******************************************************************************
函数功能：
*   @ 合法性解析
参数描述：
*   @ 分析下载固件合法性并将合法固件写入外置FLASH
返回值描述：
*   @ 无
说明：
*   @ __StDownLoad strDownLoadData, 固件内容及信息
*   @ unsigned char* ucHandFlg      烧录通信返回接收数据帧ID
*******************************************************************************/
unsigned char JumpUpdata(__StDownLoad strDownLoadData,unsigned char* ucHandFlg)
{
    FRESULT res =FR_OK;
    static UINT br=0;

    DOWNLOAD_ST ucret=FILE_OK;

    ucret = DownLoadFile(strDownLoadData,ucHandFlg);

    if(ucret == FILE_DOWNLOAD_FINISH)//写固件升级模式标志位 并软复位MCU
    {
        firmware_update_flag = 0x55AA55AA; //准备跳转到boot loader

        f_unlink(DownloadFinishPath);

        res = f_open(&g_fd, DownloadFinishPath, FA_WRITE | FA_READ);

        if(res == FR_NO_FILE)
        {
            res = f_open(&g_fd,DownloadFinishPath, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
        }
        else if(res != FR_OK)
        {
            return FILE_FORMAT_ERR;
        }

        if(res == FR_OK)
        {
            f_write(&g_fd,&firmware_update_flag,4,(void *)&br); //查看文件数据是否正确
        }
        f_close(&g_fd);

        //软复位
        __set_FAULTMASK(1); // 关闭所有中断
        NVIC_SystemReset();//软复位后接下来进入bootloader
    }

    return (ucret==FILE_OK?1:0);//写成功继续 否则终止
}

/*******************************************************************************
函数功能：
*   @ 获取升级完成标志
参数描述：
*   @ 获取升级完成标志判断是否升级成功
返回值描述：
*   @ 无
说明：

*******************************************************************************/

unsigned char GetUpdataFinish(void)
{
    unsigned char ret=0;
    if((*(__IO uint32_t*)USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS) == 0x55AA55AA)//用户程序下载成功标记有效才进入用户程序，避免烧写途中掉电导致不能启动用户程
    {
        ret = 1;
    }

    return ret;
}

