/*******************************************************************************
* Copyright URIT Co.,Ltd. All Right Reserved
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*******************************************************************************/

/*******************************************************************************
* File Name     : clients_comm.h
* File Path     : platcode/clients/
* Project       : urit1 plat
* Author        : lixiaoxiang
* Date          : 2023.02.10
* Description   : mcu io base
*******************************************************************************/

/*******************************************************************************
* modify logs:
* 2023.02.10 lixiaoxiang create
*******************************************************************************/
#ifndef __CLIENT_COMM_H_
#define __CLIENT_COMM_H_

/*******************************************************************************
Include files
*******************************************************************************/

/*******************************************************************************
 Constant and Macro Definition
*******************************************************************************/
#define LOAD_UNIT_LEN                   512

/*******************************************************************************
 data type define
*******************************************************************************/
#pragma pack(1)
typedef struct _stCmdTdf
{
    unsigned char byCmd;                        // cmd
    unsigned char byCmdCnt;                     // cmd counter
    unsigned short int wParm[5];                // paremeter
} stCmdTdf;

typedef struct _stCmdStaTdf
{
    unsigned char  byCmd;                       // last cmd
    unsigned char  byCmdCnt;                    // last cmd counter
    unsigned char  byErr;                       // error code
    unsigned char  byErrCnt;                    // error code counter
} stCmdStaTdf;

typedef struct _stVersionTdf
{
    unsigned int dwArch;
    unsigned int dwVer;
    unsigned int dwVerDate;
    unsigned int dwHwVer;
    unsigned int dwHwVerDate;
} stVersionTdf;

// firmware upgrade command
typedef struct _stUpgradeCmdTdf {
    unsigned int uiFrameID;                     // frame SN
    unsigned int uiFilesize;                    // file size
    unsigned char byCanRx[LOAD_UNIT_LEN];
    unsigned short usCrcChk;                    // crc
} stUpgradeCmdTdf;

typedef struct _stUpgradeFlagTdf {
    unsigned char ucHandFlg;                    // receive sucess
    unsigned char usUpdataFinish;               // upgrade end flag
    unsigned char usUpdataReady;
} stUpgradeFlagTdf;

#pragma pack()

/*******************************************************************************
 export parm
*******************************************************************************/

/*******************************************************************************
 export functions
*******************************************************************************/

#endif
