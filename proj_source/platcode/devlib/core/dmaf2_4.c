/*******************************************************************************
文件名称：   dma.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   201610308
程序用途：   用于dma的通用处理驱动

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20151123：
第一次由XQ创建初始程序。
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "dev.h"

#if(PLAT_STM32_F2|PLAT_STM32_F4)
#if(CONFIG_DMA1|CONFIG_DMA2)
/*******************************************************************************
常量定义
*******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                             DMA Controller                                 */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for DMA_SxCR register  *****************/
#define DMA_SxCR_CHSEL                       ((uint32_t)0x0E000000)
#define DMA_SxCR_CHSEL_0                     ((uint32_t)0x02000000)
#define DMA_SxCR_CHSEL_1                     ((uint32_t)0x04000000)
#define DMA_SxCR_CHSEL_2                     ((uint32_t)0x08000000)
#define DMA_SxCR_MBURST                      ((uint32_t)0x01800000)
#define DMA_SxCR_MBURST_0                    ((uint32_t)0x00800000)
#define DMA_SxCR_MBURST_1                    ((uint32_t)0x01000000)
#define DMA_SxCR_PBURST                      ((uint32_t)0x00600000)
#define DMA_SxCR_PBURST_0                    ((uint32_t)0x00200000)
#define DMA_SxCR_PBURST_1                    ((uint32_t)0x00400000)
#define DMA_SxCR_ACK                         ((uint32_t)0x00100000)
#define DMA_SxCR_CT                          ((uint32_t)0x00080000)
#define DMA_SxCR_DBM                         ((uint32_t)0x00040000)
#define DMA_SxCR_PL                          ((uint32_t)0x00030000)
#define DMA_SxCR_PL_0                        ((uint32_t)0x00010000)
#define DMA_SxCR_PL_1                        ((uint32_t)0x00020000)
#define DMA_SxCR_PINCOS                      ((uint32_t)0x00008000)
#define DMA_SxCR_MSIZE                       ((uint32_t)0x00006000)
#define DMA_SxCR_MSIZE_0                     ((uint32_t)0x00002000)
#define DMA_SxCR_MSIZE_1                     ((uint32_t)0x00004000)
#define DMA_SxCR_PSIZE                       ((uint32_t)0x00001800)
#define DMA_SxCR_PSIZE_0                     ((uint32_t)0x00000800)
#define DMA_SxCR_PSIZE_1                     ((uint32_t)0x00001000)
#define DMA_SxCR_MINC                        ((uint32_t)0x00000400)
#define DMA_SxCR_PINC                        ((uint32_t)0x00000200)
#define DMA_SxCR_CIRC                        ((uint32_t)0x00000100)
#define DMA_SxCR_DIR                         ((uint32_t)0x000000C0)
#define DMA_SxCR_DIR_0                       ((uint32_t)0x00000040)
#define DMA_SxCR_DIR_1                       ((uint32_t)0x00000080)
#define DMA_SxCR_PFCTRL                      ((uint32_t)0x00000020)
#define DMA_SxCR_TCIE                        ((uint32_t)0x00000010)
#define DMA_SxCR_HTIE                        ((uint32_t)0x00000008)
#define DMA_SxCR_TEIE                        ((uint32_t)0x00000004)
#define DMA_SxCR_DMEIE                       ((uint32_t)0x00000002)
#define DMA_SxCR_EN                          ((uint32_t)0x00000001)

/********************  Bits definition for DMA_SxCNDTR register  **************/
#define DMA_SxNDT                            ((uint32_t)0x0000FFFF)
#define DMA_SxNDT_0                          ((uint32_t)0x00000001)
#define DMA_SxNDT_1                          ((uint32_t)0x00000002)
#define DMA_SxNDT_2                          ((uint32_t)0x00000004)
#define DMA_SxNDT_3                          ((uint32_t)0x00000008)
#define DMA_SxNDT_4                          ((uint32_t)0x00000010)
#define DMA_SxNDT_5                          ((uint32_t)0x00000020)
#define DMA_SxNDT_6                          ((uint32_t)0x00000040)
#define DMA_SxNDT_7                          ((uint32_t)0x00000080)
#define DMA_SxNDT_8                          ((uint32_t)0x00000100)
#define DMA_SxNDT_9                          ((uint32_t)0x00000200)
#define DMA_SxNDT_10                         ((uint32_t)0x00000400)
#define DMA_SxNDT_11                         ((uint32_t)0x00000800)
#define DMA_SxNDT_12                         ((uint32_t)0x00001000)
#define DMA_SxNDT_13                         ((uint32_t)0x00002000)
#define DMA_SxNDT_14                         ((uint32_t)0x00004000)
#define DMA_SxNDT_15                         ((uint32_t)0x00008000)

/********************  Bits definition for DMA_SxFCR register  ****************/
#define DMA_SxFCR_FEIE                       ((uint32_t)0x00000080)
#define DMA_SxFCR_FS                         ((uint32_t)0x00000038)
#define DMA_SxFCR_FS_0                       ((uint32_t)0x00000008)
#define DMA_SxFCR_FS_1                       ((uint32_t)0x00000010)
#define DMA_SxFCR_FS_2                       ((uint32_t)0x00000020)
#define DMA_SxFCR_DMDIS                      ((uint32_t)0x00000004)
#define DMA_SxFCR_FTH                        ((uint32_t)0x00000003)
#define DMA_SxFCR_FTH_0                      ((uint32_t)0x00000001)
#define DMA_SxFCR_FTH_1                      ((uint32_t)0x00000002)

/********************  Bits definition for DMA_LISR register  *****************/
#define DMA_LISR_TCIF3                       ((uint32_t)0x08000000)
#define DMA_LISR_HTIF3                       ((uint32_t)0x04000000)
#define DMA_LISR_TEIF3                       ((uint32_t)0x02000000)
#define DMA_LISR_DMEIF3                      ((uint32_t)0x01000000)
#define DMA_LISR_FEIF3                       ((uint32_t)0x00400000)
#define DMA_LISR_TCIF2                       ((uint32_t)0x00200000)
#define DMA_LISR_HTIF2                       ((uint32_t)0x00100000)
#define DMA_LISR_TEIF2                       ((uint32_t)0x00080000)
#define DMA_LISR_DMEIF2                      ((uint32_t)0x00040000)
#define DMA_LISR_FEIF2                       ((uint32_t)0x00010000)
#define DMA_LISR_TCIF1                       ((uint32_t)0x00000800)
#define DMA_LISR_HTIF1                       ((uint32_t)0x00000400)
#define DMA_LISR_TEIF1                       ((uint32_t)0x00000200)
#define DMA_LISR_DMEIF1                      ((uint32_t)0x00000100)
#define DMA_LISR_FEIF1                       ((uint32_t)0x00000040)
#define DMA_LISR_TCIF0                       ((uint32_t)0x00000020)
#define DMA_LISR_HTIF0                       ((uint32_t)0x00000010)
#define DMA_LISR_TEIF0                       ((uint32_t)0x00000008)
#define DMA_LISR_DMEIF0                      ((uint32_t)0x00000004)
#define DMA_LISR_FEIF0                       ((uint32_t)0x00000001)

