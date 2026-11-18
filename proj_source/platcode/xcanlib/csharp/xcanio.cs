/*******************************************************************************
文件名称：   XcanSdk.cs
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   201801021
程序用途：   xcan协议C#主体类,控制IO结构资源的管理

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
修改时间：
修改人：     
修改内容：  
*******************************************************************************/
using System;
using System.Collections.Generic;
using System.Threading;
using System.Runtime.InteropServices;

namespace XcanSdk
{
    public class XcanIO : XcanNet
    {
        /// <summary>
        /// 资源数据
        /// </summary>
        public class XcanIoDesc
        {
            public XcanIO mIoThis;
            public string mName;            // 名称
            public UInt32 mPosition;        // 位置
            public UInt16 mLen;             // 数据长度
            public Iotype mType;            // 类型
            public UInt32 mArrayLen;        // 数组长度

            /// <summary>
            /// 用于回调数据，数据改变后将回调
            /// </summary>
            public delegate void CallBackVoid();
            private CallBackVoid mCallBack;
            private UInt32[] mMyDat = null;

            public XcanIoDesc(XcanIO pIoThis, string name, Iotype dwType, UInt16 wArrayLen)
            {
                mIoThis = pIoThis;
                mName = name;
                mPosition = pIoThis.mDwLenCnt;
                mLen = (UInt16)(wArrayLen * (UInt16)dwType);
                mType = dwType;
                mArrayLen = wArrayLen;

                pIoThis.mDwLenCnt += mLen;
                pIoThis.mIoDescList.Add(this);
                mCallBack = null;
            }

            public enum Iotype : int
            {
                IO_U8 = 1,
                IO_U16 = 2,
                IO_U32 = 4
            };

            public UInt32[] Value()
            {
                UInt32[] ret = new UInt32[mArrayLen];
                if (mIoThis.RemoteDeviceExist() == false) return ret;

                if (mArrayLen == 1)
                {
                    UInt32 retDW = 0;
                    if (mType == Iotype.IO_U8) retDW = (UInt32)mIoThis.GetIoU8(mPosition);
                    else if(mType == Iotype.IO_U16) retDW = (UInt32)mIoThis.GetIoU16(mPosition);
                    else retDW = (UInt32)mIoThis.GetIoU32(mPosition);

                    ret[0] = retDW;
                }
                else
                {
                    if (mType == Iotype.IO_U8)
                    {
                        byte[] get = null;
                        get = mIoThis.GetIoArrayBytes(mPosition, mArrayLen);
                        for (UInt32 i = 0; i < mArrayLen; i++) ret[i] = (UInt32)get[i];
                    }
                    else if (mType == Iotype.IO_U16)
                    {
                        UInt16[] get = null;
                        get = mIoThis.GetIoArrayU16(mPosition, mArrayLen);
                        for (UInt32 i = 0; i < mArrayLen; i++) ret[i] = (UInt32)get[i];
                    }
                    else
                    {
                        UInt32[] get = null;
                        get = mIoThis.GetIoArrayU32(mPosition, mArrayLen);
                        for (UInt32 i = 0; i < mArrayLen; i++) ret[i] = get[i];
                    }
                }

                return ret;
            }

            /// <summary>
            /// 设置参数32
            /// </summary>
            /// <param name="dwIn">数据32</param>
            public void Value(UInt32 dwIn)
            {
                if (mIoThis.RemoteDeviceExist() == false) return;
                mIoThis.SetIoU32(mPosition, dwIn);
            }

            /// <summary>
            /// 设置参数16
            /// </summary>
            /// <param name="dwIn">数据16</param>
            public void Value(UInt16 wIn)
            {
                if (mIoThis.RemoteDeviceExist() == false) return;
                mIoThis.SetIoU16(mPosition, wIn);
            }

            /// <summary>
            /// 设置参数8
            /// </summary>
            /// <param name="dwIn">数据8</param>
            public void Value(byte byIn)
            {
                if (mIoThis.RemoteDeviceExist() == false) return;
                mIoThis.SetIoU8(mPosition, byIn);
            }

            /// <summary>
            /// 设置参数32 数组
            /// </summary>
            /// <param name="pIn">数组16</param>
            public void Value(UInt32[] pIn)
            {
                if (mIoThis.RemoteDeviceExist() == false) return;
                if (pIn.Length > mArrayLen) return;
                mIoThis.SetIoArrayU32(mPosition, pIn, (UInt32)pIn.Length);
            }

            /// <summary>
            /// 设置参数16 数组
            /// </summary>
            /// <param name="pIn">数组</param>
            public void Value(UInt16[] pIn)
            {
                if (mIoThis.RemoteDeviceExist() == false) return;
                if (pIn.Length > mArrayLen) return;
                mIoThis.SetIoArrayU16(mPosition, pIn, (UInt32)pIn.Length);
            }

            /// <summary>
            /// 设置参数8 数组
            /// </summary>
            /// <param name="pIn">数组8</param>
            public void Value(byte[] pIn)
            {
                if (mIoThis.RemoteDeviceExist() == false) return;
                if (pIn.Length > mArrayLen) return;
                mIoThis.SetIoArrayBytes(mPosition, pIn, (UInt32)pIn.Length);
            }

            /// <summary>
            /// 设置通知接口
            /// </summary>
            /// <param name="pHandler">通知接口</param>
            public void SetRefreshNotice(CallBackVoid pHandler)
            {
                mMyDat = new UInt32[mArrayLen];
                mCallBack = pHandler;

                UInt32[] objVal = Value();
                Array.Copy(objVal, mMyDat, objVal.Length);
            }

            /// <summary>
            /// 检查数据是否变化，并通知更新
            /// </summary>
            public void CheakAndNotice()
            {
                if (mCallBack == null || mMyDat == null) return;

                UInt32[] objVal = Value();
                for (int i = 0; i < objVal.Length; i ++)
                {
                    if(objVal[i] != mMyDat[i])
                    {
                        mCallBack?.Invoke();
                        Array.Copy(objVal, mMyDat, objVal.Length);
                        break;
                    }
                }
            }

            // end
        }

        // uuid
        private string mClassUUID = System.Guid.NewGuid().ToString();

        // IO管理
        private UInt16 mDwLenCnt = 0;
        private List<XcanIoDesc> mIoDescList;

        // 读写标志
        private bool mReadWriteEnd = true;
        private UInt16 mReadWriteSAddr;
        private UInt32 mReadWriteLen;

