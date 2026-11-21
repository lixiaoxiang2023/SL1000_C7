/*******************************************************************************
* Copyright URIT Co., Ltd.All Right Reserved
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
* Author        : xuqiang auto tool V2.0
* Date          : 2026.11.21
* Description   : for fw or sw version control. tools V2
*
* modify logs:  Copyright urit Co.,Ltd.All Right Reserved
* 2026.11.21    xuqiang   create
*******************************************************************************/
#ifndef _VER_DEF_H
#define _VER_DEF_H

/*******************************************************************************
Include file
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition
*******************************************************************************/
#ifdef __UC_AUTOBUILD_DEFAULT
    #define C7_VERSION                  1
#endif

#ifdef __UC_AUTOBUILD_2DEFAULT
    #define MACRO_2                     1
#endif

#ifdef __UC_AUTOBUILD_3DEFAULT
    #define MACRO_3                     1
#endif

#ifdef C7_VERSION
    #define VERSION_COMP_LOCALTOIN          0x08008130
    #define ARCH_NO                         0x10000703
    #define VERSION_FLAG                    0x17010101
    #define VERSION_DATE                    0x21112603
    #define VERSION_HW                      0x05010000
    #define VERSION_HW_DATE                 0x03032200
#endif

#define VERSION_COMP_LOCALTOIN_1            VERSION_COMP_LOCALTOIN
#define VERSION_COMP_LOCALTOIN_2            VERSION_COMP_LOCALTOIN + 4
#define VERSION_COMP_LOCALTOIN_3            VERSION_COMP_LOCALTOIN + 8
#define VERSION_COMP_LOCALTOIN_4            VERSION_COMP_LOCALTOIN + 12
#define VERSION_COMP_LOCALTOIN_5            VERSION_COMP_LOCALTOIN + 16

// ARCH_NO msb = product(2byte a) + productmodule(1byte b) + productline(1byte c) eg: ox aaaa bb cc 

/*******************************************************************************
object Variables Definition
*******************************************************************************/
typedef enum __emProductLineTdf
{
    ProductLine_NO = 0x00,
    ProductLine_UC = 0x01,
    ProductLine_UD = 0x02,
    ProductLine_SL = 0x03
} emProductLineTdf;

typedef struct __stVerSectionTdf
{
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
            unsigned char customer;
            unsigned char mainversion;
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

    union {
        unsigned int dwHwVer;
        struct {
            unsigned char res0;
            unsigned char res1;
            unsigned char major;
            unsigned char alter;
        } stHwVer;
    } d3;

    union {
        unsigned int dwHwDate;
        struct {
                unsigned char res;
                unsigned char year;
                unsigned char month;
                unsigned char day;
            } stHwDate;
    } d4;

} stVerSectionTdf;

/*******************************************************************************
Global Variables Definition
*******************************************************************************/
extern const unsigned int c_dwArch;
extern const unsigned int c_dwVer;
extern const unsigned int c_dwVerDate;
extern const unsigned int c_dwVerHw;
extern const unsigned int c_dwVerHwDate;
#endif