/********************  Bits definition for DMA_HISR register  *****************/
#define DMA_HISR_TCIF7                       ((uint32_t)0x08000000)
#define DMA_HISR_HTIF7                       ((uint32_t)0x04000000)
#define DMA_HISR_TEIF7                       ((uint32_t)0x02000000)
#define DMA_HISR_DMEIF7                      ((uint32_t)0x01000000)
#define DMA_HISR_FEIF7                       ((uint32_t)0x00400000)
#define DMA_HISR_TCIF6                       ((uint32_t)0x00200000)
#define DMA_HISR_HTIF6                       ((uint32_t)0x00100000)
#define DMA_HISR_TEIF6                       ((uint32_t)0x00080000)
#define DMA_HISR_DMEIF6                      ((uint32_t)0x00040000)
#define DMA_HISR_FEIF6                       ((uint32_t)0x00010000)
#define DMA_HISR_TCIF5                       ((uint32_t)0x00000800)
#define DMA_HISR_HTIF5                       ((uint32_t)0x00000400)
#define DMA_HISR_TEIF5                       ((uint32_t)0x00000200)
#define DMA_HISR_DMEIF5                      ((uint32_t)0x00000100)
#define DMA_HISR_FEIF5                       ((uint32_t)0x00000040)
#define DMA_HISR_TCIF4                       ((uint32_t)0x00000020)
#define DMA_HISR_HTIF4                       ((uint32_t)0x00000010)
#define DMA_HISR_TEIF4                       ((uint32_t)0x00000008)
#define DMA_HISR_DMEIF4                      ((uint32_t)0x00000004)
#define DMA_HISR_FEIF4                       ((uint32_t)0x00000001)

/********************  Bits definition for DMA_LIFCR register  ****************/
#define DMA_LIFCR_CTCIF3                     ((uint32_t)0x08000000)
#define DMA_LIFCR_CHTIF3                     ((uint32_t)0x04000000)
#define DMA_LIFCR_CTEIF3                     ((uint32_t)0x02000000)
#define DMA_LIFCR_CDMEIF3                    ((uint32_t)0x01000000)
#define DMA_LIFCR_CFEIF3                     ((uint32_t)0x00400000)
#define DMA_LIFCR_CTCIF2                     ((uint32_t)0x00200000)
#define DMA_LIFCR_CHTIF2                     ((uint32_t)0x00100000)
#define DMA_LIFCR_CTEIF2                     ((uint32_t)0x00080000)
#define DMA_LIFCR_CDMEIF2                    ((uint32_t)0x00040000)
#define DMA_LIFCR_CFEIF2                     ((uint32_t)0x00010000)
#define DMA_LIFCR_CTCIF1                     ((uint32_t)0x00000800)
#define DMA_LIFCR_CHTIF1                     ((uint32_t)0x00000400)
#define DMA_LIFCR_CTEIF1                     ((uint32_t)0x00000200)
#define DMA_LIFCR_CDMEIF1                    ((uint32_t)0x00000100)
#define DMA_LIFCR_CFEIF1                     ((uint32_t)0x00000040)
#define DMA_LIFCR_CTCIF0                     ((uint32_t)0x00000020)
#define DMA_LIFCR_CHTIF0                     ((uint32_t)0x00000010)
#define DMA_LIFCR_CTEIF0                     ((uint32_t)0x00000008)
#define DMA_LIFCR_CDMEIF0                    ((uint32_t)0x00000004)
#define DMA_LIFCR_CFEIF0                     ((uint32_t)0x00000001)

/********************  Bits definition for DMA_HIFCR  register  ****************/
#define DMA_HIFCR_CTCIF7                     ((uint32_t)0x08000000)
#define DMA_HIFCR_CHTIF7                     ((uint32_t)0x04000000)
#define DMA_HIFCR_CTEIF7                     ((uint32_t)0x02000000)
#define DMA_HIFCR_CDMEIF7                    ((uint32_t)0x01000000)
#define DMA_HIFCR_CFEIF7                     ((uint32_t)0x00400000)
#define DMA_HIFCR_CTCIF6                     ((uint32_t)0x00200000)
#define DMA_HIFCR_CHTIF6                     ((uint32_t)0x00100000)
#define DMA_HIFCR_CTEIF6                     ((uint32_t)0x00080000)
#define DMA_HIFCR_CDMEIF6                    ((uint32_t)0x00040000)
#define DMA_HIFCR_CFEIF6                     ((uint32_t)0x00010000)
#define DMA_HIFCR_CTCIF5                     ((uint32_t)0x00000800)
#define DMA_HIFCR_CHTIF5                     ((uint32_t)0x00000400)
#define DMA_HIFCR_CTEIF5                     ((uint32_t)0x00000200)
#define DMA_HIFCR_CDMEIF5                    ((uint32_t)0x00000100)
#define DMA_HIFCR_CFEIF5                     ((uint32_t)0x00000040)
#define DMA_HIFCR_CTCIF4                     ((uint32_t)0x00000020)
#define DMA_HIFCR_CHTIF4                     ((uint32_t)0x00000010)
#define DMA_HIFCR_CTEIF4                     ((uint32_t)0x00000008)
#define DMA_HIFCR_CDMEIF4                    ((uint32_t)0x00000004)
#define DMA_HIFCR_CFEIF4                     ((uint32_t)0x00000001)

/******************************************************************************/
/*                                                                            */
/*                         AHB Master DMA2D Controller (DMA2D)                */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for DMA2D_CR register  ******************/