        // io设备的链表记录
        private class DevIO
        {
            public bool blTrueIO;
            public string mClassUUID;

            public UInt32 dwUUID;
            public UInt16 wRemoteDevID;
            public UInt32 dwComChnID;
            public UInt16 wIoUUID;
            public UInt16 wMyDevID;

            public UInt32 dwICmdCntPos = 0;
            public UInt32 dwOCmdCntPos = 0;

            public unsafe byte* pbyIoSrc = null;
            public unsafe byte* pbyIoRdWr = null;
            public UInt32 dwIoLen;

            /// <summary>
            /// 本类句柄
            /// </summary>
            public XcanIO pThis;
        }

        /// <summary>
        /// 桥接链表元素
        /// </summary>
        private class XcanBridge
        {
            public XcanIO pThis;
            public bool blIsAdd;

            public UInt32 mRemoteDevID;
            public UInt32 mCommChnID;
        };

        // 类内存在桥接
        private class XcanBridgeCell
        {
            public UInt32 mUUID;
            public UInt32 mRemoteDevID;
            public UInt32 mCommChnID;
            public UInt16 mIoUUID;
            public UInt16 mMyDevID;

            public unsafe byte* mIoSrc;
            public unsafe byte* mIoBuff;
            public unsafe byte* mShadow;
            public unsafe byte* mShadowBuff;
            public unsafe byte* mShadowCache;

            public GCHandle mGCHandleIoBuff;
            public GCHandle mGCHandleShadow;
            public GCHandle mGCHandleShadowBuff;
            public GCHandle mGCHandleShadowCache;
        } ;

        // 接口通信质量数据接口
        public class stTrsCommSta {
            public UInt16 wTxSN;
            public UInt16 wTxID;
            public UInt16 wRxSN;
            public UInt16 wRxID;
            public UInt32 dwTrsLost;
        };

        private XcanBridgeCell mBridge;

        public  UInt32 mUUID;
        public  UInt16 mRemoteDevID;
        public  UInt32 mCommChnID;
        public  UInt16 mIoUUID;
        public  UInt16 mMydevID;
        
        private unsafe byte* mIoSrc;
        private unsafe byte* mIoBuff;
        private unsafe byte* mShadow;
        private unsafe byte* mShadowBuff;
        private unsafe byte* mShadowCache;
        private unsafe byte* mIoRdWr;

        private GCHandle mGCHandleIoSrc;
        private GCHandle mGCHandleIoBuff;
        private GCHandle mGCHandleShadow;
        private GCHandle mGCHandleShadowBuff;
        private GCHandle mGCHandleShadowCache;
        private GCHandle mGCHandleRdWrBuff;

        private UInt32 mIoLen;
        private UInt32 mIoBuffLen;
        private UInt32 mIoShadowLen;
        private UInt32 mIoShadowBuffLen;
        private UInt32 mIoShadowCacheLen;

        private UInt16 mIOInStart;
        private UInt16 mIOOutStart;
        private UInt16 mIOSyncEnd = 0;

        private UInt16 mIODSyncEnd = 0;
        private UInt16 mIODEnd = 0;

        // 数据命令执行方法
        private UInt32 mICmdPos;
        private UInt32 mICmdCntPos;
        private UInt32 mICmdDataPos;
        private UInt32 mOCmdCntPos;

        // 私有链表
        private static List<DevIO> mListDevIo;
        private static List<XcanBridge> mListBridge;

        private bool mIimVritualIO = true;
        // 数据更新事件接口
        public delegate void pNoticeHanlder();
        private pNoticeHanlder mpNotice;
        private static DevXcanTdf.OnRegReceiveNoticeHanlder mNoticeHander = null;

        /// <summary>
        /// 构造函数，定义一个数据结构
        /// </summary>
        /// <param name="dwIoLen"></param>
        /// <param name="xcanNetDev"></param>
        public XcanIO()
        {
            mpNotice = null;

            mIimVritualIO = true;
            if (mListDevIo == null) mListDevIo = new List<DevIO>();
            if (mListBridge == null) mListBridge = new List<XcanBridge>();
            if (mIoDescList == null) mIoDescList = new List<XcanIoDesc>();
            if (mNoticeHander == null) mNoticeHander = ReceiveNoticeHandler;

            // 私有方法
            mUUID = 0;
            mRemoteDevID = 0;
            mCommChnID = 0;
            mIoUUID = 0;

            unsafe
            {
                mIoSrc = null;
                mIoBuff = null;
                mShadow = null;
                mShadowBuff = null;
            }

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

            mBridge = new XcanBridgeCell();
            mBridge.mUUID = 0;
            mBridge.mRemoteDevID = 0;
            mBridge.mCommChnID = 0;
            mBridge.mIoUUID = 0;

            unsafe
            {
                mBridge.mIoSrc = null;
                mBridge.mIoBuff = null;
                mBridge.mShadow = null;
                mBridge.mShadowBuff = null;
            }
        }

        /// <summary>
        /// 析构函数,回收不用的数据
        /// </summary>
        ~XcanIO()
        {
            // 删除list元素
            for (int i = 0; i < mListDevIo.Count; i++)
            {
                if (mListDevIo[i].mClassUUID == mClassUUID)
                {
                    mListDevIo.Remove(mListDevIo[i]);
                    break;
                }
            }

            if (mBridge != null)
            {
                mXcan.onRemovePort?.Invoke(mBridge.mUUID);                                      // 回收C库资源
                if (mBridge.mGCHandleIoBuff.IsAllocated) mBridge.mGCHandleIoBuff.Free();
                if (mBridge.mGCHandleShadow.IsAllocated) mBridge.mGCHandleShadow.Free();
                if (mBridge.mGCHandleShadowBuff.IsAllocated) mBridge.mGCHandleShadowBuff.Free();
            }

            if(mIimVritualIO == false)
            {
                if (mGCHandleIoSrc.IsAllocated) mGCHandleIoSrc.Free();
                if (mGCHandleIoBuff.IsAllocated) mGCHandleIoBuff.Free();
                if (mGCHandleShadow.IsAllocated) mGCHandleShadow.Free();
                if (mGCHandleShadowBuff.IsAllocated) mGCHandleShadowBuff.Free();
                if (mGCHandleRdWrBuff.IsAllocated) mGCHandleRdWrBuff.Free();

                mXcan.onRemovePort?.Invoke(mBridge.mUUID);                                      // 回收C库资源
                mIoDescList.Clear();
            }
        }

