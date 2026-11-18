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
* File Name     : ver_def.c
* File Path     : ./
* Project       : urit department-1 platcode
* Author        : xuqiang auto tool V2.0
* Date          : 2026.09.23
* Description   : for fw or sw version control. tools V2
*
* modify logs:  Copyright urit Co.,Ltd.All Right Reserved
* 2026.09.23    xuqiang   create
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "ver_def.h"

/*******************************************************************************
object Variables Definition
*******************************************************************************/

/*******************************************************************************
Global Variables Definition
*******************************************************************************/
#pragma location = VERSION_COMP_LOCALTOIN_1
const unsigned int c_dwArch = ARCH_NO;

#pragma location = VERSION_COMP_LOCALTOIN_2
const unsigned int c_dwVer = VERSION_FLAG;

#pragma location = VERSION_COMP_LOCALTOIN_3
const unsigned int c_dwVerDate = VERSION_DATE;

#pragma location = VERSION_COMP_LOCALTOIN_4
const unsigned int c_dwVerHw = VERSION_HW;

#pragma location = VERSION_COMP_LOCALTOIN_5
const unsigned int c_dwVerHwDate = VERSION_HW_DATE;