#define DMA2D_CR_START                     ((uint32_t)0x00000001)               /*!< Start transfer */
#define DMA2D_CR_SUSP                      ((uint32_t)0x00000002)               /*!< Suspend transfer */
#define DMA2D_CR_ABORT                     ((uint32_t)0x00000004)               /*!< Abort transfer */
#define DMA2D_CR_TEIE                      ((uint32_t)0x00000100)               /*!< Transfer Error Interrupt Enable */
#define DMA2D_CR_TCIE                      ((uint32_t)0x00000200)               /*!< Transfer Complete Interrupt Enable */
#define DMA2D_CR_TWIE                      ((uint32_t)0x00000400)               /*!< Transfer Watermark Interrupt Enable */
#define DMA2D_CR_CAEIE                     ((uint32_t)0x00000800)               /*!< CLUT Access Error Interrupt Enable */
#define DMA2D_CR_CTCIE                     ((uint32_t)0x00001000)               /*!< CLUT Transfer Complete Interrupt Enable */
#define DMA2D_CR_CEIE                      ((uint32_t)0x00002000)               /*!< Configuration Error Interrupt Enable */
#define DMA2D_CR_MODE                      ((uint32_t)0x00030000)               /*!< DMA2D Mode */

/********************  Bit definition for DMA2D_ISR register  *****************/

#define DMA2D_ISR_TEIF                     ((uint32_t)0x00000001)               /*!< Transfer Error Interrupt Flag */
#define DMA2D_ISR_TCIF                     ((uint32_t)0x00000002)               /*!< Transfer Complete Interrupt Flag */
#define DMA2D_ISR_TWIF                     ((uint32_t)0x00000004)               /*!< Transfer Watermark Interrupt Flag */
#define DMA2D_ISR_CAEIF                    ((uint32_t)0x00000008)               /*!< CLUT Access Error Interrupt Flag */
#define DMA2D_ISR_CTCIF                    ((uint32_t)0x00000010)               /*!< CLUT Transfer Complete Interrupt Flag */
#define DMA2D_ISR_CEIF                     ((uint32_t)0x00000020)               /*!< Configuration Error Interrupt Flag */

/********************  Bit definition for DMA2D_IFCR register  ****************/

#define DMA2D_IFCR_CTEIF                   ((uint32_t)0x00000001)               /*!< Clears Transfer Error Interrupt Flag         */
#define DMA2D_IFCR_CTCIF                   ((uint32_t)0x00000002)               /*!< Clears Transfer Complete Interrupt Flag      */
#define DMA2D_IFCR_CTWIF                   ((uint32_t)0x00000004)               /*!< Clears Transfer Watermark Interrupt Flag     */
#define DMA2D_IFCR_CAECIF                  ((uint32_t)0x00000008)               /*!< Clears CLUT Access Error Interrupt Flag      */
#define DMA2D_IFCR_CCTCIF                  ((uint32_t)0x00000010)               /*!< Clears CLUT Transfer Complete Interrupt Flag */
#define DMA2D_IFCR_CCEIF                   ((uint32_t)0x00000020)               /*!< Clears Configuration Error Interrupt Flag    */

/* Legacy defines */
#define DMA2D_IFSR_CTEIF                   DMA2D_IFCR_CTEIF                     /*!< Clears Transfer Error Interrupt Flag         */
#define DMA2D_IFSR_CTCIF                   DMA2D_IFCR_CTCIF                     /*!< Clears Transfer Complete Interrupt Flag      */
#define DMA2D_IFSR_CTWIF                   DMA2D_IFCR_CTWIF                     /*!< Clears Transfer Watermark Interrupt Flag     */
#define DMA2D_IFSR_CCAEIF                  DMA2D_IFCR_CAECIF                    /*!< Clears CLUT Access Error Interrupt Flag      */
#define DMA2D_IFSR_CCTCIF                  DMA2D_IFCR_CCTCIF                    /*!< Clears CLUT Transfer Complete Interrupt Flag */
#define DMA2D_IFSR_CCEIF                   DMA2D_IFCR_CCEIF                     /*!< Clears Configuration Error Interrupt Flag    */

/********************  Bit definition for DMA2D_FGMAR register  ***************/

#define DMA2D_FGMAR_MA                     ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_FGOR register  ****************/

#define DMA2D_FGOR_LO                      ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_BGMAR register  ***************/

#define DMA2D_BGMAR_MA                     ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGOR register  ****************/

#define DMA2D_BGOR_LO                      ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_FGPFCCR register  *************/

#define DMA2D_FGPFCCR_CM                   ((uint32_t)0x0000000F)               /*!< Input color mode CM[3:0] */
#define DMA2D_FGPFCCR_CM_0                 ((uint32_t)0x00000001)               /*!< Input color mode CM bit 0 */
#define DMA2D_FGPFCCR_CM_1                 ((uint32_t)0x00000002)               /*!< Input color mode CM bit 1 */
#define DMA2D_FGPFCCR_CM_2                 ((uint32_t)0x00000004)               /*!< Input color mode CM bit 2 */
#define DMA2D_FGPFCCR_CM_3                 ((uint32_t)0x00000008)               /*!< Input color mode CM bit 3 */
#define DMA2D_FGPFCCR_CCM                  ((uint32_t)0x00000010)               /*!< CLUT Color mode */
#define DMA2D_FGPFCCR_START                ((uint32_t)0x00000020)               /*!< Start */
#define DMA2D_FGPFCCR_CS                   ((uint32_t)0x0000FF00)               /*!< CLUT size */
#define DMA2D_FGPFCCR_AM                   ((uint32_t)0x00030000)               /*!< Alpha mode AM[1:0] */
#define DMA2D_FGPFCCR_AM_0                 ((uint32_t)0x00010000)               /*!< Alpha mode AM bit 0 */
#define DMA2D_FGPFCCR_AM_1                 ((uint32_t)0x00020000)               /*!< Alpha mode AM bit 1 */
#define DMA2D_FGPFCCR_ALPHA                ((uint32_t)0xFF000000)               /*!< Alpha value */

/********************  Bit definition for DMA2D_FGCOLR register  **************/

#define DMA2D_FGCOLR_BLUE                  ((uint32_t)0x000000FF)               /*!< Blue Value */
#define DMA2D_FGCOLR_GREEN                 ((uint32_t)0x0000FF00)               /*!< Green Value */
#define DMA2D_FGCOLR_RED                   ((uint32_t)0x00FF0000)               /*!< Red Value */

/********************  Bit definition for DMA2D_BGPFCCR register  *************/

