/*******************************************************************************
文件名称：   blc.c
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2020.3.18
程序用途：   用于nandflash的坏换管理

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
修改记录：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include <string.h>
#include "blc.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/
static unsigned char s_BlcTable[BLC_TABLE_SIZE];

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 创建表
参数描述：
*   @ u32 dwBlcS 替换表格的开始块编号
*   @ u32 dwBlcE 替代表格的结束表编号
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
const u8* BlcCreate(u32 dwBlcS, u32 dwBlcE)
{
    stBlcH *pstBlcH = (stBlcH *)s_BlcTable;

    memset(s_BlcTable, 0, BLC_TABLE_SIZE);
    pstBlcH->dwMigc = BLC_MIGC_NUM;
    pstBlcH->dwBlkS = dwBlcS;
    pstBlcH->dwBlkE = dwBlcE;
    pstBlcH->dwBlkP = 0;

    return (const u8*) s_BlcTable;
}

/*******************************************************************************
函数功能：
*   @ 获取表的内容
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
u8* BlcContex(void)
{
    return (u8*) s_BlcTable;
}

/*******************************************************************************
函数功能：
*   @ 坏块管理初始换
参数描述：
*   @ 无
返回值描述：
串口设备提供的操作接口
说明：
*   @ 掉用此接口完成设备的初始化
*******************************************************************************/
int BlcInit(u16 *pwTab)
{
    u16 *pwCp = (u16*) s_BlcTable;
    stBlcH *pstBlcH = (stBlcH *)s_BlcTable;

    if(pwTab) {
        for(int i = 0; i < (BLC_TABLE_SIZE >> 1); i ++) *pwCp ++ = *pwTab ++;
    }

    // cheak
    if(pstBlcH->dwMigc != BLC_MIGC_NUM) return -1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 检查坏块映射
参数描述：
*   @ 检查块编号
返回值描述：
*   @ 新的块编号
说明：
*   @ 无
*******************************************************************************/
u32 BlcCheak(u32 dwBlkNum)
{
    stBlcH *pstBlcH = (stBlcH *)s_BlcTable;
    stBlcSwap *pstSwap = (stBlcSwap *)s_BlcTable[sizeof(pstBlcH)];

    if(pstBlcH->dwBlkP == 0 || pstSwap->wOld == 0) return dwBlkNum;
    for(; pstSwap->wOld != 0 ; pstSwap ++) {
        if(pstSwap->wOld == dwBlkNum) return pstSwap->wNew;
    }

    return dwBlkNum;
}

/*******************************************************************************
函数功能：
*   @ 检查坏块映射
参数描述：
*   @ 检查块编号
返回值描述：
*   @ 0 错误 其他 ok
说明：
*   @ 无
*******************************************************************************/
u32 BlcInsert(u32 dwBlkNum)
{
    stBlcH *pstBlcH = (stBlcH *)s_BlcTable;
    stBlcSwap *pstSwap = (stBlcSwap *)s_BlcTable[sizeof(pstBlcH)];

    if(pstBlcH->dwMigc != BLC_MIGC_NUM) return 0;
    if(pstBlcH->dwBlkS == pstBlcH->dwBlkE) return 0;
    if(pstBlcH->dwBlkS + pstBlcH->dwBlkP >= (pstBlcH->dwBlkE - 1)) return 0;

    // 无坏块
    if(pstSwap->wOld == 0) {
        pstSwap->wOld = dwBlkNum;
        pstSwap->wNew = pstBlcH->dwBlkS + pstBlcH->dwBlkP;
        pstBlcH->dwBlkP ++;

        return pstSwap->wNew;
    }

    // 检查是否是交换块出现坏块,同时坏块是否在交换区中
    for(; pstSwap->wOld != 0; pstSwap ++) {
        if(pstSwap->wNew == dwBlkNum) {
            pstSwap->wNew = pstBlcH->dwBlkS + pstBlcH->dwBlkP;
            pstBlcH->dwBlkP ++;

            return pstSwap->wNew;
        }
    }

    pstSwap->wOld = dwBlkNum;
    pstSwap->wNew = pstBlcH->dwBlkS + pstBlcH->dwBlkP;
    pstBlcH->dwBlkP ++;

    return pstSwap->wNew;
}