        /// <summary>
        /// 初始化io资源计数器
        /// </summary>
        public void SetIoInsertStart()
        {
            mDwLenCnt = 0;
        }

        /// <summary>
        /// 设置自动同步结束地址，包括in和out
        /// </summary>
        public void SetIoDSyncEnd()
        {
            mIODSyncEnd = (UInt16)mDwLenCnt;
        }

        /// <summary>
        /// 自动设置io同步资源的起始
        /// </summary>
        public void SetIoSynInStart()
        {
            mIOInStart = (UInt16)mDwLenCnt;
        }

        /// <summary>
        /// 自动同步设置io资源的输出起始
        /// </summary>
        public void SetIoSynOutStart()
        {
            mIOOutStart = (UInt16)mDwLenCnt;
        }

        /// <summary>
        /// 自动同步设置IO资源的结束
        /// </summary>
        public void SetIoSynEnd()
        {
            mIOSyncEnd = (UInt16)mDwLenCnt;
        }

        /// <summary>
        /// 设置结束长度和资源长度
        /// </summary>
        public void SetIoEnd()
        {
            mIoLen = mDwLenCnt;
            mIODEnd = mDwLenCnt;

            foreach(XcanIoDesc io in mIoDescList)
            {
                if (io.mName == "mInCmd") mICmdPos = io.mPosition;
                else if (io.mName == "mInCmdCnt") mICmdCntPos = io.mPosition;
                else if (io.mName == "mOutCmdCnt") mOCmdCntPos = io.mPosition;
            }
        }

        /// <summary>
        /// 增加一个具体的IO资源数据，此数据将和远程设备进行通信绑定。
        /// </summary>
        /// <param name="dwRemoteDevID">远程设备ID</param>
        /// <param name="dwComChnID">通信端口ID</param>
        /// <param name="wIoUUID">资源ID</param>
        /// <param name="dwIoLen">资源长度</param>
        /// <param name="blIsRx">是否是接收资源</param>
        public void LinkToDevice(UInt16 wRemoteDevID, UInt32 dwCommChnID, UInt16 wIoUUID, bool blIsRx)
        {
            XcanSuportDev cDev = new XcanSuportDev();

            // 检查设备
            cDev.dwCommChnID = 0;
            foreach (XcanSuportDev dev in mListCommNet)
            {
                if (dev.dwCommChnID == dwCommChnID)
                {
                    cDev = dev;
                    break;
                }
            }

            // 未找到对应的通信接口
            // if (cDev.dwCommChnID == 0) return;

            // 检查否是资源重复
            DevIO cIO = new DevIO();
            cIO.wIoUUID = 0;
            foreach (DevIO io in mListDevIo) 
            {
                if (io.dwComChnID == dwCommChnID && io.wIoUUID == wIoUUID && io.wRemoteDevID == wRemoteDevID && io.blTrueIO == true)
                {
                    cIO = io;
                    break;
                }
            }

            // 已经有存在的实体，构建虚拟的IO
            if (cIO.wIoUUID == wIoUUID)
            {
                if (cIO.dwIoLen == 0) return;
                DevIO cAdd = new DevIO();

                cAdd.blTrueIO = false;
                cAdd.dwIoLen = mIoLen = cIO.dwIoLen;

                unsafe
                {
                    cAdd.pbyIoSrc = mIoSrc = cIO.pbyIoSrc;
                    cAdd.pbyIoRdWr = mIoRdWr = cIO.pbyIoRdWr;
                }

                cAdd.wMyDevID = mMydevID = cIO.wMyDevID;
                cAdd.wRemoteDevID = mRemoteDevID = cIO.wRemoteDevID;
                cAdd.dwComChnID = mCommChnID = cIO.dwComChnID;
                cAdd.wIoUUID = mIoUUID = cIO.wIoUUID;
                cAdd.dwUUID = mUUID = cIO.dwUUID;

                cAdd.dwICmdCntPos = mICmdCntPos = cIO.dwICmdCntPos;
                cAdd.dwOCmdCntPos = mOCmdCntPos = cIO.dwOCmdCntPos;
                cAdd.mClassUUID = mClassUUID;
                cAdd.pThis = this;

                mListDevIo.Add(cAdd);
                return;
            }

            if (mIoLen == 0) return;

            mIimVritualIO = false;
            cIO.blTrueIO = true;
            cIO.wIoUUID = wIoUUID;
            cIO.wMyDevID = GetMyNetDevID();
            cIO.wRemoteDevID = wRemoteDevID;
            cIO.dwComChnID = dwCommChnID;
            cIO.dwIoLen = mIoLen;
            cIO.dwICmdCntPos = mICmdCntPos;
            cIO.dwOCmdCntPos = mOCmdCntPos;

            // 创建缓冲长度
            mIoBuffLen = mIoLen + 8;
            mIoShadowLen = mIoBuffLen;
            mIoShadowBuffLen = mIoLen + (mIoLen / 8);
            mIoShadowCacheLen = mIoBuffLen;

            mMydevID = GetMyNetDevID();
            mRemoteDevID = wRemoteDevID;
            mCommChnID = dwCommChnID;
            mIoUUID = wIoUUID;

            // 开始申请地址, 并转换成非托管内存
            byte[] byIoSrc = new byte[mIoLen];
            mGCHandleIoSrc = GCHandle.Alloc(byIoSrc, GCHandleType.Pinned);
            mGCHandleIoSrc.AddrOfPinnedObject();

            byte[] byIoBuff = new byte[mIoBuffLen];
            mGCHandleIoBuff = GCHandle.Alloc(byIoBuff, GCHandleType.Pinned);
            mGCHandleIoBuff.AddrOfPinnedObject();

            byte[] byIoShadow = new byte[mIoShadowLen];
            mGCHandleShadow = GCHandle.Alloc(byIoShadow, GCHandleType.Pinned);
            mGCHandleShadow.AddrOfPinnedObject();

            byte[] byIoShadowBuff = new byte[mIoShadowBuffLen];
            mGCHandleShadowBuff = GCHandle.Alloc(byIoShadowBuff, GCHandleType.Pinned);
            mGCHandleShadowBuff.AddrOfPinnedObject();

            byte[] byIoShadowCache = new byte[mIoShadowCacheLen];
            mGCHandleShadowCache = GCHandle.Alloc(byIoShadowCache, GCHandleType.Pinned);
            mGCHandleShadowCache.AddrOfPinnedObject();

            byte[] byIoRdWrBuff = new byte[mIoBuffLen];
            mGCHandleRdWrBuff = GCHandle.Alloc(byIoRdWrBuff, GCHandleType.Pinned);
            mGCHandleRdWrBuff.AddrOfPinnedObject();

            unsafe
            {
                fixed (byte* pbyIOTmp = byIoSrc)
                {
                    cIO.pbyIoSrc = mIoSrc = pbyIOTmp;
                }

                fixed (byte* pbyIOTmp = byIoBuff)
                {
                    mIoBuff = pbyIOTmp;
                }

                fixed (byte* pbyIOTmp = byIoShadow)
                {
                    mShadow = pbyIOTmp;
                }

                fixed (byte* pbyIOTmp = byIoShadowBuff)
                {
                    mShadowBuff = pbyIOTmp;
                }

                fixed (byte* pbyIOTmp = byIoShadowCache)
                {
                    mShadowCache = pbyIOTmp;
                }

                fixed (byte* pbyIOTmp = byIoRdWrBuff)
                {
                    cIO.pbyIoRdWr = mIoRdWr = pbyIOTmp;
                }

                if (blIsRx == true)
                {
                    if(cDev.dwCommChnID != 0) mUUID = mXcan.onAddRxPort(ref cDev.Dev, mRemoteDevID, mIoUUID, mIoSrc, mIoBuff, mIoLen);
                    else mUUID = mXcan.onAddIo(mRemoteDevID, dwCommChnID, mIoUUID, mIoSrc, mIoBuff, mIoLen, true);

                    mXcan.onServicePortInit(mUUID, mShadow, mShadowBuff, mShadowCache);
                    mXcan.onSynSet(mUUID, true, mIOOutStart, mIOSyncEnd, mIODSyncEnd, mIODEnd);
                }
                else
                {
                    if (cDev.dwCommChnID != 0) mUUID = mXcan.onAddTxPort(ref cDev.Dev, mRemoteDevID, mIoUUID, mIoSrc, mIoBuff, mIoLen);
                    else mUUID = mXcan.onAddIo(mRemoteDevID, dwCommChnID, mIoUUID, mIoSrc, mIoBuff, mIoLen, false);

                    mXcan.onServicePortInit(mUUID, mShadow, mShadowBuff, mShadowCache);
                    mXcan.onSynSet(mUUID, true, mIOInStart, mIOOutStart, mIODSyncEnd, mIODEnd);
                }
            }

            // 注册C接口的通知接口
            mXcan.onRegReceiveNotice(mUUID, mNoticeHander);
            mXcan.onRegReadWriteNotice(mUUID, RdWrNoticeHandler);

            cIO.dwUUID = mUUID;
            cIO.pThis = this;
            cIO.mClassUUID = mClassUUID;
            mListDevIo.Add(cIO);
        }