#define DMA2D_BGPFCCR_CM                   ((uint32_t)0x0000000F)               /*!< Input color mode CM[3:0] */
#define DMA2D_BGPFCCR_CM_0                 ((uint32_t)0x00000001)               /*!< Input color mode CM bit 0 */
#define DMA2D_BGPFCCR_CM_1                 ((uint32_t)0x00000002)               /*!< Input color mode CM bit 1 */
#define DMA2D_BGPFCCR_CM_2                 ((uint32_t)0x00000004)               /*!< Input color mode CM bit 2 */
#define DMA2D_FGPFCCR_CM_3                 ((uint32_t)0x00000008)               /*!< Input color mode CM bit 3 */
#define DMA2D_BGPFCCR_CCM                  ((uint32_t)0x00000010)               /*!< CLUT Color mode */
#define DMA2D_BGPFCCR_START                ((uint32_t)0x00000020)               /*!< Start */
#define DMA2D_BGPFCCR_CS                   ((uint32_t)0x0000FF00)               /*!< CLUT size */
#define DMA2D_BGPFCCR_AM                   ((uint32_t)0x00030000)               /*!< Alpha mode AM[1:0] */
#define DMA2D_BGPFCCR_AM_0                 ((uint32_t)0x00010000)               /*!< Alpha mode AM bit 0 */
#define DMA2D_BGPFCCR_AM_1                 ((uint32_t)0x00020000)               /*!< Alpha mode AM bit 1 */
#define DMA2D_BGPFCCR_ALPHA                ((uint32_t)0xFF000000)               /*!< Alpha value */

/********************  Bit definition for DMA2D_BGCOLR register  **************/

#define DMA2D_BGCOLR_BLUE                  ((uint32_t)0x000000FF)               /*!< Blue Value */
#define DMA2D_BGCOLR_GREEN                 ((uint32_t)0x0000FF00)               /*!< Green Value */
#define DMA2D_BGCOLR_RED                   ((uint32_t)0x00FF0000)               /*!< Red Value */

/********************  Bit definition for DMA2D_FGCMAR register  **************/

#define DMA2D_FGCMAR_MA                    ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGCMAR register  **************/

#define DMA2D_BGCMAR_MA                    ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_OPFCCR register  **************/

#define DMA2D_OPFCCR_CM                    ((uint32_t)0x00000007)               /*!< Color mode CM[2:0] */
#define DMA2D_OPFCCR_CM_0                  ((uint32_t)0x00000001)               /*!< Color mode CM bit 0 */
#define DMA2D_OPFCCR_CM_1                  ((uint32_t)0x00000002)               /*!< Color mode CM bit 1 */
#define DMA2D_OPFCCR_CM_2                  ((uint32_t)0x00000004)               /*!< Color mode CM bit 2 */

/********************  Bit definition for DMA2D_OCOLR register  ***************/

/*!<Mode_ARGB8888/RGB888 */

#define DMA2D_OCOLR_BLUE_1                 ((uint32_t)0x000000FF)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_1                ((uint32_t)0x0000FF00)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_1                  ((uint32_t)0x00FF0000)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_1                ((uint32_t)0xFF000000)               /*!< Alpha Channel Value */

/*!<Mode_RGB565 */
#define DMA2D_OCOLR_BLUE_2                 ((uint32_t)0x0000001F)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_2                ((uint32_t)0x000007E0)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_2                  ((uint32_t)0x0000F800)               /*!< Red Value */

/*!<Mode_ARGB1555 */
#define DMA2D_OCOLR_BLUE_3                 ((uint32_t)0x0000001F)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_3                ((uint32_t)0x000003E0)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_3                  ((uint32_t)0x00007C00)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_3                ((uint32_t)0x00008000)               /*!< Alpha Channel Value */

/*!<Mode_ARGB4444 */
#define DMA2D_OCOLR_BLUE_4                 ((uint32_t)0x0000000F)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_4                ((uint32_t)0x000000F0)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_4                  ((uint32_t)0x00000F00)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_4                ((uint32_t)0x0000F000)               /*!< Alpha Channel Value */

/********************  Bit definition for DMA2D_OMAR register  ****************/

#define DMA2D_OMAR_MA                      ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_OOR register  *****************/

#define DMA2D_OOR_LO                       ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_NLR register  *****************/

#define DMA2D_NLR_NL                       ((uint32_t)0x0000FFFF)               /*!< Number of Lines */
#define DMA2D_NLR_PL                       ((uint32_t)0x3FFF0000)               /*!< Pixel per Lines */

/********************  Bit definition for DMA2D_LWR register  *****************/

#define DMA2D_LWR_LW                       ((uint32_t)0x0000FFFF)               /*!< Line Watermark */

/********************  Bit definition for DMA2D_AMTCR register  ***************/

#define DMA2D_AMTCR_EN                     ((uint32_t)0x00000001)               /*!< Enable */
#define DMA2D_AMTCR_DT                     ((uint32_t)0x0000FF00)               /*!< Dead Time */




#define DMA_Channel_0                     ((unsigned int)0x00000000)
#define DMA_Channel_1                     ((unsigned int)0x02000000)
#define DMA_Channel_2                     ((unsigned int)0x04000000)
#define DMA_Channel_3                     ((unsigned int)0x06000000)
#define DMA_Channel_4                     ((unsigned int)0x08000000)
#define DMA_Channel_5                     ((unsigned int)0x0A000000)
#define DMA_Channel_6                     ((unsigned int)0x0C000000)
#define DMA_Channel_7                     ((unsigned int)0x0E000000)

#define DMA_DIR_PeripheralToMemory        ((unsigned int)0x00000000)
#define DMA_DIR_MemoryToPeripheral        ((unsigned int)0x00000040)
#define DMA_DIR_MemoryToMemory            ((unsigned int)0x00000080)

#define DMA_PeripheralInc_Enable          ((unsigned int)0x00000200)
#define DMA_PeripheralInc_Disable         ((unsigned int)0x00000000)

#define DMA_MemoryInc_Enable              ((unsigned int)0x00000400)
#define DMA_MemoryInc_Disable             ((unsigned int)0x00000000)

#define DMA_PeripheralDataSize_Byte       ((unsigned int)0x00000000)
#define DMA_PeripheralDataSize_HalfWord   ((unsigned int)0x00000800)
#define DMA_PeripheralDataSize_Word       ((unsigned int)0x00001000)

#define DMA_MemoryDataSize_Byte           ((unsigned int)0x00000000)
#define DMA_MemoryDataSize_HalfWord       ((unsigned int)0x00002000)
#define DMA_MemoryDataSize_Word           ((unsigned int)0x00004000)

#define DMA_Mode_Normal                   ((unsigned int)0x00000000)
#define DMA_Mode_Circular                 ((unsigned int)0x00000100)

#define DMA_Priority_Low                  ((unsigned int)0x00000000)
#define DMA_Priority_Medium               ((unsigned int)0x00010000)
#define DMA_Priority_High                 ((unsigned int)0x00020000)
#define DMA_Priority_VeryHigh             ((unsigned int)0x00030000)

