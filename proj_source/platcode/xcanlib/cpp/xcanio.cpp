/*******************************************************************************
文件名称：   xcansdk.h
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2019.5.30
程序用途：   xcan协议,C# 对IO资源的管理

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2019.5.30
修改人：胥强
修改内容：初始编写

*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "xcanio.h"
#include "xcanmgr.h"

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
class XcanIOD
{
public:
    // IO资源管理数据
    typedef struct __stDevIoListTdf {
        bool blTrueIO;

        unsigned int  dwClassUUID;
        unsigned int  dwUUID;
        short unsigned int wMyDevID;
        short unsigned int wRemoteDevID;
        unsigned int  dwComChnID;
        unsigned short int wIoUUID;

        unsigned char *pbyIoSrc;
        unsigned int  dwIoLen;

        XcanIO *pThis;              // 类据柄
    } stDevIoListTdf;

    typedef struct {
        XcanIO *pThis;
        bool blIsAdd;

        short unsigned int mMyDevID;
        short unsigned int mRemoteDevID;
        unsigned int  mCommChnID;
    } stBridgeTdf;

    bool mIimVritualIO;
    bool mMaterIO;
    bool mGenInterIo;

    // 数据更新事件
    void(*mpNotice) (unsigned int dwIoUUID);

    // 重载计数器
    static unsigned int mClassBuildCnt;
    static unsigned int mClassUuidCnt;
    unsigned int mClassUUID;

    // 资源列表
    static std::list<stDevIoListTdf> *mListDevIo;

    // 私有方法
    unsigned int  mUUID;
    short unsigned int mMyDevID;
    short unsigned int mRemoteDevID;
    unsigned int  mComChnID;
    unsigned short int mIoUUID;

    unsigned char *mIoSrc;
    unsigned char *mIoBuff;
    unsigned char *mShadow;
    unsigned char *mShadowBuff;
    unsigned char *mShadowIoCache;

    unsigned int mIoLen;
    unsigned int mIoBuffLen;
    unsigned int mIoShadowLen;
    unsigned int mIoShadowBuffLen;
    unsigned int mIoShadowIoCacheLen;

    unsigned short int mIOInStart;
    unsigned short int mIOOutStart;
    unsigned short int mIOSyncEnd;

    unsigned short int mIODSyncEnd;
    unsigned short int mIODEnd;

    // 数据命令执行方法
    unsigned int mICmdPos;
    unsigned int mICmdCntPos;
    unsigned int mOCmdCntPos;
    unsigned int mICmdDataPos;

    // 桥接资源
    struct __stBridge {
        unsigned int  mUUID;
        short unsigned int mMyDevID;
        short unsigned int mRemoteDevID;
        unsigned int  mCommChnID;
        unsigned short int mIoUUID;

        unsigned char *mIoSrc;
        unsigned char *mIoBuff;
        unsigned char *mShadow;
        unsigned char *mShadowBuff;
        unsigned char *mShadowIoCache;
    } mBridge;

    static std::list<XcanIO::stBridgeTdf> *mListBridge;

    /*
     * for data read and write oparetions
    */
    unsigned char *mRdWrBuff = nullptr;
    bool mReadWriteEnd = true;
    short unsigned int mReadWriteSAddr;
    unsigned int mReadWriteLen;

    // 资源列表
    unsigned int mDwLenCnt;
    list<XcanIO::XcanIoDesc *> mIoDescList;

    /*******************************************************************************
    函数功能：
    *   @ 构造函数
    参数描述：
    *   @ 无
    返回值描述：
    *   @ 无
    说明：
    *   @ 无
    *******************************************************************************/
    XcanIOD() {
        mIimVritualIO = true;
        mMaterIO = true;
        mGenInterIo = false;

        // 管理类
        XcanNet::GetMutex();

        XcanIOD::mClassBuildCnt++;
        XcanIOD::mClassUuidCnt++;
        mClassUUID = XcanIOD::mClassUuidCnt;

        // 桥接表
        if (XcanIOD::mListBridge == nullptr) {
            XcanIOD::mListBridge = new std::list<XcanIO::stBridgeTdf>();
        }

        // 初始化IO列表
        if (XcanIOD::mListDevIo == nullptr) XcanIOD::mListDevIo = new std::list<XcanIOD::stDevIoListTdf>();
        XcanNet::PutMutex();

        mpNotice = nullptr;

        // 初始化桥接
        memset(&mBridge, 0, sizeof(mBridge));
        mBridge.mShadow = mBridge.mShadowBuff = mBridge.mShadowIoCache = mBridge.mIoSrc = mBridge.mIoBuff = nullptr;

        // 私有方法
        mUUID = 0;
        mRemoteDevID = 0;
        mComChnID = 0;
        mIoUUID = 0;

        mIoSrc = nullptr;
        mIoBuff = nullptr;
        mShadow = nullptr;
        mShadowBuff = nullptr;

        mIoLen = 0;
        mIoBuffLen = 0;
        mIoShadowLen = 0;
        mIoShadowBuffLen = 0;

        mIOInStart = 0;
        mIOOutStart = 0;
        mIOSyncEnd = 0;

        mIODSyncEnd = 0;
        mIODEnd = 0;

        // 数据命令执行方法
        mICmdPos = 0;
        mICmdCntPos = 0;
        mOCmdCntPos = 0;

        mDwLenCnt = 0;

        mIoDescList.clear();
    }

    /*******************************************************************************
    函数功能：
    *   @ 析构函数
    参数描述：
    *   @ 无
    返回值描述：
    *   @ 无
    说明：
    *   @ 无
    *******************************************************************************/
    ~XcanIOD() {
        // 回收不用的资源, 回收IO列表中的数据
        std::list<XcanIOD::stDevIoListTdf>::iterator itr = XcanIOD::mListDevIo->begin();
        for (; itr != XcanIOD::mListDevIo->end(); itr ++) {
            if (itr->dwClassUUID == mClassUUID) {
                XcanIOD::mListDevIo->erase(itr ++);
                break;
            }
        }

        if (mBridge.mUUID != 0) XcanIO::mXcan->RemovePort(mBridge.mUUID); // 回收C库资源
        if (mBridge.mIoBuff != nullptr) delete mBridge.mIoBuff;
        if (mBridge.mShadow != nullptr) delete mBridge.mShadow;
        if (mBridge.mShadowBuff != nullptr) delete mBridge.mShadowBuff;

        if (mIimVritualIO == false)
        {
            if (mIoSrc != nullptr && mGenInterIo == true) delete mIoSrc;
            if (mIoBuff != nullptr) delete mIoBuff;
            if (mShadow != nullptr) delete mShadow;
            if (mShadowBuff != nullptr) delete mShadowBuff;
            if (mRdWrBuff != nullptr) delete mRdWrBuff;

            mIoSrc = nullptr;
            mIoBuff = nullptr;
            mShadow = nullptr;
            mShadowBuff = nullptr;
            mRdWrBuff = nullptr;

            XcanIO::mXcan->RemovePort(mUUID); // 回收C库资源

            if (mIoDescList.size() == 0) return;
            std::list<XcanIO::XcanIoDesc*>::iterator itr = XcanIOD::mIoDescList.begin();
            for (; itr != XcanIOD::mIoDescList.end(); itr ++) {
                delete *itr;
            }

            mIoDescList.clear();
        }
    }
};

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/
std::list<XcanIOD::stDevIoListTdf> *XcanIOD::mListDevIo = nullptr;
unsigned int XcanIOD::mClassBuildCnt = 0;
unsigned int XcanIOD::mClassUuidCnt = 0;
std::list<XcanIO::stBridgeTdf> *XcanIOD::mListBridge = nullptr;