        /// <summary>
        /// 资源桥接到远程的设备
        /// </summary>
        /// <param name="pBridge">桥接数据</param>
        /// <returns></returns>
        private int BridgeInsert(XcanBridge pBridge)
        {
            UInt32 dwComChnID = pBridge.mCommChnID;
            UInt32 dwRemoteDevID = pBridge.mRemoteDevID;
            XcanIO pThis = pBridge.pThis;

            XcanSuportDev cDev = new XcanSuportDev();

            // 检查设备
            cDev.dwCommChnID = 0;
            foreach (XcanSuportDev dev in mListCommNet)
            {
                if (dev.dwCommChnID == dwComChnID)
                {
                    cDev = dev;
                    break;
                }
            }

            if (cDev.dwCommChnID != dwComChnID) return -1;

            // 插入到资源列表,在列表中没有对应的数据时，将数据加入列表，并注册C 库
            pThis.mBridge.mCommChnID = dwComChnID;
            pThis.mBridge.mRemoteDevID = dwRemoteDevID;
            pThis.mBridge.mMyDevID = mMydevID;

            // 创建IO资源所需要的数据缓存
            pThis.mBridge.mIoUUID = pThis.mIoUUID;
            unsafe
            {
                pThis.mBridge.mIoSrc = pThis.mIoSrc;
            }

            // 开始申请地址, 并转换成非托管内存
            byte[] byIoBuff = new byte[pThis.mIoBuffLen];
            pThis.mBridge.mGCHandleIoBuff = GCHandle.Alloc(byIoBuff, GCHandleType.Pinned);
            pThis.mBridge.mGCHandleIoBuff.AddrOfPinnedObject();

            byte[] byIoShadow = new byte[pThis.mIoShadowLen];
            pThis.mBridge.mGCHandleShadow = GCHandle.Alloc(byIoShadow, GCHandleType.Pinned);
            pThis.mBridge.mGCHandleShadow.AddrOfPinnedObject();

            byte[] byIoShadowBuff = new byte[pThis.mIoShadowBuffLen];
            pThis.mBridge.mGCHandleShadowBuff = GCHandle.Alloc(byIoShadowBuff, GCHandleType.Pinned);
            pThis.mBridge.mGCHandleShadowBuff.AddrOfPinnedObject();

            byte[] byIoShadowCache = new byte[pThis.mIoShadowCacheLen];
            pThis.mBridge.mGCHandleShadowCache = GCHandle.Alloc(byIoShadowCache, GCHandleType.Pinned);
            pThis.mBridge.mGCHandleShadowCache.AddrOfPinnedObject();

            unsafe
            {
                fixed (byte* pbyIOTmp = byIoBuff)
                {
                    pThis.mBridge.mIoBuff = pbyIOTmp;
                }

                fixed (byte* pbyIOTmp = byIoShadow)
                {
                    pThis.mBridge.mShadow = pbyIOTmp;
                }

                fixed (byte* pbyIOTmp = byIoShadowBuff)
                {
                    pThis.mBridge.mShadowBuff = pbyIOTmp;
                }

                fixed (byte* pbyIOTmp = byIoShadowCache)
                {
                    pThis.mBridge.mShadowCache = pbyIOTmp;
                }
                pThis.mBridge.mUUID = mXcan.onAddTxPort(ref cDev.Dev, pThis.mBridge.mRemoteDevID, pThis.mBridge.mIoUUID,
                                             pThis.mBridge.mIoSrc, pThis.mBridge.mIoBuff, pThis.mIoLen);
                mXcan.onServicePortInit(pThis.mBridge.mUUID, pThis.mBridge.mShadow, pThis.mBridge.mShadowBuff, pThis.mBridge.mShadowCache);
                mXcan.onSynSet(pThis.mBridge.mUUID, true, pThis.mIOInStart, pThis.mIOOutStart, pThis.mIODSyncEnd, pThis.mIODEnd);
            }

            return 0;
        }