#define DMA_FIFOMode_Disable              ((unsigned int)0x00000000)
#define DMA_FIFOMode_Enable               ((unsigned int)0x00000004)

#define DMA_FIFOThreshold_1QuarterFull    ((unsigned int)0x00000000)
#define DMA_FIFOThreshold_HalfFull        ((unsigned int)0x00000001)
#define DMA_FIFOThreshold_3QuartersFull   ((unsigned int)0x00000002)
#define DMA_FIFOThreshold_Full            ((unsigned int)0x00000003)

#define DMA_MemoryBurst_Single            ((unsigned int)0x00000000)
#define DMA_MemoryBurst_INC4              ((unsigned int)0x00800000)
#define DMA_MemoryBurst_INC8              ((unsigned int)0x01000000)
#define DMA_MemoryBurst_INC16             ((unsigned int)0x01800000)

#define DMA_PeripheralBurst_Single        ((unsigned int)0x00000000)
#define DMA_PeripheralBurst_INC4          ((unsigned int)0x00200000)
#define DMA_PeripheralBurst_INC8          ((unsigned int)0x00400000)
#define DMA_PeripheralBurst_INC16         ((unsigned int)0x00600000)

#define DMA_FIFOStatus_Less1QuarterFull   ((unsigned int)0x00000000 << 3)
#define DMA_FIFOStatus_1QuarterFull       ((unsigned int)0x00000001 << 3)
#define DMA_FIFOStatus_HalfFull           ((unsigned int)0x00000002 << 3)
#define DMA_FIFOStatus_3QuartersFull      ((unsigned int)0x00000003 << 3)
#define DMA_FIFOStatus_Empty              ((unsigned int)0x00000004 << 3)
#define DMA_FIFOStatus_Full               ((unsigned int)0x00000005 << 3)

#define TRANSFER_IT_ENABLE_MASK (unsigned int)(DMA_SxCR_TCIE | DMA_SxCR_HTIE | \
DMA_SxCR_TEIE | DMA_SxCR_DMEIE)

#define DMA_Stream0_IT_MASK     (unsigned int)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
    DMA_LISR_TCIF0)

#define DMA_Stream1_IT_MASK     (unsigned int)(DMA_Stream0_IT_MASK << 6)
#define DMA_Stream2_IT_MASK     (unsigned int)(DMA_Stream0_IT_MASK << 16)
#define DMA_Stream3_IT_MASK     (unsigned int)(DMA_Stream0_IT_MASK << 22)
#define DMA_Stream4_IT_MASK     (unsigned int)(DMA_Stream0_IT_MASK | (unsigned int)0x20000000)
#define DMA_Stream5_IT_MASK     (unsigned int)(DMA_Stream1_IT_MASK | (unsigned int)0x20000000)
#define DMA_Stream6_IT_MASK     (unsigned int)(DMA_Stream2_IT_MASK | (unsigned int)0x20000000)
#define DMA_Stream7_IT_MASK     (unsigned int)(DMA_Stream3_IT_MASK | (unsigned int)0x20000000)
#define TRANSFER_IT_MASK        (unsigned int)0x0F3C0F3C
#define HIGH_ISR_MASK           (unsigned int)0x20000000
#define RESERVED_MASK           (unsigned int)0x0F7D0F7D


/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    unsigned int DMA_Channel;
    unsigned int DMA_PeripheralBaseAddr;
    unsigned int DMA_Memory0BaseAddr;
    unsigned int DMA_DIR;
    unsigned int DMA_BufferSize;
    unsigned int DMA_PeripheralInc;
    unsigned int DMA_MemoryInc;
    unsigned int DMA_PeripheralDataSize;
    unsigned int DMA_MemoryDataSize;
    unsigned int DMA_Mode;
    unsigned int DMA_Priority;
    unsigned int DMA_FIFOMode;
    unsigned int DMA_FIFOThreshold;
    unsigned int DMA_MemoryBurst;
    unsigned int DMA_PeripheralBurst;
} DMA_InitTypeDef;

struct stIRQHandler {
    void (*Handler) (unsigned int dwIn);
    unsigned int dwIn;
};

/*******************************************************************************
内部全局变量定义
*******************************************************************************/
struct stIRQHandler DMA_IrqHandler[DMA_DEV_NUM][DMA_STREAM_NUM] = {0};

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
void DMA_DeInit(DMA_Stream_TypeDef* DMAy_Streamx)
{
    /* Check the parameters */
    // assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));

    /* Disable the selected DMAy Streamx */
    DMAy_Streamx->CR &= ~((unsigned int)DMA_SxCR_EN);

    /* Reset DMAy Streamx control register */
    DMAy_Streamx->CR  = 0;

    /* Reset DMAy Streamx Number of Data to Transfer register */
    DMAy_Streamx->NDTR = 0;

    /* Reset DMAy Streamx peripheral address register */
    DMAy_Streamx->PAR  = 0;

    /* Reset DMAy Streamx memory 0 address register */
    DMAy_Streamx->M0AR = 0;

    /* Reset DMAy Streamx memory 1 address register */
    DMAy_Streamx->M1AR = 0;

    /* Reset DMAy Streamx FIFO control register */
    DMAy_Streamx->FCR = (unsigned int)0x00000021;

    /* Reset interrupt pending bits for the selected stream */
    if (DMAy_Streamx == DMA1_Stream0) {
        /* Reset interrupt pending bits for DMA1 Stream0 */
        DMA1->LIFCR = DMA_Stream0_IT_MASK;
    }
    else if (DMAy_Streamx == DMA1_Stream1) {
        /* Reset interrupt pending bits for DMA1 Stream1 */
        DMA1->LIFCR = DMA_Stream1_IT_MASK;
    }
    else if (DMAy_Streamx == DMA1_Stream2) {
        /* Reset interrupt pending bits for DMA1 Stream2 */
        DMA1->LIFCR = DMA_Stream2_IT_MASK;
    }
    else if (DMAy_Streamx == DMA1_Stream3) {
        /* Reset interrupt pending bits for DMA1 Stream3 */
        DMA1->LIFCR = DMA_Stream3_IT_MASK;
    }
    else if (DMAy_Streamx == DMA1_Stream4) {
        /* Reset interrupt pending bits for DMA1 Stream4 */
        DMA1->HIFCR = DMA_Stream4_IT_MASK;
    }
    else if (DMAy_Streamx == DMA1_Stream5) {
        /* Reset interrupt pending bits for DMA1 Stream5 */
        DMA1->HIFCR = DMA_Stream5_IT_MASK;
    }
    else if (DMAy_Streamx == DMA1_Stream6) {
        /* Reset interrupt pending bits for DMA1 Stream6 */
        DMA1->HIFCR = (unsigned int)DMA_Stream6_IT_MASK;
    }
    else if (DMAy_Streamx == DMA1_Stream7) {
        /* Reset interrupt pending bits for DMA1 Stream7 */
        DMA1->HIFCR = DMA_Stream7_IT_MASK;
    }
    else if (DMAy_Streamx == DMA2_Stream0) {
        /* Reset interrupt pending bits for DMA2 Stream0 */
        DMA2->LIFCR = DMA_Stream0_IT_MASK;
    }
    else if (DMAy_Streamx == DMA2_Stream1) {
        /* Reset interrupt pending bits for DMA2 Stream1 */
        DMA2->LIFCR = DMA_Stream1_IT_MASK;
    }
    else if (DMAy_Streamx == DMA2_Stream2) {
        /* Reset interrupt pending bits for DMA2 Stream2 */
        DMA2->LIFCR = DMA_Stream2_IT_MASK;
    }
    else if (DMAy_Streamx == DMA2_Stream3) {
        /* Reset interrupt pending bits for DMA2 Stream3 */
        DMA2->LIFCR = DMA_Stream3_IT_MASK;
    }
    else if (DMAy_Streamx == DMA2_Stream4) {
        /* Reset interrupt pending bits for DMA2 Stream4 */
        DMA2->HIFCR = DMA_Stream4_IT_MASK;
    }
    else if (DMAy_Streamx == DMA2_Stream5) {
        /* Reset interrupt pending bits for DMA2 Stream5 */
        DMA2->HIFCR = DMA_Stream5_IT_MASK;
    }
    else if (DMAy_Streamx == DMA2_Stream6) {
        /* Reset interrupt pending bits for DMA2 Stream6 */
        DMA2->HIFCR = DMA_Stream6_IT_MASK;
    }
    else {
        if (DMAy_Streamx == DMA2_Stream7) {
            /* Reset interrupt pending bits for DMA2 Stream7 */
            DMA2->HIFCR = DMA_Stream7_IT_MASK;
        }
    }
}