/*******************************************************************************
函数功能：
*   @ 构造函数
参数描述：
*   @ XcanIO *pIoThis 数据基类
*   @ string name 数据名称
*   @ Iotype dwType 数据类型
*   @ unsigned short int wArrayLen 数据长度
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
XcanIO::XcanIoDesc::XcanIoDesc(XcanIO *pIoThis, const char *name, Iotype dwType, unsigned short int wArrayLen)
{
    mIoThis = pIoThis;
    mName = name;
    mPosition = pIoThis->GetIoDescPos();
    mLen = (unsigned short int)(wArrayLen * (unsigned short int)dwType);
    mType = dwType;
    mArrayLen = wArrayLen;

    pIoThis->SetIoDescPos(mPosition + mLen);
    pIoThis->InsertIoDesc(this);

    if (mIoD != nullptr) delete mIoD;
    mIoD = new unsigned int[wArrayLen];

    mCallBackNotice = nullptr;
    mMyDat = nullptr;
}

/*******************************************************************************
函数功能：
*   @ 析构函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
XcanIO::XcanIoDesc::~XcanIoDesc()
{
    if (mIoD != nullptr) delete mIoD;
}

/*******************************************************************************
函数功能：
*   @ 取值
参数描述：
*   @ 无
返回值描述：
*   @ 对象的值
说明：
*   @  无
*******************************************************************************/
unsigned int *XcanIO::XcanIoDesc::Value(void)
{
    if (mType == Iotype::IO_U8) {
        unsigned char *get = new unsigned char[mArrayLen];
        mIoThis->GetIoArrayBytes(mPosition, mArrayLen, get);
        for (unsigned int i = 0; i < mArrayLen; i++) mIoD[i] = (unsigned int)get[i];
        delete[] get;
    }
    else if (mType == Iotype::IO_U16) {
        unsigned short int *get = new unsigned short int[mArrayLen];
        mIoThis->GetIoArrayU16(mPosition, mArrayLen, get);
        for (unsigned int i = 0; i < mArrayLen; i++) mIoD[i] = (unsigned int)get[i];
        delete[] get;
    }
    else {
        unsigned int *get = new unsigned int[mArrayLen];
        mIoThis->GetIoArrayU32(mPosition, mArrayLen, get);
        for (unsigned int i = 0; i < mArrayLen; i++) mIoD[i] = get[i];
        delete[] get;
    }

    return mIoD;
}

/*******************************************************************************
函数功能：
*   @ 设置参数32
参数描述：
*   @ u32 In 数组32
*   @ unsigned int dwLen 长度
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::XcanIoDesc::Value(unsigned int dwIn)
{
    mIoThis->SetIoU32(mPosition, dwIn);
}

/*******************************************************************************
函数功能：
*   @ 设置参数16
参数描述：
*   @ u8 In 数组8
*   @ unsigned int dwLen 长度
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::XcanIoDesc::Value(unsigned short int wIn)
{
    mIoThis->SetIoU16(mPosition, wIn);
}

/*******************************************************************************
函数功能：
*   @ 设置参数8
参数描述：
*   @ u8 In 数组8
*   @ unsigned int dwLen 长度
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::XcanIoDesc::Value(unsigned char byIn)
{
    mIoThis->SetIoU8(mPosition, byIn);
}

/*******************************************************************************
函数功能：
*   @ 设置参数32 数组
参数描述：
*   @ u32 *pIn 数组32
*   @ unsigned int dwLen 长度
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::XcanIoDesc::Value(unsigned int *pIn, unsigned int dwLen)
{
    if (dwLen > mArrayLen) return;
    mIoThis->SetIoArrayU32(mPosition, pIn, dwLen);
}

/*******************************************************************************
函数功能：
*   @ 设置参数16 数组
参数描述：
*   @ u16 *pIn 数组16
*   @ unsigned int dwLen 长度
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::XcanIoDesc::Value(unsigned short int *pIn, unsigned int dwLen)
{
    if (dwLen > mArrayLen) return;
    mIoThis->SetIoArrayU16(mPosition, pIn, dwLen);
}

/*******************************************************************************
函数功能：
*   @ 设置参数8 数组
参数描述：
*   @ byte *pIn 数组8
*   @ unsigned int dwLen 长度
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::XcanIoDesc::Value(unsigned char *pIn, unsigned int dwLen)
{
    if (dwLen > mArrayLen) return;
    mIoThis->SetIoArrayBytes(mPosition, pIn, dwLen);
}

/*******************************************************************************
函数功能：
*   @ 设置通知接口
参数描述：
*   @ void (*pHandler)(void) 通知接口
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::XcanIoDesc::SetRefreshNotice(void (*pHandler)(void))
{
    mMyDat = new unsigned int[mArrayLen];
    mCallBackNotice = pHandler;

    unsigned int* objVal = Value();
    for (unsigned int i = 0; i < mArrayLen; i++) mMyDat[i] = objVal[i];
}

/*******************************************************************************
函数功能：
*   @ 检查数据是否变化，并通知更新
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::XcanIoDesc::CheakAndNotice(void)
{
    if (mCallBackNotice == nullptr || mMyDat == nullptr) return;

    unsigned int* objVal = Value();
    for (unsigned int i = 0; i < mArrayLen; i++) {
        if (objVal[i] != mMyDat[i]) {
            if (mCallBackNotice != nullptr) mCallBackNotice();
            for (unsigned int k = 0; k < mArrayLen; k ++) mMyDat[k] = objVal[k];
            break;
        }
    }
}

/******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 构造函数，定义一个数据结构
参数描述：
*   @ DevPortTdf *pstPort
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
XcanIO::XcanIO()
{
    d = new XcanIOD();
}

/*******************************************************************************
函数功能：
*   @ 析构函数,回收不用的数据
参数描述：
*   @
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
XcanIO::~XcanIO()
{
    delete d;
    d = nullptr;

    // 防止数据被错误回收
    XcanIOD::mClassBuildCnt --;
    if(XcanIOD::mClassBuildCnt != 0) return;

    if(XcanIOD::mListBridge != nullptr) XcanIOD::mListBridge->clear();
    delete XcanIOD::mListBridge;

    // 回收IO资源
    if (XcanIOD::mListDevIo != nullptr) XcanIOD::mListDevIo->clear();
    delete XcanIOD::mListDevIo;
}

/*******************************************************************************
函数功能：
*   @ 记录当前资源的序号位置
参数描述：
*   @ unsigned int dwPos 位置
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::SetIoDescPos(unsigned int dwPos)
{
    d->mDwLenCnt = dwPos;
}

/*******************************************************************************
函数功能：
*   @ 获取当前的序号位置
参数描述：
*   @ 无
返回值描述：
*   @ 当前的序号位置
说明：
*   @ 无
*******************************************************************************/
unsigned int XcanIO::GetIoDescPos(void)
{
    return d->mDwLenCnt;
}