        /// <summary>
        /// 删除资源桥接到远程的设备
        /// </summary>
        /// <param name="pBridge">桥接数据</param>
        /// <returns></returns>
        private int BridgeDelete(XcanBridge pBridge)
        {
            XcanIO pThis = pBridge.pThis;

            if (pThis.mBridge.mUUID == 0) return -1;
            mXcan.onRemovePort(pThis.mBridge.mUUID);
            if (pThis.mGCHandleIoBuff.IsAllocated) pThis.mGCHandleIoBuff.Free();
            if (pThis.mGCHandleShadow.IsAllocated) pThis.mGCHandleShadow.Free();
            if (pThis.mGCHandleShadowBuff.IsAllocated) pThis.mGCHandleShadowBuff.Free();

            return 0;
        }

        /// <summary>
        /// 将资源桥接到远程的设备
        /// </summary>
        /// <param name="dwRemoteDevID">需要绑定的通信接口ID</param>
        /// <param name="dwCommChnID">远程设备的ID</param>
        /// <returns></returns>
        public int BridgeLink(UInt32 dwRemoteDevID, UInt32 dwCommChnID)
        {
            XcanBridge stBridge = new XcanBridge();
            stBridge.blIsAdd = true;
            stBridge.pThis = this;
            stBridge.mCommChnID = dwCommChnID;
            stBridge.mRemoteDevID = dwRemoteDevID;

            GetMutex();
            {
                mListBridge.Add(stBridge);
            }
            PutMutex();
            
            return 0;
        }

        /// <summary>
        /// 断开将资源桥接到远程的设备
        /// </summary>
        /// <param name=""></param>
        /// <returns>
        /// -1 失败
        /// 0 成功
        /// </returns>
        public int BridgeRemove()
        {
            XcanBridge stBridge = new XcanBridge();
            stBridge.blIsAdd = false;

            stBridge.pThis = this;
            stBridge.mCommChnID = mCommChnID;
            stBridge.mRemoteDevID = mRemoteDevID;

            GetMutex();
            {
                mListBridge.Add(stBridge);
            }
            PutMutex();

            return 0;
        }

        /// <summary>
        /// 用于安全删除桥接数据
        /// </summary>
        public static void BridgeService()
        {
            GetMutex();
            {
                if (mListBridge.Count == 0)
                {
                    PutMutex();
                    return;
                }

                foreach (XcanBridge A in mListBridge)
                {
                    if (A.blIsAdd == true) A.pThis.BridgeInsert(A);
                    else A.pThis.BridgeDelete(A);

                    mListBridge.Remove(A);
                }
            }
            PutMutex();
        }

        /// <summary>
        /// 用于IO资源收到数据后触发数据有接收到,将触发所有的回调信息
        /// </summary>
        /// <param name="wIoUUID">接收消息的IO资源ID</param>
        private static void ReceiveNoticeHandler(UInt32 wUUID)
        {
            foreach (DevIO io in mListDevIo)
            {
                if (io.dwUUID == wUUID)
                {
                    io.pThis.mpNotice?.Invoke();
                    io.pThis.BundReceiveNotice();

                    if (io.pThis.mIoDescList == null || io.pThis.mIoDescList.Count == 0) continue;

                    // start
                    foreach(XcanIoDesc ioDesc in io.pThis.mIoDescList)
                    {
                        ioDesc.CheakAndNotice();
                    }
                }
            }
        }

        /// <summary>
        /// 通过继承的方式访问通知接口，在接收到数据更新后将更新此位
        /// </summary>
        public virtual void BundReceiveNotice()
        {
            ;
        }

        /// <summary>
        /// 数据读写完成后的回调通知，static
        /// </summary>
        /// <param name=""></param>
        /// <param name="dwIoUUID">unsigned int dwIoUUID 资源的UUID</param>
        /// <param name=""></param>
        /// <param name="dwOp">unsigned int dwOp 0: write, 1: read</param>
        public static void RdWrNoticeHandler(UInt32 dwIoUUID, UInt32 dwOp)
        {
            DevIO cIO;
            XcanIO pXcanIO;

            foreach (DevIO io in mListDevIo)
            {
                cIO = io;
                if (dwIoUUID != cIO.pThis.mIoUUID) continue;
                pXcanIO = cIO.pThis;
                if (dwOp == 0)
                {
                    unsafe
                    {
                        for (int i = 0; i < pXcanIO.mReadWriteLen; i++) pXcanIO.mIoRdWr[i] = pXcanIO.mIoSrc[pXcanIO.mReadWriteSAddr + i];
                    }
                    // 数据读取完成
                    
                    pXcanIO.mReadWriteEnd = true;
                }
                else
                {
                    // 数据写入完成
                    pXcanIO.mReadWriteEnd = true;
                }
            }
        }

        /// <summary>
        /// 注册回调接口
        /// </summary>
        /// <param name="pNotice">外部的回调接口</param>
        public void SetReceiveNoticeHandler(pNoticeHanlder pNotice)
        {
            mpNotice = pNotice;
        }

        /// <summary>
        /// 检查是否本设备为虚拟的设备
        /// </summary>
        /// <returns>true: 真实设备； false: 虚拟设备</returns>
        public bool IsRealDev()
        {
            return (mIimVritualIO == true) ? false : true;
        }

        /// <summary>
        /// 查询远端设备是否连接到本地
        /// </summary>
        public bool RemoteDeviceExist()
        {
            return mXcan.onSynInLink(mUUID);
        }

        /// <summary>
        /// 查询本地是否连接到远端设备
        /// </summary>
        public bool LinkToRemoteDevice()
        {
            return mXcan.onSynOutLink(mUUID);
        }