void DMA_Init(DMA_Stream_TypeDef* DMAy_Streamx, DMA_InitTypeDef* DMA_InitStruct)
{
    unsigned int tmpreg = 0;

    /*------------------------- DMAy Streamx CR Configuration ------------------*/
    /* Get the DMAy_Streamx CR value */
    tmpreg = DMAy_Streamx->CR;

    /* Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
    tmpreg &= ((unsigned int)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST | \
        DMA_SxCR_PL | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE | \
            DMA_SxCR_MINC | DMA_SxCR_PINC | DMA_SxCR_CIRC | \
                DMA_SxCR_DIR));

    /* Configure DMAy Streamx: */
    /* Set CHSEL bits according to DMA_CHSEL value */
    /* Set DIR bits according to DMA_DIR value */
    /* Set PINC bit according to DMA_PeripheralInc value */
    /* Set MINC bit according to DMA_MemoryInc value */
    /* Set PSIZE bits according to DMA_PeripheralDataSize value */
    /* Set MSIZE bits according to DMA_MemoryDataSize value */
    /* Set CIRC bit according to DMA_Mode value */
    /* Set PL bits according to DMA_Priority value */
    /* Set MBURST bits according to DMA_MemoryBurst value */
    /* Set PBURST bits according to DMA_PeripheralBurst value */
    tmpreg |= DMA_InitStruct->DMA_DIR |
        DMA_InitStruct->DMA_PeripheralInc | DMA_InitStruct->DMA_MemoryInc |
            DMA_InitStruct->DMA_PeripheralDataSize | DMA_InitStruct->DMA_MemoryDataSize |
                DMA_InitStruct->DMA_Mode | DMA_InitStruct->DMA_Priority |
                    DMA_InitStruct->DMA_MemoryBurst | DMA_InitStruct->DMA_PeripheralBurst;

    /* Write to DMAy Streamx CR register */
    DMAy_Streamx->CR = tmpreg;
    DMAy_Streamx->BitCR.CHSEL = DMA_InitStruct->DMA_Channel;

    /*------------------------- DMAy Streamx FCR Configuration -----------------*/
    /* Get the DMAy_Streamx FCR value */
    tmpreg = DMAy_Streamx->FCR;

    /* Clear DMDIS and FTH bits */
    tmpreg &= (unsigned int)~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);

    /* Configure DMAy Streamx FIFO:
    Set DMDIS bits according to DMA_FIFOMode value
    Set FTH bits according to DMA_FIFOThreshold value */
    tmpreg |= DMA_InitStruct->DMA_FIFOMode | DMA_InitStruct->DMA_FIFOThreshold;

    /* Write to DMAy Streamx CR */
    DMAy_Streamx->FCR = tmpreg;

    /*------------------------- DMAy Streamx NDTR Configuration ----------------*/
    /* Write to DMAy Streamx NDTR register */
    DMAy_Streamx->NDTR = DMA_InitStruct->DMA_BufferSize;

    /*------------------------- DMAy Streamx PAR Configuration -----------------*/
    /* Write to DMAy Streamx PAR */
    DMAy_Streamx->PAR = DMA_InitStruct->DMA_PeripheralBaseAddr;

    /*------------------------- DMAy Streamx M0AR Configuration ----------------*/
    /* Write to DMAy Streamx M0AR */
    DMAy_Streamx->M0AR = DMA_InitStruct->DMA_Memory0BaseAddr;
}

