/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* by grqd_xp                                                            */
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include "devinit.h"

#include "usb_conf.h"
#include "diskio.h"
#include "usbh_msc_core.h"

#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

extern USB_OTG_CORE_HANDLE              USB_OTG_Core;
extern USBH_HOST                        USB_OTG_Host;
/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/* Note that Tiny-FatFs supports only single drive and always            */
/* accesses drive number 0.                                              */

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
                         BYTE drv				/* Physical drive nmuber (0..) */
                             )
{
    if(drv == 1){
        if(HCD_IsDeviceConnected(&USB_OTG_Core))
        {
            Stat &= ~STA_NOINIT;
        }

        return Stat;
    }

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
                     BYTE drv		/* Physical drive nmuber (0..) */
                         )
{
	if(drv == 1){
        return Stat;
    }

    return 0;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
                   BYTE drv,		/* Physical drive nmuber (0..) */
                   BYTE *buff,		/* Data buffer to store read data */
                   DWORD sector,	/* Sector address (LBA) */
                   BYTE count		/* Number of sectors to read (1..255) */
                       )
{
    int iTimOut = 0;
    if(drv == 1) {
        BYTE status = USBH_MSC_OK;
        if ((drv > 1) || !count) return RES_PARERR;
        if (Stat & STA_NOINIT) return RES_NOTRDY;

        if(HCD_IsDeviceConnected(&USB_OTG_Core)) {
            do{
                status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,USBH_MSC_Param.MSPageLength * count);
                USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_OTG_Host);

                if(!HCD_IsDeviceConnected(&USB_OTG_Core)) return RES_ERROR;
                if(iTimOut ++ > 0x5fffff) return RES_ERROR;
                //vTaskDelay(20);
            } while(status == USBH_MSC_BUSY );
        }

        if(status == USBH_MSC_OK) return RES_OK;
        return RES_ERROR;
    }
    else {
        BYTE *pBuf = buff;
        DevAt45->ReadPage(sector,pBuf,512);
        return RES_OK ;
    }
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
                    BYTE drv,			/* Physical drive nmuber (0..) */
                    const BYTE *buff,	/* Data to be written */
                    DWORD sector,		/* Sector address (LBA) */
                    BYTE count			/* Number of sectors to write (1..255) */
                        )
{
    int iTimOut = 0;
    if(drv == 1){
        BYTE status = USBH_MSC_OK;
        if ((drv > 1) || !count) return RES_PARERR;
        if (Stat & STA_NOINIT) return RES_NOTRDY;
        if (Stat & STA_PROTECT) return RES_WRPRT;

        if(HCD_IsDeviceConnected(&USB_OTG_Core)) {
            do{
                status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector,USBH_MSC_Param.MSPageLength * count);
                USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_OTG_Host);

                if(!HCD_IsDeviceConnected(&USB_OTG_Core)) return RES_ERROR;
                if(iTimOut ++ > 0x5fffff) return RES_ERROR;
                //vTaskDelay(15);
            } while(status == USBH_MSC_BUSY );
        }

        if(status == USBH_MSC_OK) return RES_OK;
        return RES_ERROR;
    }
    else {
        BYTE *pBuf = (BYTE*)buff;
        DevAt45->WritePage(sector,pBuf, 512);
        return RES_OK ;
    }
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
                    BYTE drv,		/* Physical drive nmuber (0..) */
                    BYTE ctrl,		/* Control code */
                    void *buff		/* Buffer to send/receive control data */
                        )
{
    switch (ctrl) {
	case CTRL_SYNC :
        return RES_OK;
	case GET_SECTOR_COUNT :
        //(*((DWORD*)buff) = 4096;
        *(DWORD*)buff = (drv == 1)?(DWORD) USBH_MSC_Param.MSCapacity:8192;
        return RES_OK;
    case GET_SECTOR_SIZE :
        //*((WORD*)buff) = 512;
        *(DWORD*)buff = (drv == 1)?(DWORD) USBH_MSC_Param.MSPageLength:512;
        return RES_OK;
	case GET_BLOCK_SIZE :
        *((DWORD*)buff) = (drv == 1)?512:1;
        return RES_OK;
	case CTRL_POWER :
		break;
	case CTRL_LOCK :
		break;
	case CTRL_EJECT :
		break;/* MMC/SDC command */
	case MMC_GET_TYPE :
		break;
	case MMC_GET_CSD :
		break;
	case MMC_GET_CID :
		break;
	case MMC_GET_OCR :
		break;
	case MMC_GET_SDSTAT :
		break;
	}
	return RES_PARERR;
}

DWORD get_fattime(void){
	return 0;
}