        /// <summary>
        /// 设置命令，带命令位置
        /// </summary>
        /// <param name="dwICmdPos">unsigned int dwICmdPos 输入命令位置</param>
        /// <param name="dwICmdCntPos">unsigned int dwICmdCntPos 输入命令计数位置</param>
        /// <param name="dwOCmdCntPos">unsigned int dwOCmdCntPos 输出命令计数位置</param>
        public void SetCmd(UInt32 dwICmdPos, UInt32 dwICmdCntPos, UInt32 dwOCmdCntPos)
        {
            mICmdPos = dwICmdPos;
            mICmdCntPos = dwICmdCntPos;
            mOCmdCntPos = dwOCmdCntPos;
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
        public void SetCmd(UInt32 dwICmdPos, UInt32 dwICmdCntPos, UInt32 dwICmdDataPos, UInt32 dwOCmdCntPos)
        {
            mICmdPos = dwICmdPos;
            mICmdCntPos = dwICmdCntPos;
            mICmdDataPos = dwICmdDataPos;
            mOCmdCntPos = dwOCmdCntPos;
        }

        /// <summary>
        /// 向子设备写入数据，带命令，并监控返回
        /// </summary>
        /// <param name="byCmd">命令</param>
        /// <param name="dwInPos">需要下发的数据位置</param>
        /// <param name="dwInLen">需要下发的数据长度</param>
        /// <param name="dwOutPos">需要监控的接收数据位置</param>
        /// <param name="dwOutLen">需要监控的接收数据长度</param>
        /// <param name="dwDat">需要下发的数据</param>
        /// <param name="dwDatMask">需要和输入对比的数据mask</param>
        /// <returns></returns>
        public int SetCmdWithCheak(byte byCmd, UInt32 dwInPos, UInt32 dwInLen,
                                    UInt32 dwOutPos, UInt32 dwOutLen,
                                    UInt32 dwDat, UInt32 dwDatMask, UInt32 dwWaitTim)
        {
            byte byShit = 0;
            UInt32 dwOutDat = 0;
            UInt32 dwInDat = 0;

            while ((dwDatMask & 0x01) == 0)
            {
                dwDatMask >>= 1;
                byShit++;
                if (byShit > 30) return -1;
            }

            dwInDat = ((dwDat >> byShit) & dwDatMask);

            if (dwOutLen == 1) dwOutDat = (UInt32)GetIoU8(dwOutPos);
            else if (dwOutLen == 2) dwOutDat = (UInt32)GetIoU16(dwOutPos);
            else if (dwOutLen == 4) dwOutDat = (UInt32)GetIoU32(dwOutPos);

            if (dwOutDat == dwInDat) return 0;

            if (dwInLen == 1) SetIoU8(dwInPos, (byte)dwDat);
            else if (dwInLen == 2) SetIoU16(dwInPos, (UInt16)dwDat);
            else if (dwInLen == 4) SetIoU32(dwInPos, (UInt32)dwDat);
            else return -1;

            if (SetCommand(byCmd) != 0) return -1;
            
            for(int i = 0; i < 100 * dwWaitTim; i++)
            {
                if(dwOutLen == 1) dwOutDat = (UInt32)GetIoU8(dwOutPos);
		        else if(dwOutLen == 2) dwOutDat = (UInt32)GetIoU16(dwOutPos);
		        else if(dwOutLen == 4) dwOutDat = (UInt32)GetIoU32(dwOutPos);
		        else return -1;

		        if(dwOutDat == dwInDat) break;
                Thread.Sleep(10);
            }

            if (dwOutDat != dwInDat) return -1;
            return 0;
        }

        /// <summary>
        /// 设置命令
        /// </summary>
        /// <param name="byCmd">输入命令</param>
        /// <returns>
        /// -2 参数未定义
        /// -1 设备断开
        ///  0 执行成功
        ///  1 设备忙
        /// </returns>
        public int SetCommand(byte byCmd)
        {
            byte byLastCnt = 0;

            // 没有注册此数据
            if (mICmdCntPos == mOCmdCntPos) return -2;

            // 检查设备连接
            if (LinkToRemoteDevice() == false) return -3;
            if (RemoteDeviceExist() == false) return -1;

            // 设备忙
            if (GetIoU8(mICmdCntPos) != GetIoU8(mOCmdCntPos)) return 1;

            SetIoU8(mICmdPos, byCmd);
            byLastCnt = GetIoU8(mOCmdCntPos);
            SetIoU8(mICmdCntPos, (byte)(byLastCnt + 1));

            return 0;
        }

        /*******************************************************************************
        函数功能：
        *   @ 设置命令
        参数描述：
        *   @ UInt16 byCmd 输入命令
        返回值描述：
        *   @ -2 参数未定义
        *   @ -1 设备断开
        *   @  0 执行成功
        *   @  1 设备忙
        说明：
        *   @  无
        *******************************************************************************/
        public int SetCommandW(UInt16 wCmd)
        {
            UInt16 wLastCnt = 0;

            // 没有注册此数据
            if (mICmdCntPos == mOCmdCntPos) return -2;

            // 检查设备连接
            if (LinkToRemoteDevice() == false) return -3; // 查询本地是否连接到远端设备
            if (RemoteDeviceExist() == false) return -1; // 查询远端设备是否连接到本地

            // 设备忙
            if (GetIoU16(mICmdCntPos) != GetIoU16(mOCmdCntPos)) return 1;

            SetIoU16(mICmdPos, wCmd);
            wLastCnt = GetIoU16(mOCmdCntPos);

            wLastCnt ++;
            SetIoU16(mICmdCntPos, wLastCnt);

            return 0;
        }

        /*******************************************************************************
        函数功能：
        *   @ 设置命令带参数
        参数描述：
        *   @ UInt16 byCmd 输入命令
        返回值描述：
        *   @ - 2 参数未定义
        *   @ - 1 设备断开
        *   @  0 执行成功
        *   @  1 设备忙
        说明：
        *   @  无
        *******************************************************************************/
        public int SetCommand(UInt16 wCmd, UInt16[] data)
        {
            UInt16 wLastCnt = 0;
            UInt32 dwICmdDatatPos = mICmdDataPos;

            // 没有注册此数据
            if (mICmdCntPos == mOCmdCntPos) return -2;

            // 检查设备连接
            if (LinkToRemoteDevice() == false) return -3;
            if (RemoteDeviceExist() == false) return -1;

            // 设备忙
            if (GetIoU16(mICmdCntPos) != GetIoU16(mOCmdCntPos)) return 1;

            SetIoU16(mICmdPos, wCmd);
            wLastCnt = GetIoU16(mOCmdCntPos);

            wLastCnt ++;
            SetIoU16(mICmdCntPos, wLastCnt);

            if (data.Length == 0) return 0;
            else {
                for (int i = 0; i < data.Length; i++) {
                    SetIoU16(dwICmdDatatPos, data[i]);
                    dwICmdDatatPos = dwICmdDatatPos + 2;
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
        public int GetState()
        {
            // 没有注册此数的据
            if (mICmdCntPos == mOCmdCntPos) return -2;

            // 检查设备连接
            if (LinkToRemoteDevice() == false) return -3;
            if (RemoteDeviceExist() == false) return -1;

            return 0;
        }

        /// <summary>
        /// 检查命令是否执行完成
        /// </summary>
        /// <returns>
        /// true 忙
        /// false 空闲
        /// </returns>
        public bool IsCommandExcEnd()
        {
            return (GetIoU8(mICmdCntPos) != GetIoU8(mOCmdCntPos)) ? true : false;
        }

        /// <summary>
        /// 检查命令是否执行完成
        /// </summary>
        /// <returns>
        /// true 忙
        /// false 空闲
        /// </returns>
        public bool IsCommandExcEndW()
        {
            return (GetIoU16(mICmdCntPos) != GetIoU16(mOCmdCntPos)) ? true : false;
        }

        /// <summary>
        /// 检查远程设备忙
        /// </summary>
        /// <returns>
        /// true 忙
        /// false 空闲
        /// </returns>
        public bool IsRemoteBusy()
        {
            return (GetIoU8(mICmdCntPos) != GetIoU8(mOCmdCntPos)) ? true : false;
        }

        /// <summary>
        /// 取得通信发送包和丢失包的接口
        /// </summary>
        /// <returns>
        /// *   @ unsigned int dwUUID 数据ID
        /// *   @ UInt16 int wTxSN 发送的sn号，每次发送一个包+1
        /// *   @ UInt16 int wTxID sync数据包的ID，当发送成功后，收到对应的ID反馈后+1
        /// *   @ UInt16 int wRxSN 收到的接收方接收序号，没接收一个数据包+1
        /// *   @ UInt16 int wRxID 由接收方，返回的发送ID，接收方不修改，直接返回
        /// *   @ unsigned int wTrsLost 当发送方接收到返回ID与本地不一致时+1
        /// </returns>
        public stTrsCommSta GetTrsCommSta()
        {
            stTrsCommSta cSta = new stTrsCommSta();
            unsafe
            {
                UInt16 wTxSN;
                UInt16 wTxID;
                UInt16 wRxSN;
                UInt16 wRxID;
                UInt32 dwTrsLost;
                
                mXcan.onGetCommSta(mUUID, &wTxSN, &wTxID, &wRxSN, &wRxID, &dwTrsLost);

                cSta.wTxSN = wTxSN;
                cSta.wTxID = wTxID;
                cSta.wRxSN = wRxSN;
                cSta.wRxID = wRxID;
                cSta.dwTrsLost = dwTrsLost;
            }

            return cSta;
        }

        /// <summary>
        /// 确定将数据写入远程设备，对于绑定设备无效
        /// </summary>
        public bool DeepWrite(UInt32 dwPos, byte byDat)
        { 
            return mXcan.onMustToRemote(mUUID, dwPos, byDat, ToCdelayHandler);
        }

        /// <summary>
        /// 写入数据
        /// </summary>
        /// <param name="wAddr">short unsigned int wAddr 开始地址</param>
        /// <param name="dwLen">unsigned int dwLen 长度</param>
        /// <param name="pbyDat">unsigned char *pbyDat 需要写入的数据</param>
        /// <returns></returns>
        public bool Write(UInt16 wAddr, UInt32 dwLen, byte[] pbyDat)
        {
            if (mXcan.onIsWrBusy(mUUID) == true) return false;

            mReadWriteSAddr = wAddr;
            mReadWriteLen = dwLen;
            mReadWriteEnd = false;

            int nRet = 0;
            unsafe
            {
                for (int i = 0; i < dwLen; i++) mIoRdWr[i] = pbyDat[i];
                nRet = mXcan.onWrite(mUUID, wAddr, dwLen, mIoRdWr, 0, false);
            }
            
            if (nRet != 0)
            {
                mReadWriteEnd = true;
                return false;
            }

            return true;
        }

        /// <summary>
        /// 读取数据
        /// </summary>
        /// <param name="wAddr">short unsigned int wAddr 开始地址</param>
        /// <param name="dwLen">unsigned int dwLen 长度</param>
        /// <returns></returns>
        public bool Read(UInt16 wAddr, UInt32 dwLen)
        {
            if (mXcan.onIsRdBusy(mUUID) == true) return false;

            mReadWriteSAddr = wAddr;
            mReadWriteLen = dwLen;
            mReadWriteEnd = false;

            int nRet = mXcan.onRead(mUUID, wAddr, dwLen, 0, false);
            if (nRet != 0)
            {
                mReadWriteEnd = true;
                return false;
            }

            return true;
        }

        /// <summary>
        /// 检查读写数据是否完成
        /// </summary>
        /// <returns>true: 操作完成 false:操作未完成</returns>
        public bool IsReadWriteBusy()
        {
            return (mReadWriteEnd == true) ? false : true;
        }

        /// <summary>
        /// 读取数据
        /// </summary>
        /// <returns>byte[] 输出数据</returns>
        public byte[] ReadGetData()
        {
            byte[] retD = new byte[mReadWriteLen];

            GetMutex();
            unsafe
            {
                for (int i = 0; i < mReadWriteLen; i++) retD[i] = mIoRdWr[i];
            }
            PutMutex();

            return retD;
        }

        /// <summary>
        /// 取得IO资源的32位数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public UInt32 GetIoU32(UInt32 dwStartPos)
        {
            unsafe { if (mIoSrc == null || (dwStartPos + 4) > mIoLen) return 0; }
            UInt32 dwD = 0;

            GetMutex();
            unsafe
            {
                dwD = mIoSrc[dwStartPos + 3];
                dwD <<= 8;

                dwD += mIoSrc[dwStartPos + 2];
                dwD <<= 8;

                dwD += mIoSrc[dwStartPos + 1];
                dwD <<= 8;

                dwD |= mIoSrc[dwStartPos];
            }
            PutMutex();

            return dwD;
        }

        /// <summary>
        /// 设置IO资源的32位数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public void SetIoU32(UInt32 dwStartPos, UInt32 dwIn)
        {
            unsafe { if (mIoSrc == null || (dwStartPos + 4) > mIoLen) return; }

            GetMutex();
            unsafe
            {
                mIoSrc[dwStartPos] = (byte)(dwIn & 0xff);
                mIoSrc[dwStartPos + 1] = (byte)((dwIn & 0xff00) >> 8);
                mIoSrc[dwStartPos + 2] = (byte)((dwIn & 0xff0000) >> 16);
                mIoSrc[dwStartPos + 3] = (byte)((dwIn & 0xff000000) >> 24);
            }
            PutMutex();
        }

        /// <summary>
        /// 取得IO资源的16位数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public UInt16 GetIoU16(UInt32 dwStartPos)
        {
            UInt16 wData = 0;
            unsafe { if (mIoSrc == null || (dwStartPos + 2) > mIoLen) return 0; }

            GetMutex();
            unsafe
            {
                wData = mIoSrc[(int)dwStartPos + 1];
                wData <<= 8;

                wData |= mIoSrc[(int)dwStartPos];
            }
            PutMutex();

            return wData;
        }

        /// <summary>
        /// 设置IO资源的32位数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public void SetIoU16(UInt32 dwStartPos, UInt16 dwIn)
        {
            unsafe { if (mIoSrc == null || (dwStartPos + 2) > mIoLen) return; }

            GetMutex();
            unsafe
            {
                mIoSrc[dwStartPos] = (byte)(dwIn & 0xff);
                mIoSrc[dwStartPos + 1] = (byte)((dwIn & 0xff00) >> 8);
            }
            PutMutex();
        }

        /// <summary>
        /// 取得IO资源的8位数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public byte GetIoU8(UInt32 dwStartPos)
        {
            byte reDat = 0;
            unsafe { if (mIoSrc == null || dwStartPos >= mIoLen) return 0; }

            GetMutex();
            unsafe
            {
                reDat = mIoSrc[dwStartPos];
            }
            PutMutex();

            return reDat;
        }

        /// <summary>
        /// 设置IO资源的32位数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public void SetIoU8(UInt32 dwStartPos, byte byIn)
        {
            unsafe { if (mIoSrc == null || dwStartPos > mIoLen) return; }

            GetMutex();
            unsafe
            {
                mIoSrc[dwStartPos] = byIn;
            }
            PutMutex();
        }

        /// <summary>
        /// 取得IO资源的32位数组数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public int SetIoArrayU32(UInt32 dwStartPos, UInt32[] dwIn, UInt32 dwLen)
        {
            UInt32 dwPos = dwStartPos;
            unsafe { if (mIoSrc == null) return -1; }

            GetMutex();
            unsafe
            {
                UInt32 dwD = 0;
                for (UInt32 i = 0; i < dwLen; i++)
                {
                    dwD = dwIn[i];
                    mIoSrc[dwPos]     = (byte)(dwD & 0xff);
                    mIoSrc[dwPos + 1] = (byte)(dwD >> 8);
                    mIoSrc[dwPos + 2] = (byte)(dwD >> 16);
                    mIoSrc[dwPos + 3] = (byte)(dwD >> 24);

                    dwPos += 4;
                }
            }
            PutMutex();

            return 0;
        }

        /// <summary>
        /// 取得IO资源的32位数组数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public UInt32[] GetIoArrayU32(UInt32 dwStartPos, UInt32 dwLen)
        {
            UInt32 dwPos = dwStartPos;
            UInt32[] dwGet = new UInt32[dwLen];
            unsafe { if (mIoSrc == null) return dwGet; }

            GetMutex();
            unsafe
            {
                UInt32 dwD = 0;

                for (UInt32 i = 0; i < dwLen; i++)
                {
                    dwD  = mIoSrc[dwPos + 3];
                    dwD <<= 8;

                    dwD += mIoSrc[dwPos + 2];
                    dwD <<= 8;

                    dwD += mIoSrc[dwPos + 1];
                    dwD <<= 8;

                    dwD += mIoSrc[dwPos];

                    dwGet[i] = dwD;
                    dwPos += 4;
                }
            }
            PutMutex();

            return dwGet;
        }

        /// <summary>
        /// 取得IO资源的16位数组数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public int SetIoArrayU16(UInt32 dwStartPos, UInt16[] wIn, UInt32 dwLen)
        {
            UInt32 dwPos = dwStartPos;
            unsafe { if (mIoSrc == null) return -1;  }

            GetMutex();
            unsafe
            {
                UInt16 wD = 0;
                for (UInt32 i = 0; i < dwLen; i++)
                {
                    wD = wIn[i];
                    mIoSrc[dwPos] = (byte)(wD & 0xff);
                    mIoSrc[dwPos + 1] = (byte)((wD & 0xff00) >> 8);

                    dwPos += 2;
                }
            }
            PutMutex();

            return 0;
        }

        /// <summary>
        /// 取得IO资源的16位数组数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public UInt16[] GetIoArrayU16(UInt32 dwStartPos, UInt32 dwLen)
        {
            UInt32 dwPos = dwStartPos;
            UInt16[] wGet = new UInt16[dwLen];
            unsafe { if (mIoSrc == null) return wGet; }

            GetMutex();
            unsafe
            {
                UInt16 wD = 0;
                for (UInt32 i = 0; i < dwLen; i++)
                {
                    wD = mIoSrc[(int)dwPos + 1];
                    wD <<= 8;
                    wD |= mIoSrc[(int)dwPos];

                    wGet[i] = wD;
                    dwPos += 2;
                }
            }
            PutMutex();

            return wGet;
        }

        /// <summary>
        /// 取得IO资源的8位数组数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public int SetIoArrayBytes(UInt32 dwStartPos, byte[] byIn, UInt32 dwLen)
        {
            unsafe { if (mIoSrc == null || (dwStartPos + dwLen) > mIoLen) return -1; }

            GetMutex();
            unsafe
            {
                for (int i = 0; i < dwLen; i++) mIoSrc[dwStartPos + i] = byIn[i];
            }
            PutMutex();

            return 0;
        }

        /// <summary>
        /// 取得IO资源的8位数组数据
        /// </summary>
        /// <param name="dwStartPos"></param>
        /// <returns></returns>
        public byte[] GetIoArrayBytes(UInt32 dwStartPos, UInt32 dwLen)
        {
            byte[] byGet = new byte[dwLen];
            unsafe { if (mIoSrc == null || (dwStartPos + dwLen) > mIoLen) return byGet; }

            GetMutex();
            unsafe
            {
                for (int i = 0; i < dwLen; i++) byGet[i] = mIoSrc[dwStartPos + i];
            }
            PutMutex();

            return byGet;
        }

        /// <summary>
        /// 用延时的回调接口
        /// </summary>
        private static void ToCdelayHandler()
        {
            Thread.Sleep(5);
        }

        // end
    }
}