/*******************************************************************************
函数功能：
*   @ 向资源列表中插入资源描述
参数描述：
*   @ XcanIoDesc *pClass 资源描述
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::InsertIoDesc(XcanIoDesc *pClass)
{
    d->mIoDescList.push_back(pClass);
}

/*******************************************************************************
函数功能：
*   @ 初始化io资源计数器
参数描述：
*   @ XcanIoDesc *pClass 资源描述
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoInsertStart(void)
{
    d->mDwLenCnt = 0;
}

/*******************************************************************************
函数功能：
*   @ 自动设置io同步资源的起始
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoSynInStart(void)
{
    d->mIOInStart = (unsigned short int)d->mDwLenCnt;
}

/*******************************************************************************
函数功能：
*   @ 自动同步设置io资源的输出起始
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoSynOutStart(void)
{
    d->mIOOutStart = (unsigned short int)d->mDwLenCnt;
}

/*******************************************************************************
函数功能：
*   @ 自动同步设置IO资源的结束
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoSynEnd(void)
{
    d->mIOSyncEnd = (unsigned short int)d->mDwLenCnt;
}

/*******************************************************************************
函数功能：
*   @ 自动设置io资源的起始
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoDSyncEnd(void)
{
    d->mIODSyncEnd = (unsigned short int)d->mDwLenCnt;
}

/*******************************************************************************
函数功能：
*   @ 设置结束长度和资源长度
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoDEnd(void)
{
    d->mIoLen = (unsigned short int)d->mDwLenCnt;
    d->mIODEnd = (unsigned short int)d->mDwLenCnt;

    for(std::list<XcanIoDesc*>::iterator itr = d->mIoDescList.begin(); itr != d->mIoDescList.end(); itr ++) {
        std::string strName = std::string((*itr)->mName);
        if (strName == "mInCmd") d->mICmdPos = (*itr)->mPosition;
        else if (strName == "mInCmdCnt") d->mICmdCntPos = (*itr)->mPosition;
        else if (strName == "mOutCmdCnt") d->mOCmdCntPos = (*itr)->mPosition;
        else if (strName == "mInSwap") d->mICmdDataPos = (*itr)->mPosition;
       // else if (strName == "mOutSwap") d->mOCmdDataPos = (*itr)->mPosition;
    }
}

/*******************************************************************************
函数功能：
*   @ 启动IO资源的同步功能
参数描述：
*   @ short unsigned int wStart 同步数据开始地址
*	@ short unsigned int wEnd 同步数据结束地址
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
int XcanIO::SynSet(unsigned short int wInStart, short unsigned int wOutStart, short unsigned int wSyncEnd,
    short unsigned int wIoDSyncEnd, short unsigned int wIoDEnd)
{
    d->mIOOutStart = wOutStart;
    d->mIOInStart = wInStart;
    d->mIOSyncEnd = wSyncEnd;

    d->mIODSyncEnd = wIoDSyncEnd;
    d->mIODEnd = wIoDEnd;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 启动IO资源的同步功能
参数描述：
*   @ short unsigned int wStart 同步数据开始地址
*	@ short unsigned int wEnd 同步数据结束地址
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
int XcanIO::SynSet(unsigned short int wInStart, short unsigned int wOutStart, short unsigned int wSyncEnd)
{
    d->mIOOutStart = wOutStart;
    d->mIOInStart = wInStart;
    d->mIOSyncEnd = wSyncEnd;

    d->mIODSyncEnd = wSyncEnd;
    d->mIODEnd = wSyncEnd;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 增加一个具体的IO资源数据，此数据将和远程设备进行通信绑定。
参数描述：
*   @ unsigned int dwRemoteDevID 远程设备的ID
*	@ unsigned int dwComChnID 通信端口ID
*	@ short unsigned int wIoUUID IO资源的ID
*	@ bool blIsRx 是否为接收资源
返回值描述：
*   @ -1 失败
*   @  0 成功
说明：
*   @  无
*******************************************************************************/
int XcanIO::LinkToDevice(short unsigned int wRemoteDevID, unsigned int dwComChnID,
    short unsigned int wIoUUID, bool blIsRx)
{
    return LinkToDevice(wRemoteDevID, dwComChnID, wIoUUID, nullptr, blIsRx);
}