/*******************************************************************************
函数功能：
*   @ 初始化中断部分
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static void NvicInit(unsigned int dwDev, unsigned int dwStream)
{
    DMA_TypeDef *pstDma;
    unsigned int dwTrq;

    if (dwDev == DMA_DEV_1) {
        pstDma = DMA1;
        dwTrq  = (dwStream < 7)?(DMA1_Stream0_IRQn + dwStream) : 47;
    }
    else {
        pstDma = DMA2;
        dwTrq  = DMA2_Stream0_IRQn + dwStream + ((dwStream > 4) ? 7 : 0);
    }

    DevIo->NvicUp(dwTrq, 2, 0);
    pstDma->Stream[dwStream].BitCR.TCIE = 0;
}

/*******************************************************************************
*函数功能：
*   @ 清楚dma通道状态
*参数描述：
*   @ unsigned int dwDev ：需要调用的设备
*   @ unsigned int dwChn ：x-通道x,从1到7
*返回值描述：
*   无
*说明：
*   无。
*******************************************************************************/
static int ClrStream(unsigned int dwDev, unsigned int dwStream)
{
    unsigned int wDat;
    unsigned int dwSdreamCp = dwStream % 4;

    DMA_TypeDef *pstDma;
    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    wDat = 0x003d;
    wDat <<= (dwSdreamCp * 6);
    if(dwSdreamCp > 1) wDat <<= 4;

    if(dwStream < 4) pstDma->LIFCR = (unsigned int)wDat;
    else pstDma->HIFCR = (unsigned int)wDat;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 主要是配置DMA内存到外设
参数描述：
*   @ unsigned int dwDev ：0-DMA1；1-DMA2
*   @ unsigned int dwChn ：x-通道x,从1到7
*   @ unsigned int dwMode: 工作模式
*   @ unsigned int dwMDatSize ：0-8位，1-16位，2-32位
*   @ unsigned int dwPDatSize ：0-8位，1-16位，2-32位
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int ConfigM2P( unsigned int dwDev, unsigned int dwStream, unsigned int dwChn, \
    unsigned int dwMode, unsigned int dwMDatSize, unsigned int dwPDatSize)
{
    DMA_TypeDef *pstDma;
    DMA_InitTypeDef  stInit;

    if(dwDev == DMA_DEV_1) pstDma = DMA1;
    else if(dwDev == DMA_DEV_2) pstDma = DMA2;
    else return -1;

    DMA_DeInit(&pstDma->Stream[dwStream]);
    memset(&stInit, 0, sizeof(DMA_InitTypeDef));

    stInit.DMA_PeripheralBaseAddr = 0;
    stInit.DMA_Memory0BaseAddr = 0;
    stInit.DMA_Channel = dwChn;

    stInit.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    stInit.DMA_BufferSize = 0;
    stInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    stInit.DMA_MemoryInc = DMA_MemoryInc_Enable;

    if(dwPDatSize == DMA_PSIZE_BYTE) stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    else if(dwPDatSize == DMA_PSIZE_HALFWORD) stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    else stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;

    if(dwMDatSize == DMA_MSIZE_BYTE) stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    else if(dwMDatSize == DMA_MSIZE_HALFWORD) stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    else stInit.DMA_PeripheralDataSize = DMA_MemoryDataSize_Word;

    stInit.DMA_Mode = (dwMode == DMA_Circular_Mode) ? DMA_Mode_Circular : DMA_Mode_Normal;
    stInit.DMA_Priority = DMA_Priority_Low;
    DMA_Init(&pstDma->Stream[dwStream], &stInit);

    // 初始化中断
    NvicInit(dwDev, dwStream);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 主要是配置DMA外设到内存
参数描述：
*   @ unsigned int dwDev ：0-DMA1；1-DMA2
*   @ unsigned int dwChn ：x-通道x,从1到7
*   @ unsigned int dwMode: 工作模式 DMA_Normal_Mode DMA_Circular_Mode
*   @ unsigned int dwMDatSize ：0-8位，1-16位，2-32位
*   @ unsigned int dwPDatSize ：0-8位，1-16位，2-32位
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int ConfigP2M( unsigned int dwDev, unsigned int dwStream, unsigned int dwChn, \
    unsigned int dwMode, unsigned int dwMDatSize, unsigned int dwPDatSize)
{
    DMA_TypeDef *pstDma;
    DMA_InitTypeDef  stInit;

    if(dwDev == DMA_DEV_1) pstDma = DMA1;
    else if(dwDev == DMA_DEV_2) pstDma = DMA2;
    else return -1;

    // if(!dwChn) return -1;
    DMA_DeInit(&pstDma->Stream[dwStream]);
    memset(&stInit, 0, sizeof(DMA_InitTypeDef));

    stInit.DMA_PeripheralBaseAddr = 0;
    stInit.DMA_Memory0BaseAddr = 0;
    stInit.DMA_Channel = dwChn;

    stInit.DMA_DIR = DMA_DIR_PeripheralToMemory;                // 外设作为数据来源
    stInit.DMA_BufferSize = 0;
    stInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       // 外设数据源地址不增加
    stInit.DMA_MemoryInc = DMA_MemoryInc_Enable;

    if(dwPDatSize == DMA_PSIZE_BYTE) stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    else if(dwPDatSize == DMA_PSIZE_HALFWORD) stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    else stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;

    if(dwMDatSize == DMA_MSIZE_BYTE) stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    else if(dwMDatSize == DMA_MSIZE_HALFWORD) stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    else stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;

    stInit.DMA_Mode = (dwMode == DMA_Normal_Mode) ? DMA_Mode_Normal : DMA_Mode_Circular;
    stInit.DMA_Priority = DMA_Priority_Low;                     // DMA_Priority_High;
    DMA_Init(&pstDma->Stream[dwStream], &stInit);

    // 初始化中断
    NvicInit(dwDev, dwStream);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 中断统一处理接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void DmaIRQHandler(unsigned int dwDev, unsigned int dwStream)
{
    DMA_TypeDef *pstDma;
    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    if (pstDma->Stream[dwStream].BitCR.CIRC == 0) {                             // 非循环模式，关闭DMA通道
        pstDma->Stream[dwStream].BitCR.EN = 0;
        pstDma->Stream[dwStream].BitCR.TCIE = 0;
    }

    if(DMA_IrqHandler[dwDev][dwStream].Handler) {
        DMA_IrqHandler[dwDev][dwStream].Handler( DMA_IrqHandler[dwDev][dwStream].dwIn);
        if (pstDma->Stream[dwStream].BitCR.CIRC == 0) {
            DMA_IrqHandler[dwDev][dwStream].Handler = NULL;
        }
    }
}

/*******************************************************************************
函数功能：
*   @ 通过DMA发送数据
参数描述：
*   @ unsigned int dwDev ：需要调用的设备
*   @ unsigned int dwStream ：x-通道x,从1到7
*   @ unsigned int dwPadd ： 外设的地址
*   @ unsigned int dwMadd ： 内存的地址
*   @ unsigned int dwLen ：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无。
*******************************************************************************/
static int Trs(unsigned int dwDev, unsigned int dwStream, unsigned int dwPadd, unsigned int dwMadd, unsigned int dwLen)
{
    DMA_TypeDef *pstDma;
    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    pstDma->Stream[dwStream].BitCR.EN = 0;
    // 清除中断
    ClrStream(dwDev, dwStream);

    pstDma->Stream[dwStream].PAR  = (unsigned int)dwPadd;
    pstDma->Stream[dwStream].NDTR = (unsigned int)dwLen;
    pstDma->Stream[dwStream].M0AR = (unsigned int)dwMadd;
    pstDma->Stream[dwStream].M1AR = (unsigned int)dwMadd;
    pstDma->Stream[dwStream].BitCR.EN = 1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置传输完成中断事件
参数描述：
*   @ unsigned int dwDev ：需要调用的设备
*   @ unsigned int dwChn ：x-通道x,从1到7
*   @ void (*Handler)(void) 传输完成后调用的接口
返回值描述：
*   @ 无
说明：
*   @ 此接口只能执行一次，每次传输的时候都需要重新设置
*******************************************************************************/
static void SetTrsHandler(unsigned int dwDev, unsigned int dwStream, void (*Handler)(unsigned int dwIn), unsigned dwIn)
{
    DMA_TypeDef *pstDma;

    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    ClrStream(dwDev, dwStream);
    pstDma->Stream[dwStream].BitCR.TCIE = 1;
    ClrStream(dwDev, dwStream);

    DMA_IrqHandler[dwDev][dwStream].Handler = Handler;
    DMA_IrqHandler[dwDev][dwStream].dwIn = dwIn;
}

/*******************************************************************************
函数功能：
*   @ 查询DMA是否在传输数据
参数描述：
*   @ unsigned int dwDev ：需要调用的设备
*   @ unsigned int dwChn ：x-通道x,从1到7
返回值描述：
*   @ true ：dma忙
*   @ false ；dma空闲
说明：
*   @ 通过查询dma的计数器判断是否存在数据传输。
*******************************************************************************/
static bool IsBusy(unsigned int dwDev, unsigned int dwStream)
{
    DMA_TypeDef *pstDma;

    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    if(pstDma->Stream[dwStream].NDTR) return true;
    return false;
}

/*******************************************************************************
函数功能：
*   @ 中断传输接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void DMA1_Stream0_IRQHandler(void)
{
    if (DMA1->BitLISR.TCIF0) {
        DMA1->BitLIFCR.CTCIF0 = 1;
        DmaIRQHandler( 0, 0);
    }
}

void DMA1_Stream1_IRQHandler(void)
{
    if (DMA1->BitLISR.TCIF1) {
        DMA1->BitLIFCR.CTCIF1 = 1;
        DmaIRQHandler( 0, 1);
    }
}

void DMA1_Stream2_IRQHandler(void)
{
    if (DMA1->BitLISR.TCIF2) {
        DMA1->BitLIFCR.CTCIF2 = 1;
        DmaIRQHandler( 0, 2);
    }
}

void DMA1_Stream3_IRQHandler(void)
{
    if (DMA1->BitLISR.TCIF3) {
        DMA1->BitLIFCR.CTCIF3 = 1;
        DmaIRQHandler( 0, 3);
    }
}

void DMA1_Stream4_IRQHandler(void)
{
    if (DMA1->BitHISR.TCIF4) {
        DMA1->BitHIFCR.CTCIF4 = 1;
        DmaIRQHandler( 0, 4);
    }
}

void DMA1_Stream5_IRQHandler(void)
{
    if (DMA1->BitHISR.TCIF5) {
        DMA1->BitHIFCR.CTCIF5 = 1;
        DmaIRQHandler( 0, 5);
    }
}

void DMA1_Stream6_IRQHandler(void)
{
    if (DMA1->BitHISR.TCIF6) {
        DMA1->BitHIFCR.CTCIF6 = 1;
        DmaIRQHandler( 0, 6);
    }
}

void DMA1_Stream7_IRQHandler(void)
{
    if (DMA1->BitHISR.TCIF7) {
        DMA1->BitHIFCR.CTCIF7 = 1;
        DmaIRQHandler( 0, 7);
    }
}

void DMA2_Stream0_IRQHandler(void)
{
    if (DMA2->BitLISR.TCIF0) {
        DMA2->BitLIFCR.CTCIF0 = 1;
        DmaIRQHandler( 1, 0);
    }
}

void DMA2_Stream1_IRQHandler(void)
{
    if (DMA2->BitLISR.TCIF1) {
        DMA2->BitLIFCR.CTCIF1 = 1;
        DmaIRQHandler( 1, 1);
    }
}

void DMA2_Stream2_IRQHandler(void)
{
    if (DMA2->BitLISR.TCIF2) {
        DMA2->BitLIFCR.CTCIF2 = 1;
        DmaIRQHandler( 1, 2);
    }
}

void DMA2_Stream3_IRQHandler(void)
{
    if (DMA2->BitLISR.TCIF3) {
        DMA2->BitLIFCR.CTCIF3 = 1;
        DmaIRQHandler( 1, 3);
    }
}

void DMA2_Stream4_IRQHandler(void)
{
    if (DMA2->BitLISR.TCIF0) {
        DMA2->BitLIFCR.CTCIF0 = 1;
        DmaIRQHandler( 1, 4);
    }
}

void DMA2_Stream5_IRQHandler(void)
{
    if (DMA2->BitHISR.TCIF5) {
        DMA2->BitHIFCR.CTCIF5 = 1;
        DmaIRQHandler( 1, 5);
    }
}

void DMA2_Stream6_IRQHandler(void)
{
    if (DMA2->BitHISR.TCIF6) {
        DMA2->BitHIFCR.CTCIF6 = 1;
        DmaIRQHandler( 1, 6);
    }
}

void DMA2_Stream7_IRQHandler(void)
{
    if (DMA2->BitHISR.TCIF7) {
        DMA2->BitHIFCR.CTCIF7 = 1;
        DmaIRQHandler( 1, 7);
    }
}

static const DevDmaTdf s_Dev = {
    ConfigM2P,
    ConfigP2M,
    Trs,
    IsBusy,
    SetTrsHandler
};

/*******************************************************************************
函数功能：
*   @ 初始化串口设备
参数描述：
*   @ void (*pvGpioInit)(unsigned char)：gpio初始化
*   @ unsigned char byDat：gpio初始化时需要带入的参数
返回值描述：
*   @ 串口设备提供的操作接口
说明：
*   @ 无
*******************************************************************************/
const DevDmaTdf * DmaInit ( void )
{
#if(CONFIG_DMA1)
    RCC->BitAHB1ENR.DMA1EN  = 1;
#endif
#if(CONFIG_DMA2)
    RCC->BitAHB1ENR.DMA2EN  = 1;
#endif
    return &s_Dev;
}

#endif
#endif