/*******************************************************************************
* Copyright URIT Co.,Ltd. All Right Reserved
*
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*******************************************************************************/

/*******************************************************************************
* File Name     : ver_def.h
* File Path     : ./
* Project       : urit department-1 platcode
* Author        : xuqiang
* Date          : 2023.03.02
* Description   : for fw or sw version control.
*
* modify logs:  Copyright urit Co.,Ltd.All Right Reserved
* 2023.03.02    xuqiang   create
*******************************************************************************/
#ifndef _VER_DEF_H
#define _VER_DEF_H

/*******************************************************************************
Include file
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition
*******************************************************************************/
#define VERSION_COMP_LOCALTOIN              0x08008130
#define VERSION_COMP_LOCALTOIN_1            VERSION_COMP_LOCALTOIN
#define VERSION_COMP_LOCALTOIN_2            VERSION_COMP_LOCALTOIN + 4
#define VERSION_COMP_LOCALTOIN_3            VERSION_COMP_LOCALTOIN + 8

#ifdef A5_VERSION
    #define ARCH_NO                         0x03300101;
    #define VERSION_FLAG                    0x01010106;
    #define VERSION_DATE                    0x01170302;
#endif

/*******************************************************************************
 object Variables Definition
*******************************************************************************/
typedef enum __emProductLineTdf {
    ProductLine_NO              = 0x00,
    ProductLine_UC              = 0x01,
    ProductLine_UD              = 0x02,
    ProductLine_SL              = 0x03
} emProductLineTdf;

typedef struct __stVerSectionTdf {
    union {
        unsigned int dwArch;
        struct {
            unsigned char product_line;
            unsigned char product_module;
            unsigned short int product;
        } stArch;
    } d0;

    union {
        unsigned int dwVer;
        struct {
            unsigned char match;
            unsigned char function;
            unsigned char compatible;
            unsigned char version;
        } stVer;
    } d1;

    union {
        unsigned int dwDate;
        struct {
            unsigned char modify_times;
            unsigned char year;
            unsigned char month;
            unsigned char day;
        } stDate;
    } d2;

} stVerSectionTdf;

/*******************************************************************************
Global Variables Definition
*******************************************************************************/
#pragma location = VERSION_COMP_LOCALTOIN_1
const unsigned int c_dwArch = ARCH_NO;

#pragma location = VERSION_COMP_LOCALTOIN_2
const unsigned int c_dwVer = VERSION_FLAG;

#pragma location = VERSION_COMP_LOCALTOIN_3
const unsigned int c_dwVerDate = VERSION_DATE;

/*
// now using, bin and packing
                  V X       .YY      ***         .NNMMDD      .CodeVer    _BX
// bin
UC32VET_M_CHN_    V 9       .02      .1          .20230301    .1620       _B191.bin
CL500_M_ENG_RU_ES V16.      .00      .4          .20230227    .3499       _B256.bin

// pack
U500B_M_CHN_ENG_  V 1       .14                  .20230227    .3499       _B256.zip
UC32-FW_ENG_      V 3       .06      ***         .20230301    .1620       _B191.zip

// modify
// bin
                Vcustomer .mainver .compatible  .version    .commitsn     _Bcodever-buildsn-builddate
U500B_M_CHN_      V 1       .01      .01          .05         .1          _B2356-17-20230301.bin
U500B_M_VET_      V 3       .01      .01          .02         .2          _B2358-20-20230301.bin
U500B_M_ENG_      V 7       .01      .01          .02         .2          _B2358-20-20230301.bin

// packing
                  V X       .YY                   .NNMMDD     .CodeVer    _BX
U500B_M_CHN_ENG_  V 1       .14                   .20230227   .3499       _B256.zip
UD2000_APP_CHN_   V 1       .02                   .20230227   .4577       _B174.zip
*/
#endif