/*******************************************************************************
函数功能：
*   @ 增加一个具体的IO资源数据，此数据将和远程设备进行通信绑定。
参数描述：
*   @ unsigned int dwRemoteDevID 远程设备的ID
*	@ unsigned int dwComChnID 通信端口ID
*	@ short unsigned int wIoUUID IO资源的ID
*	@ unsigned char *pbyIoSrc IO资源的地址，外部提供
*	@ bool blIsRx 是否为接收资源
返回值描述：
*   @ -1 失败
*   @  0 成功
说明：
*   @  无
*******************************************************************************/
int XcanIO::LinkToDevice(short unsigned int wRemoteDevID, unsigned int dwComChnID,
    short unsigned int wIoUUID, unsigned char *pbyIoSrc, bool blIsRx)
{
    stCommChnTdf *pstCommChn = nullptr;
    stXcanNetTdf stNet;
    std::list<stXcanNetTdf>::iterator itdev = mListCommNet->begin();
    unsigned int dwIoLen = d->mIODEnd;

    if (dwIoLen == 0) return -1;
    if (dwComChnID == 0 || wRemoteDevID == 0 || wIoUUID == 0) return -1;

    // 设备检查
    if (mListCommNet->empty() == false) {
        for (; itdev != mListCommNet->end(); itdev ++) {
            stNet = *itdev;
            if (stNet.dwCommChnID == dwComChnID) break;
        }
    }

    if (stNet.dwCommChnID == dwComChnID) {
        pstCommChn = &stNet.stComm;
    }

    // 检查资源
    XcanIOD::stDevIoListTdf stIO;
    std::list<XcanIOD::stDevIoListTdf>::iterator it = XcanIOD::mListDevIo->begin();
    if (XcanIOD::mListDevIo->empty() != true) {
        for ( ; it != XcanIOD::mListDevIo->end(); ++ it) {
            stIO = *it;
            if(stIO.wIoUUID == wIoUUID && stIO.wRemoteDevID == wRemoteDevID &&
                stIO.dwComChnID == dwComChnID && stIO.blTrueIO == true) break;
        }
    }

    // 已经有实体，不在进一步初始化
    if(it != XcanIOD::mListDevIo->end()) {
        stIO.blTrueIO = false;

        d->mIoLen = stIO.dwIoLen;
        d->mIoSrc = stIO.pbyIoSrc;

        d->mMyDevID = stIO.wMyDevID;
        d->mRemoteDevID = stIO.wRemoteDevID;
        d->mComChnID = stIO.dwComChnID;
        d->mIoUUID = stIO.wIoUUID;
        d->mUUID = stIO.dwUUID;
        d->mRdWrBuff = stIO.pThis->d->mRdWrBuff;

        stIO.pThis = this;
        stIO.dwClassUUID = d->mClassUUID;

        XcanIOD::mListDevIo->push_back(stIO);
        return 0;
    }

    // 申请缓存
    d->mGenInterIo = false;
    if (pbyIoSrc == nullptr) {
        d->mGenInterIo = true;
        pbyIoSrc = new unsigned char[dwIoLen];
    }

    // 插入到资源列表,在列表中没有对应的数据时，将数据加入列表，并注册C 库
    d->mIimVritualIO = false;
    d->mMaterIO = (blIsRx == true) ? true : false;

    stIO.blTrueIO = true;
    stIO.dwClassUUID = d->mClassUUID;
    stIO.wIoUUID = wIoUUID;
    stIO.wMyDevID = XcanNet::GetMyNetDevID();
    stIO.wRemoteDevID = wRemoteDevID;
    stIO.dwComChnID = dwComChnID;
    stIO.pbyIoSrc = pbyIoSrc;
    stIO.dwIoLen = dwIoLen;

    // 创建缓冲长度
    d->mIoLen = dwIoLen;
    d->mIoBuffLen = dwIoLen + 8;
    d->mIoShadowLen = d->mIoBuffLen;
    d->mIoShadowBuffLen = d->mIoLen + (d->mIoLen / 8);
    d->mIoShadowIoCacheLen = dwIoLen;

    // 创建IO资源所需要的数据缓存
    d->mIoSrc = pbyIoSrc;
    d->mIoBuff = new unsigned char[d->mIoBuffLen];
    d->mShadow = new unsigned char[d->mIoShadowLen];
    d->mShadowBuff = new unsigned char[d->mIoShadowBuffLen];
    d->mShadowIoCache = new unsigned char[d->mIoShadowIoCacheLen];

    // 读写的数据长度
    d->mRdWrBuff = new unsigned char[d->mIoBuffLen];

    memset(d->mRdWrBuff, 0, d->mIoBuffLen);
    memset(d->mIoSrc, 0, d->mIoLen);
    memset(d->mIoBuff, 0, d->mIoBuffLen);
    memset(d->mShadow, 0, d->mIoShadowLen);
    memset(d->mShadowBuff, 0, d->mIoShadowBuffLen);
    memset(d->mShadowIoCache, 0, d->mIoShadowIoCacheLen);

    d->mMyDevID = XcanNet::GetMyNetDevID();
    d->mRemoteDevID = wRemoteDevID;
    d->mComChnID = dwComChnID;
    d->mIoUUID = wIoUUID;
    d->mIoLen = dwIoLen;

    // 在通信设备未准备好的情况下，先不向内核注册数据。
    if (blIsRx == true) {
        if (pstCommChn != nullptr) d->mUUID = mXcan->AddRxPort(&stNet.stComm, d->mRemoteDevID, d->mIoUUID,
            d->mIoSrc, d->mIoBuff, d->mIoLen);
        else d->mUUID = mXcan->AddIo(d->mRemoteDevID, dwComChnID, d->mIoUUID, d->mIoSrc, d->mIoBuff, d->mIoLen, true);

        mXcan->ServicePortInit(d->mUUID, d->mShadow, d->mShadowBuff, d->mShadowIoCache);
        mXcan->SynSet(d->mUUID, true, d->mIOOutStart, d->mIOSyncEnd, d->mIODSyncEnd, d->mIODEnd);
    }
    else {
        if (pstCommChn != nullptr) d->mUUID = mXcan->AddTxPort(&stNet.stComm, d->mRemoteDevID,
            d->mIoUUID, d->mIoSrc, d->mIoBuff, d->mIoLen);
        else d->mUUID = mXcan->AddIo(d->mRemoteDevID, dwComChnID, d->mIoUUID, d->mIoSrc, d->mIoBuff, d->mIoLen, false);

        mXcan->ServicePortInit(d->mUUID, d->mShadow, d->mShadowBuff, d->mShadowIoCache);
        mXcan->SynSet(d->mUUID, true, d->mIOInStart, d->mIOOutStart, d->mIODSyncEnd, d->mIODEnd);
    }

    // 注册回调函数
    mXcan->RegReceiveNotice(d->mUUID, ReceiveNoticeHandler);
    mXcan->RegReadWriteNotice(d->mUUID, RdWrNoticeHandler);

    stIO.dwUUID = d->mUUID;
    stIO.pThis = this;
    XcanIOD::mListDevIo->push_back(stIO);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 检查传输状态
参数描述：
*   @ 无
返回值描述：
*   @ 传输状态数据
说明：
*   @ 无
*******************************************************************************/
XcanIO::stTrsCommStaTdf XcanIO::GetTrsCommSta(void)
{
    XcanIO::stTrsCommStaTdf stSta;

    mXcan->GetCommSta(d->mUUID, &stSta.wTxPkgSN, &stSta.wTxPkgID, &stSta.wRxPkgSN, &stSta.wRxPkgID, &stSta.dwTxPkgLost);
    return stSta;
}

/*******************************************************************************
函数功能：
*   @ 资源桥接到远程的设备
参数描述：
*   @ XcanIO::stBridgeTdf *pBridge 桥接数据
返回值描述：
*   @ -1 失败
*   @  0 成功
说明：
*   @ 无
*******************************************************************************/
int XcanIO::BridgeInsert(XcanIO::stBridgeTdf *pBridge)
{
    unsigned int dwComChnID =  pBridge->mCommChnID;
    short unsigned int wRemoteDevID = pBridge->mRemoteDevID;
    short unsigned int wMyDevID = pBridge->mMyDevID;
    XcanIO *pThis = pBridge->pThis;

    stXcanNetTdf stNet;
    std::list<stXcanNetTdf>::iterator pstDev = XcanNet::mListCommNet->begin();

    // 设备检查
    if(XcanNet::mListCommNet->empty() == true) return -1;
    for ( ; pstDev != XcanNet::mListCommNet->end(); ++ pstDev) {
        if(pstDev->dwCommChnID == dwComChnID) break;
    }

    stNet = *pstDev;
    if(stNet.dwCommChnID != dwComChnID) return -1;

    // 插入到资源列表,在列表中没有对应的数据时，将数据加入列表，并注册C 库
    pThis->d->mBridge.mCommChnID = dwComChnID;
    pThis->d->mBridge.mRemoteDevID = wRemoteDevID;
    pThis->d->mBridge.mMyDevID = wMyDevID;

    if(pThis->d->mBridge.mIoBuff) delete d->mBridge.mIoBuff;
    if(pThis->d->mBridge.mShadow) delete d->mBridge.mShadow;
    if(pThis->d->mBridge.mShadowBuff) delete d->mBridge.mShadowBuff;

    // 创建IO资源所需要的数据缓存
    pThis->d->mBridge.mIoUUID = pThis->d->mIoUUID;
    pThis->d->mBridge.mIoSrc = pThis->d->mIoSrc;
    pThis->d->mBridge.mIoBuff = new unsigned char[pThis->d->mIoBuffLen];
    pThis->d->mBridge.mShadow = new unsigned char[pThis->d->mIoShadowLen];
    pThis->d->mBridge.mShadowBuff = new unsigned char[pThis->d->mIoShadowBuffLen];
    pThis->d->mBridge.mShadowIoCache = new unsigned char[pThis->d->mIoShadowIoCacheLen];

    memset(pThis->d->mBridge.mIoBuff, 0, pThis->d->mIoBuffLen);
    memset(pThis->d->mBridge.mShadow, 0, pThis->d->mIoShadowLen);
    memset(pThis->d->mBridge.mShadowBuff, 0, pThis->d->mIoShadowBuffLen);
    memset(pThis->d->mBridge.mShadowIoCache, 0, pThis->d->mIoShadowIoCacheLen);

    pThis->d->mBridge.mUUID = mXcan->AddTxPort(&stNet.stComm, pThis->d->mBridge.mRemoteDevID, pThis->d->mBridge.mIoUUID,
                                     pThis->d->mBridge.mIoSrc, pThis->d->mBridge.mIoBuff, pThis->d->mIoLen);
    mXcan->ServicePortInit(pThis->d->mBridge.mUUID, pThis->d->mBridge.mShadow, pThis->d->mBridge.mShadowBuff,
        pThis->d->mBridge.mShadowIoCache);
    mXcan->SynSet(pThis->d->mBridge.mUUID, true, pThis->d->mIOInStart, pThis->d->mIOOutStart, pThis->d->mIODSyncEnd, pThis->d->mIODEnd);

    // 建立透传组
    mXcan->AddGroup(d->mUUID, pThis->d->mBridge.mUUID);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 删除资源桥接到远程的设备
参数描述：
*   @ XcanIO::stBridgeTdf *pBridge 桥接数据
返回值描述：
*   @ -1 失败
*   @  0 成功
说明：
*   @ 无
*******************************************************************************/
int XcanIO::BridgeDelete(XcanIO::stBridgeTdf *pBridge)
{
    XcanIO *pThis = pBridge->pThis;

    if (pThis->d->mBridge.mUUID == 0) return -1;
    mXcan->RemovePort(pThis->d->mBridge.mUUID);
    if(pThis->d->mBridge.mIoBuff) delete d->mBridge.mIoBuff;
    if(pThis->d->mBridge.mShadow) delete d->mBridge.mShadow;
    if(pThis->d->mBridge.mShadowBuff) delete d->mBridge.mShadowBuff;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 将资源桥接到远程的设备
参数描述：
*   @ unsigned int dwComChnID 需要绑定的通信接口ID
*   @ short unsigned int dwRemoteDevID 远程设备的ID
返回值描述：
*   @ -1 失败
*   @  0 成功
说明：
*   @  无
*******************************************************************************/
int XcanIO::BridgeLink(short unsigned int wRemoteDevID, unsigned int dwCommChnID)
{
    XcanIO::stBridgeTdf stBridge;
    stBridge.blIsAdd = true;
    stBridge.pThis = this;
    stBridge.mCommChnID = dwCommChnID;
    stBridge.mRemoteDevID = wRemoteDevID;

    GetMutex();
    XcanIOD::mListBridge->push_back(stBridge);
    PutMutex();
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 断开将资源桥接到远程的设备
参数描述：
*   @ 无
返回值描述：
*   @ -1 失败
*   @  0 成功
说明：
*   @  无
*******************************************************************************/
int XcanIO::BridgeRemove(void)
{
    XcanIO::stBridgeTdf stBridge;
    stBridge.blIsAdd = false;

    stBridge.pThis = this;
    stBridge.mCommChnID = d->mComChnID;
    stBridge.mRemoteDevID = d->mRemoteDevID;
    stBridge.mMyDevID = d->mMyDevID;

    GetMutex();
    XcanIOD::mListBridge->push_back(stBridge);
    PutMutex();
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 用于安全删除桥接数据
参数描述：
*   @ XcanIO::stBridgeTdf *pBridge 桥接数据
返回值描述：
*   @ -1 失败
*   @  0 成功
说明：
*   @ 无
*******************************************************************************/
void XcanIO::BridgeService(void)
{
    int nRet = 0;
    XcanIO::stBridgeTdf stBridge;

    if (XcanIOD::mListBridge->empty()) return;

    GetMutex();
    for ( ; ; ) {
        if (XcanIOD::mListBridge->empty() == true) break;
        stBridge = XcanIOD::mListBridge->front();

        if (stBridge.blIsAdd == true) nRet = stBridge.pThis->BridgeInsert(&stBridge);
        else nRet = stBridge.pThis->BridgeDelete(&stBridge);

        if (nRet) break;
        XcanIOD::mListBridge->pop_front();
    }

    PutMutex();
}

/*******************************************************************************
函数功能：
*   @ 设定数据接收的回调窗口，static
参数描述：
*   @ unsigned int dwIoUUID 资源的UUID
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::ReceiveNoticeHandler(unsigned int dwUUID)
{
    XcanIOD::stDevIoListTdf cIO;

    for (std::list<XcanIOD::stDevIoListTdf>::iterator it = XcanIOD::mListDevIo->begin(); it != XcanIOD::mListDevIo->end(); it ++) {
        cIO = *it;

        if (cIO.pThis->d == nullptr) continue;
        if (dwUUID != cIO.pThis->d->mUUID) continue;
        if (cIO.pThis->d->mpNotice) cIO.pThis->d->mpNotice(cIO.pThis->d->mIoUUID);

        if (cIO.pThis->d->mIoDescList.size() == 0) continue;
        // start
        list<XcanIO::XcanIoDesc *>::iterator itr = cIO.pThis->d->mIoDescList.begin();
        for(; itr != cIO.pThis->d->mIoDescList.end(); itr ++) {
            (*itr)->CheakAndNotice();
        }
    }
}

/*******************************************************************************
函数功能：
*   @ 数据读写完成后的回调通知，static
参数描述：
*   @ unsigned int dwIoUUID 资源的UUID
*   @ unsigned int dwOp 0: write, 1: read
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanIO::RdWrNoticeHandler(unsigned int dwIoUUID, unsigned int dwOp)
{
    XcanIOD::stDevIoListTdf cIO;
    XcanIO *pXcanIO = nullptr;

    for (std::list<XcanIOD::stDevIoListTdf>::iterator it = XcanIOD::mListDevIo->begin(); it != XcanIOD::mListDevIo->end(); it ++) {
        cIO = *it;
        if (dwIoUUID != cIO.pThis->d->mIoUUID) continue;
        pXcanIO = cIO.pThis;
        if (dwOp == 0) {
            // 数据读取完成
            if(pXcanIO->d->mRdWrBuff != nullptr) memcpy(pXcanIO->d->mRdWrBuff, pXcanIO->d->mIoSrc + pXcanIO->d->mReadWriteSAddr,
                pXcanIO->d->mReadWriteLen);
            pXcanIO->d->mReadWriteEnd = true;
        }
        else {
            // 数据写入完成
            pXcanIO->d->mReadWriteEnd = true;
        }
    }
}

/*******************************************************************************
函数功能：
*   @ 设置协议层的更新通知接口
参数描述：
*   @ void (*pNotice) (unsigned int dwIoUUID) 通信接口
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetReceiveNoticeHandler(void (*pNotice) (unsigned int dwIoUUID))
{
    d->mpNotice = pNotice;
}

/*******************************************************************************
函数功能：
*   @ 检查是否是真是的设备
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanIO::IsRealDev(void)
{
    return (d->mIimVritualIO == true)? false : true;
}

/*******************************************************************************
函数功能：
*   @ 查询远端设备是否连接到本地
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanIO::RemoteDeviceExist()
{
    return mXcan->SynInLink(d->mUUID);
}

/*******************************************************************************
函数功能：
*   @ 查询本地是否连接到远端设备
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanIO::LinkToRemoteDevice()
{
    return mXcan->SynOutLink(d->mUUID);
}

/*******************************************************************************
函数功能：
*   @ 计算出IO的地址
参数描述：
*   @ unsigned char *pbyInPos 输入的io数据地址
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
unsigned int XcanIO::GetIoPosition(unsigned char *pbyInPos)
{
    return (unsigned int)(pbyInPos - d->mIoSrc);
}

/*******************************************************************************
函数功能：
*   @ 初始
参数描述：
*   @ unsigned int dwICmdPos 输入命令位置
*   @ unsigned int dwICmdCntPos 输入命令计数位置
*   @ unsigned int dwOCmdCntPos 输出命令计数位置
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetCmd(unsigned int dwICmdPos, unsigned int dwICmdCntPos, unsigned int dwOCmdCntPos)
{
    d->mICmdPos = dwICmdPos;
    d->mICmdCntPos = dwICmdCntPos;
    d->mOCmdCntPos = dwOCmdCntPos;
}

/*******************************************************************************
函数功能：
*   @ 初始
参数描述：
*   @ unsigned int dwICmdPos 输入命令位置
*   @ unsigned int dwICmdCntPos 输入命令计数位置
*   @ unsigned int dwICmdDataPos 发送数据起始位置
*   @ unsigned int dwOCmdCntPos 输出命令计数位置

返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetCmd(unsigned int dwICmdPos, unsigned int dwICmdCntPos, unsigned int  dwICmdDataPos, unsigned int dwOCmdCntPos)
{
    d->mICmdPos = dwICmdPos;
    d->mICmdCntPos = dwICmdCntPos;
    d->mICmdDataPos = dwICmdDataPos;
    d->mOCmdCntPos = dwOCmdCntPos;
}

/*******************************************************************************
函数功能：
*   @ 向子设备写入数据，带命令，并监控返回
参数描述：
*   @ unsigned char byCmd 命令
*   @ unsigned int dwInPos 需要下发的数据位置
*   @ unsigned int dwInLen 需要下发的数据长度
*   @ unsigned int dwOutPos 需要监控的接收数据位置
*   @ unsigned int dwOutLen 需要监控的接收数据长度
*   @ unsigned int dwDat  需要下发的数据
*   @ unsigned int dwDatMask 需要和输入对比的数据mask
返回值描述：
*   @ 无
说明：
*   @ 数据长度只支持124字节长度
*******************************************************************************/
int XcanIO::SetCmdWithCheak(unsigned char byCmd, unsigned int dwInPos, unsigned int dwInLen,
    unsigned int dwOutPos, unsigned int dwOutLen,
    unsigned int dwDat, unsigned int dwDatMask)
{
    unsigned int dwShit = 0;
    unsigned int dwOutDat = 0;
    unsigned int dwInDat = 0;

    while ((dwDatMask & 0x01) == 0) {
        dwDatMask >>= 1;
        dwShit++;
        if (dwShit > 30) return -1;
    }

    dwInDat = ((dwDat >> dwShit) & dwDatMask);

    if (dwOutLen == 1) dwOutDat = (unsigned int)GetIoU8(dwOutPos);
    else if (dwOutLen == 2) dwOutDat = (unsigned int)GetIoU16(dwOutPos);
    else if (dwOutLen == 4) dwOutDat = (unsigned int)GetIoU32(dwOutPos);

    if (dwOutDat == dwInDat) return 0;

    if (dwInLen == 1) SetIoU8(dwInPos, (unsigned char)dwDat);
    else if (dwInLen == 2) SetIoU16(dwInPos, (short unsigned int)dwDat);
    else if (dwInLen == 4) SetIoU32(dwInPos, (unsigned int)dwDat);
    else return -1;

    if (SetCommand(byCmd)) return -1;

    /*for (int i = 0; i < 100; i++) {
        if(dwOutLen == 1) dwOutDat = (unsigned int)GetIoU8(dwOutPos);
        else if(dwOutLen == 2) dwOutDat = (unsigned int)GetIoU16(dwOutPos);
        else if(dwOutLen == 4) dwOutDat = (unsigned int)GetIoU32(dwOutPos);
        else return -1;

        if(dwOutDat == dwInDat) break;
        usleep(50000); // 50ms
    }*/ /* qt 暂时不能实现，此部分 阻塞执行 */

    // if(dwOutDat != dwInDat) return -1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置命令
参数描述：
*   @ unsigned char byCmd 输入命令
返回值描述：
*   @ -2 参数未定义
*   @ -1 设备断开
*   @  0 执行成功
*   @  1 设备忙
说明：
*   @  无
*******************************************************************************/
int XcanIO::SetCommand(unsigned char byCmd)
{
    unsigned char byLastCnt = 0;

    // 没有注册此数据
    if (d->mICmdCntPos == d->mOCmdCntPos) return -2;

    // 检查设备连接
    if (LinkToRemoteDevice() == false) return -3;
    if (RemoteDeviceExist() == false) return -1;

    // 设备忙
    if (GetIoU8(d->mICmdCntPos) != GetIoU8(d->mOCmdCntPos)) return 1;

    SetIoU8(d->mICmdPos, byCmd);
    byLastCnt = GetIoU8(d->mOCmdCntPos);
    SetIoU8(d->mICmdCntPos, byLastCnt + 1);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置命令
参数描述：
*   @ unsigned short byCmd 输入命令
返回值描述：
*   @ -2 参数未定义
*   @ -1 设备断开
*   @  0 执行成功
*   @  1 设备忙
说明：
*   @  无
*******************************************************************************/
int XcanIO::SetCommand(unsigned short wCmd)
{
    unsigned short wLastCnt = 0;

    // 没有注册此数据
    if (d->mICmdCntPos == d->mOCmdCntPos) return -2;

    // 检查设备连接
    if (LinkToRemoteDevice() == false) return -3;
    if (RemoteDeviceExist() == false) return -1;

    // 设备忙
    if (GetIoU16(d->mICmdCntPos) != GetIoU16(d->mOCmdCntPos)) return 1;

    SetIoU16(d->mICmdPos, wCmd);
    wLastCnt = GetIoU16(d->mOCmdCntPos);
    SetIoU16(d->mICmdCntPos, wLastCnt + 1);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置命令带参数
参数描述：
*   @ unsigned short byCmd 输入命令
返回值描述：
*   @ - 2 参数未定义
*   @ - 1 设备断开
*   @  0 执行成功
*   @  1 设备忙
说明：
*   @  无
*******************************************************************************/
int XcanIO::SetCommand(unsigned short byCmd, unsigned short * data, int dataLen)
{
    unsigned short wLastCnt = 0;
    unsigned int wICmdDatatPos = 0;
    wICmdDatatPos = d->mICmdDataPos;
    // 没有注册此数据
    if (d->mICmdCntPos == d->mOCmdCntPos) return -2;

    // 检查设备连接
    if (LinkToRemoteDevice() == false) return -3;
    if (RemoteDeviceExist() == false) return -1;

    // 设备忙
    if (GetIoU16(d->mICmdCntPos) != GetIoU16(d->mOCmdCntPos)) return 1;

    SetIoU16(d->mICmdPos, byCmd);
    wLastCnt = GetIoU16(d->mOCmdCntPos);
    SetIoU16(d->mICmdCntPos, wLastCnt + 1);

    if (dataLen == 0) return 0;
    else {
        for (int i = 0; i < dataLen; i++) {
            SetIoU16(wICmdDatatPos, *data++);
            wICmdDatatPos = wICmdDatatPos + 2;
        }
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ get dev status
参数描述：
*   @ empty
返回值描述：
*   @ -2 参数未定义
*   @ -1 设备断开
*   @  0 执行成功
*   @  1 设备忙
说明：
*   @  无
*******************************************************************************/
int XcanIO::GetState(void)
{
    // 没有注册此数的据
    if (d->mICmdCntPos == d->mOCmdCntPos) return -2;

    // 检查设备连接
    if (LinkToRemoteDevice() == false) return -3;
    if (RemoteDeviceExist() == false) return -1;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置命令
参数描述：
*   @ unsigned char byCmd 输入命令
*   @ void (*pHandler) (unsigned int)
返回值描述：
*   @ -2 参数未定义
*   @ -1 设备断开
*   @  0 执行成功
*   @  1 设备忙
说明：
*   @  无
*******************************************************************************/
int XcanIO::SetCommand(unsigned char byCmd, void(*pHandler) (unsigned int))
{
    unsigned char byLastCnt = 0;

    // 没有注册此数据
    if (d->mICmdCntPos == d->mOCmdCntPos) return -2;

    // 检查设备连接
    if (LinkToRemoteDevice() == false) return -1;
    if (RemoteDeviceExist() == false) return -3;

    // 设备忙
    if (GetIoU8(d->mICmdCntPos) != GetIoU8(d->mOCmdCntPos)) return 1;

    SetIoU8(d->mICmdPos, byCmd);
    byLastCnt = GetIoU8(d->mOCmdCntPos);
    SetIoU8(d->mICmdCntPos, byLastCnt + 1);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 检查命令是否执行完成
参数描述：
*   @ 无
返回值描述：
*   @ true 忙
*   @ false 空闲
说明：
*   @  无
*******************************************************************************/
bool XcanIO::IsCommandExcEnd(void)
{
    return (GetIoU8(d->mICmdCntPos) != GetIoU8(d->mOCmdCntPos)) ? true : false;
}

/*******************************************************************************
函数功能：
*   @ 检查命令是否执行完成
参数描述：
*   @ 无
返回值描述：
*   @ true 忙
*   @ false 空闲
说明：
*   @  无
*******************************************************************************/
bool XcanIO::IsCommandExcEndW(void)
{
    return (GetIoU16(d->mICmdCntPos) != GetIoU16(d->mOCmdCntPos)) ? true : false;
}

/*******************************************************************************
函数功能：
*   @ 检查远程设备忙
参数描述：
*   @ 无
返回值描述：
*   @ true 忙
*   @ false 空闲
说明：
*   @  无
*******************************************************************************/
bool XcanIO::IsRemoteBusy(void)
{
    return (GetIoU8(d->mICmdCntPos) != GetIoU8(d->mOCmdCntPos)) ? true : false;
}

/*******************************************************************************
函数功能：
*   @ 确定将数据写入远程设备，对于绑定设备无效
参数描述：
*   @ unsigned int dwPos: 数据的其实位置
*	@ unsigned char byDat: 需要写入的数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanIO::DeepWrite(unsigned int dwPos, unsigned char byDat)
{
    return mXcan->MustToRemote(d->mUUID, dwPos, byDat, nullptr);
}

/*******************************************************************************
函数功能：
*   @ 写入数据
参数描述：
*   @ short unsigned int wAddr 开始地址
*   @ unsigned int dwLen 长度
*   @ unsigned char *pbyDat 需要写入的数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanIO::Write(short unsigned int wAddr, unsigned int dwLen, unsigned char *pbyDat)
{
    if (mXcan->IsWrBusy(d->mUUID) == true) return false;
    if(d->mRdWrBuff == nullptr) return false;

    d->mReadWriteSAddr = wAddr;
    d->mReadWriteLen = dwLen;
    d->mReadWriteEnd = false;

    memcpy(d->mRdWrBuff, pbyDat, dwLen);
    int nRet = mXcan->Write(d->mUUID, wAddr, dwLen, d->mRdWrBuff, 0, false);
    if (nRet != 0) {
        d->mReadWriteEnd = true;
        return false;
    }

    return true;
}

/*******************************************************************************
函数功能：
*   @ 读取数据
参数描述：
*   @ short unsigned int wAddr 开始地址
*   @ unsigned int dwLen 长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanIO::Read(short unsigned int wAddr, unsigned int dwLen)
{
    if (mXcan->IsRdBusy(d->mUUID) == true) return false;

    d->mReadWriteSAddr = wAddr;
    d->mReadWriteLen = dwLen;
    d->mReadWriteEnd = false;

    int nRet = mXcan->Read(d->mUUID, wAddr, dwLen, 0, false);
    if (nRet != 0) {
        d->mReadWriteEnd = true;
        return false;
    }

    return true;
}

/*******************************************************************************
函数功能：
*   @ 检查读写数据是否完成
参数描述：
*   @ 无
返回值描述：
*   @ true: 操作完成 false:操作未完成
说明：
*   @ 无
*******************************************************************************/
bool XcanIO::IsReadWriteBusy(void)
{
    return (d->mReadWriteEnd == true)?false:true;
}

/*******************************************************************************
函数功能：
*   @ 读取数据
参数描述：
*   @ unsigned char* pbyOut 输出缓冲地址
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanIO::ReadGetData(unsigned char* pbyOut)
{
    if(d->mReadWriteEnd == false || pbyOut == nullptr || d->mRdWrBuff == nullptr || d->mReadWriteLen == 0) return false;

    GetMutex();
    memcpy(pbyOut, d->mRdWrBuff, d->mReadWriteLen);
    PutMutex();

    return true;
}

/*******************************************************************************
函数功能：
*   @ 从指定的位置读取32位数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
unsigned int XcanIO::GetIoU32(unsigned int dwStartPos)
{
    if (d->mIoSrc == nullptr) return 0;
    if ((dwStartPos + 4) > d->mIoLen) return 0;

    GetMutex();
    unsigned int dwData = d->mIoSrc[dwStartPos + 3];
    dwData = (short unsigned int)(dwData << 24);

    unsigned int nData = d->mIoSrc[dwStartPos + 2];
    dwData |= (short unsigned int)(nData << 16);

    nData = d->mIoSrc[(int)dwStartPos + 1];
    dwData |= (short unsigned int)(nData << 8);

    dwData |= d->mIoSrc[(int)dwStartPos];
    PutMutex();

    return dwData;
}

/*******************************************************************************
函数功能：
*   @ 从指定的位置设置32位数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ unsigned int dwIn：需要设置的数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoU32(unsigned int dwStartPos, unsigned int dwIn)
{
    if (d->mIoSrc == nullptr) return;
    if ((dwStartPos + 4) > d->mIoLen) return;

    GetMutex();
    d->mIoSrc[dwStartPos]      = (unsigned char)(dwIn & 0xff);
    d->mIoSrc[dwStartPos + 1]  = (unsigned char)((dwIn & 0xff00) >> 8);
    d->mIoSrc[dwStartPos + 2]  = (unsigned char)((dwIn & 0xff0000) >> 16);
    d->mIoSrc[dwStartPos + 3]  = (unsigned char)((dwIn & 0xff000000) >> 24);
    PutMutex();
}

/*******************************************************************************
函数功能：
*   @ 取得IO资源的16位数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
short unsigned int XcanIO::GetIoU16(unsigned int dwStartPos)
{
    if (d->mIoSrc == nullptr) return 0;
    if ((dwStartPos + 2) > d->mIoLen) return 0;

    GetMutex();
    short unsigned int wData = d->mIoSrc[(int)dwStartPos + 1];
    wData <<= 8;

    wData |= d->mIoSrc[(int)dwStartPos];
    PutMutex();

    return wData;
}

/*******************************************************************************
函数功能：
*   @ 从指定的位置设置16位数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ short unsigned int wIn：需要设置的数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoU16(unsigned int dwStartPos, short unsigned int wIn)
{
    if (d->mIoSrc == nullptr) return;
    if ((dwStartPos + 2) > d->mIoLen) return;

    GetMutex();
    d->mIoSrc[dwStartPos] = (unsigned char)(wIn & 0xff);
    d->mIoSrc[dwStartPos + 1] = (unsigned char)((wIn & 0xff00) >> 8);
    PutMutex();
}

/*******************************************************************************
函数功能：
*   @ 取得IO资源的8位数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
unsigned char XcanIO::GetIoU8(unsigned int dwStartPos)
{
    if (d->mIoSrc == nullptr) return 0;
    if ((dwStartPos) > d->mIoLen) return 0;

    unsigned char byRet = 0;
    GetMutex();
    byRet = d->mIoSrc[dwStartPos];
    PutMutex();

    return byRet;
}

/*******************************************************************************
函数功能：
*   @ 从指定的位置设置8位数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ unsigned char：需要设置的数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanIO::SetIoU8(unsigned int dwStartPos, unsigned char byIn)
{
    if (d->mIoSrc == nullptr) return;
    if ((dwStartPos) > d->mIoLen) return;

    GetMutex();
    d->mIoSrc[dwStartPos] = byIn;
    PutMutex();
}

/*******************************************************************************
函数功能：
*   @ 设置IO资源的32位数组数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ unsigned int *pdwIn：需要设置的数据
*	@ insigned int dwLen 数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanIO::SetIoArrayU32(unsigned int dwStartPos, unsigned int *pdwIn, unsigned int dwLen)
{
    if (d->mIoSrc == nullptr) return -1;
    if (!pdwIn) return -1;

    GetMutex();
    memcpy( &d->mIoSrc[(int)dwStartPos], pdwIn, dwLen * 4);
    PutMutex();

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取IO资源的32位数组数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ insigned int dwLen 数据长度
*	@ unsigned int *pdwOut：需要设置的数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanIO::GetIoArrayU32(unsigned int dwStartPos, unsigned int dwLen, unsigned int *pdwOut)
{
    if (!pdwOut) return -1;
    if (d->mIoSrc == nullptr) return -1;

    GetMutex();
    memcpy(pdwOut, &d->mIoSrc[(int)dwStartPos], dwLen * 4);
    PutMutex();

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置IO资源的16位数组数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ short unsigned int *pdwIn：需要设置的数据
*	@ insigned int dwLen 数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanIO::SetIoArrayU16(unsigned int dwStartPos, short unsigned int *pwIn, unsigned int dwLen)
{
    if (d->mIoSrc == nullptr) return -1;

    GetMutex();
    memcpy( &d->mIoSrc[(int)dwStartPos], pwIn, dwLen * 2);
    PutMutex();

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取IO资源的16位数组数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ insigned int dwLen 数据长度
*	@ unsigned int *pdwOut：需要设置的数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanIO::GetIoArrayU16(unsigned int dwStartPos, unsigned int dwLen, short unsigned int *pwOut)
{
    if (!pwOut) return -1;
    if (d->mIoSrc == nullptr) return 0;

    GetMutex();
    memcpy(pwOut, &d->mIoSrc[(int)dwStartPos], dwLen * 2);
    PutMutex();

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置IO资源的8位数组数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ unsigned char *pbyIn：需要设置的数据
*	@ insigned int dwLen 数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanIO::SetIoArrayBytes(unsigned int dwStartPos, unsigned char *pbyIn, unsigned int dwLen)
{
    if (d->mIoSrc == nullptr) return -1;
    if ((dwStartPos + dwLen) > d->mIoLen) return -1;

    GetMutex();
    memcpy(&d->mIoSrc[(int)dwStartPos], pbyIn, dwLen);
    PutMutex();

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取IO资源的8位数组数据
参数描述：
*   @ unsigned int dwStartPos: 数据的起始位置
*	@ insigned int dwLen 数据长度
*	@ unsigned int *pdwOut：需要设置的数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanIO::GetIoArrayBytes(unsigned int dwStartPos, unsigned int dwLen, unsigned char *pbyOut)
{
    if (!pbyOut) return -1;
    if (d->mIoSrc == nullptr) return -1;
    if ((dwStartPos + dwLen) > d->mIoLen) return -1;

    GetMutex();
    memcpy(pbyOut, &d->mIoSrc[(int)dwStartPos], dwLen);
    PutMutex();

    return 0;
}
