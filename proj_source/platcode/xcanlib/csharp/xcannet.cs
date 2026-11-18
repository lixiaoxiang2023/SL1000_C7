/*******************************************************************************
文件名称：   XcanSdk.cs
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   201801021
程序用途：   xcan协议C#主体类

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
修改时间：
修改人：     
修改内容：  
*******************************************************************************/
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Threading;

namespace XcanSdk
{
    public class XcanNet
    {
        // 内核计数器
        private static UInt32 mCoreCnt = 0;
        private static bool mCportInit = false;

        // 通信接口参数
        public UInt32 mdwCommChnID;
        public UInt32 mdwFrameLen;

        // xcan 内核
        public  static DevXcanTdf mXcan;
        public  static List<XcanSuportDev> mListCommNet = new List<XcanSuportDev>();
        private static DevXcanTdf.OnXcanCoreInPortCb mReceiveHandler = null;
        private static DevXcanTdf.OnXcanMutexHandler mGetCoreMutexHandler = null;
        private static DevXcanTdf.OnXcanMutexHandler mPutCoreMutexHandler = null;

        private static Queue<byte[]> queueReceiveList = new Queue<byte[]>();
        private static Queue<UInt32> queueComChnIDList = new Queue<UInt32>();
        private static Queue<Int32> queueReveiveLenList = new Queue<Int32>();

        // 线程锁
        private static Object mMutexNet = new Object();
        private static Mutex mMutexObj = new Mutex();

        // 本地设备ID
        private static UInt16 wNetMyDevID;

        // 支持多个设备类
        public class XcanSuportDev
        {
            // 自己的句柄
            public XcanNet pThis;

            public UInt32 dwCommChnID = 0;
            public XcanSportDevTdf Dev;
        }

        /// <summary>
        /// 构造实体的设备,并加载到设备链表
        /// </summary>
        public XcanNet(UInt32 dwCommChnID, UInt32 dwFrameLen)
        {
            mCoreCnt ++;

            mdwCommChnID = dwCommChnID;
            mdwFrameLen = dwFrameLen;

            XcanSuportDev cDev = new XcanSuportDev();
            cDev.pThis = this;

            cDev.dwCommChnID = dwCommChnID;

            cDev.Dev.dwComChnID = dwCommChnID;
            cDev.Dev.dwFrameDataLen = dwFrameLen;
            cDev.Dev.onAddFilterCallBack = OnAddFilterCb;
            cDev.Dev.onReceiveCallBack = null;
            cDev.Dev.onSendEmptyNoticeCallBack = null;
            cDev.Dev.onSendEmptyNoticeDownCallBack = null;
            cDev.Dev.onSendCallBack = OnSendCb;
            cDev.Dev.onIsSendFullCallBack = OnIsFullCb;
            cDev.Dev.onErr = OnErrCb;

            mListCommNet.Add(cDev);

            if (mCportInit == false)
            {
                // 初始化c接口
                mCportInit = true;
                IntPtr prt = new IntPtr(Marshal.SizeOf(typeof(DevXcanTdf)));
                prt = XcanInit();
                mXcan = (DevXcanTdf)Marshal.PtrToStructure(prt, typeof(DevXcanTdf));
                //Todo 2020.11.25 增加新的内核接口
                mReceiveHandler = mXcan.onGetCoreInPort();

                mGetCoreMutexHandler = GetMutex;
                mPutCoreMutexHandler = PutMutex;
                mXcan.onSetMutex(mGetCoreMutexHandler, mPutCoreMutexHandler);
            }
        }

        /// <summary>
        /// 构造
        /// </summary>
        public XcanNet()
        {
            mCoreCnt ++;
        }

        /// <summary>
        /// 析构
        /// </summary>
        ~XcanNet()
        {
            ;
        }

        /// <summary>
        /// 获取任务锁
        /// </summary>
        public static void GetMutex()
        {
            mMutexObj.WaitOne();
        }

        /// <summary>
        /// 释放线程锁
        /// </summary>
        public static void PutMutex()
        {
            mMutexObj.ReleaseMutex();
        }

        /// <summary>
        /// 取得当前需要使用的通信设备
        /// </summary>
        /// <param name="dwDevID">通信ID</param>
        /// <returns></returns>
        private static XcanSuportDev GetCommDev(UInt32 dwDevID)
        {
            foreach (XcanSuportDev dev in mListCommNet)
            {
                if (dev.dwCommChnID == dwDevID)
                {
                    return dev;
                }
            }

            return null;
        }

        /// <summary>
        /// 设置本机设备ID
        /// </summary>
        /// <param name="wDevID">unsigned int dwDevID 本机设备ID</param>
        public static void SetMyNetDevID(UInt16 wDevID)
        {
            wNetMyDevID = wDevID;
            mXcan.onSetMyDevID(wDevID);
        }

        /// <summary>
        /// 获取本机设备ID
        /// </summary>
        /// <returns>unsigned int dwDevID 本机设备ID</returns>
        public static UInt16 GetMyNetDevID()
        {
            return wNetMyDevID;
        }


        /// <summary>
        /// 过滤器的设置接口
        /// </summary>
        /// <param name="dwDevID">通信设备的ID</param>
        /// <param name="wFilterID">过滤器的ID</param>
        /// <returns>0 成功，-1 失败</returns>
        private static int OnAddFilterCb(UInt32 dwComChnID, UInt16 wFilterID, UInt32 dwFilterLen)
        {
            XcanSuportDev cDev = GetCommDev(dwComChnID);

            if (cDev == null) return -1;
            cDev.pThis.AddFilter(dwComChnID, wFilterID, dwFilterLen);

            return 0;
        }

        /// <summary>
        /// 重新构建通信设备的过滤器设置接口
        /// </summary>
        /// <param name="dwDevID">通信设备的ID</param>
        /// <param name="wFilterID">过滤器的ID</param>
        /// <returns>0 成功，-1 失败</returns>
        public virtual int AddFilter(UInt32 dwDevID, UInt16 wFilterID, UInt32 dwFilterLen)
        {
            return 0;
        }

        /// <summary>
        /// 内核的数据接收接口输出
        /// </summary>
        private static void OnReceiveRegCb(XcanSportDevTdf.OnReceiveInCallBack callback)
        {
            // mReceiveHandler = callback;
        }

        /// <summary>
        /// 输出的数据发送接口
        /// </summary>
        /// <param name="dwComChnID">通信ID</param>
        /// <param name="dwId">帧ID</param>
        /// <param name="pIn">数据</param>
        /// <param name="dwLen">数据长度</param>
        /// <returns></returns>
        private static int OnSendCb(UInt32 dwComChnID, UInt32 dwDport,UInt32 dwId, IntPtr pIn, UInt32 dwLen)
        {
            XcanSuportDev cDev = GetCommDev(dwComChnID);
            if (cDev == null) return -1;

            byte[] get = new byte[dwLen];
            Marshal.Copy(pIn, get, 0, (int)dwLen);

            byte[] id = BitConverter.GetBytes(dwId);
            byte[] dout = new byte[dwLen + 4];

            id.CopyTo(dout, 0);
            get.CopyTo(dout, 4);

            return cDev.pThis.Send(dwDport, dout);
        }

        /// <summary>
        /// 数据发送方法，继承后需要重写方法
        /// </summary>
        /// <param name="dwComChnID">通信ID</param>
        /// <param name="pIn">数据</param>
        /// <returns></returns>
        public virtual int Send(UInt32 dwCommChnID, byte[] pIn)
        {
            return 0;
        }

        /// <summary>
        /// 查询数据发送是否可以进行
        /// </summary>
        private static bool OnIsFullCb(UInt32 dwComChnID, UInt32 dwDport)
        {
            try
            {
                XcanSuportDev cDev = GetCommDev(dwComChnID);
                if (cDev == null) return true;

                return cDev.pThis.IsFull(dwDport);
            }
            catch (Exception)
            {
                return true;
            }
        }

        /// <summary>
        /// 重写查询发送是否可以进行，协议将检查发送是否可以有能力发送数据。
        /// </summary>
        public virtual bool IsFull(UInt32 dwComChnID)
        {
            return false;
        }

        /// <summary>
        /// 数据传输出错
        /// </summary>
        /// <param name="dwRemoteDevID">通信ID</param>
        /// <param name="dwRemoteDevID">远程的设备</param>
        /// <param name="wIoUUID">资源DI</param>
        /// <param name="byFrame">帧类型</param>
        private static void OnErrCb(UInt32 dwComChnID, UInt32 dwDport, UInt32 dwRemoteDevID, UInt16 wIoUUID, byte byFrame)
        {
            XcanSuportDev cDev = GetCommDev(dwComChnID);
            if (cDev == null) return;
            //TODO
            cDev.pThis.Err(dwDport, dwRemoteDevID, wIoUUID, byFrame);
        }

        /// <summary>
        /// 重写错误方法
        /// </summary>
        /// <param name="dwRemoteDevID">通信ID</param>
        /// <param name="dwRemoteDevID">远程的设备</param>
        /// <param name="wIoUUID">资源DI</param>
        /// <param name="byFrame">帧类型</param>
        public virtual void Err(UInt32 dwComChnID, UInt32 dwRemoteDevID, UInt16 wIoUUID, byte byFrame)
        {
            ;
        }

        /// <summary>
        /// 处理网络输入数据,存于列队
        /// </summary>
        protected static void ProcessReceiveQueue()
        {
            UInt32 dwComChnID;
            Int32 dwLen = 0;
            byte[] pbyIn = null;

            if (mGetCoreMutexHandler == null || mPutCoreMutexHandler == null) return;

            lock(mMutexNet)
            {
                if (queueReceiveList.Count <= 0) return;
                pbyIn = queueReceiveList.Dequeue();
                dwComChnID = queueComChnIDList.Dequeue();
                dwLen = queueReveiveLenList.Dequeue();
            }

            byte[] id = new byte[4];
            Array.Copy(pbyIn, id, 4);

            byte[] inDat = new byte[dwLen - 4];
            for (int i = 0; i < dwLen - 4; i++) inDat[i] = pbyIn[i + 4];

            UInt32 dwID = BitConverter.ToUInt32(id, 0);

            IntPtr ptr = Marshal.AllocHGlobal(dwLen - 4);
            Marshal.Copy(inDat, 0, ptr, dwLen - 4);
            mReceiveHandler(dwComChnID, dwID, ptr, (UInt32)(dwLen - 4));
            Marshal.FreeHGlobal(ptr); 
        }

        /// <summary>
        /// 协议栈的数据输入
        /// </summary>
        /// <param name="dwComChnID">通信ID</param>
        /// <param name="pbyIn">输入数据</param>
        /// <param name="dwLen">数据长度</param>
        /// <returns>0 成功， -1 失败</returns>
        public int In(UInt32 dwComChnID, byte[] pbyIn, Int32 dwLen)
        {
            if (dwLen <= 4) return -1;
            if (mReceiveHandler == null) return -1;
            
            lock(mMutexNet)
            {
                queueComChnIDList.Enqueue(dwComChnID);
                queueReceiveList.Enqueue(pbyIn);
                queueReveiveLenList.Enqueue(dwLen);
            }
            
            return 0;
        }

        #region #声明XcanSportDevTdf回调和结构体 

        /// <summary>
        /// 传输设备定义
        /// </summary>
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        public struct XcanSportDevTdf
        {
            /// <summary>
            /// 设备通讯通道ID，设备内唯一
            /// </summary>
            [MarshalAs(UnmanagedType.U4)]
            public UInt32 dwComChnID;

            /// <summary>
            /// 根据协议定义，设备支持的单帧数据长度。
            /// </summary>
            [MarshalAs(UnmanagedType.U4)]
            public UInt32 dwFrameDataLen;

            /// <summary>
            /// 接收回调接口
            /// </summary>
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnReceiveInCallBack( UInt32 dwDevID, UInt32 dwId, IntPtr ptr, Int32 dwLen);
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnReceiveCallBack(OnReceiveInCallBack calback);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnReceiveCallBack onReceiveCallBack;

            /// <summary>
            /// 数据发送接口
            /// </summary>
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate int OnSendCallBack( UInt32 dwDevID, UInt32 dwDport, UInt32 dwId, IntPtr pByIn, UInt32 nLen);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSendCallBack onSendCallBack;

            /// <summary>
            /// 数据发送是否可能，是否设备的发送FIFO空闲
            /// </summary>
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate bool OnIsSendFullCallBack( UInt32 dwDevID, UInt32 dwDport);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnIsSendFullCallBack onIsSendFullCallBack;

            /// <summary>
            /// 数据发送完成后，设备可以主动触发继续发送的接口，用于高速的数据发送，
            /// 如果不注册此接口，数据的发送将使用协议栈进行发送，速度可能会慢一些
            /// </summary>
            /// <summary>
            /// XCAN主动触发发送的接口回调函数
            /// </summary>
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnSendEmptyNoticeInCallBack(UInt32 dwDevID);
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnSendEmptyNoticeCallBack( UInt32 dwDevID, OnSendEmptyNoticeInCallBack calback);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSendEmptyNoticeCallBack onSendEmptyNoticeCallBack;

            /// <summary>
            /// 在协议栈需要数据发送时，使用此接口触发主动触发发送的功能
            /// </summary> 
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate int OnSendEmptyNoticeUpCallBack(UInt32 dwDevID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSendEmptyNoticeUpCallBack onSendEmptyNoticeUpCallBack;

            /// <summary>
            /// 在协议栈无数据发送时，使用此接口关闭印件主动触发发送的功能
            /// </summary>
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate int OnSendEmptyNoticeDownCallBack( UInt32 dwDevID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSendEmptyNoticeDownCallBack onSendEmptyNoticeDownCallBack;

            /// <summary>
            /// 设定硬件的接收规则，不是注册的IO数据，不需要提交到协议栈。
            /// </summary>
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate int OnAddFilterCallBack( UInt32 dwDevID, UInt16 wFilterID, UInt32 dwFilterLen);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnAddFilterCallBack onAddFilterCallBack;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnErrCallBack(UInt32 dwCommID, UInt32 dwDport, UInt32 dwDevID, UInt16 wIoUUID, byte byFrame);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnErrCallBack onErr;
        }
        #endregion

        #region // TODO XcanPortServiceInitTdf 回调函数和结构体定义
        /// <summary>
        /// XCAN通讯接口
        /// </summary>
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        public struct DevXcanTdf
        {
            /// <summary>
            /// xcan 内核接口定义
            /// </summary> 
            /// 
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnErr();
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate UInt32 OnAddRxPort(ref XcanSportDevTdf pstSurportDev, UInt32 dwBelongDev, UInt16 wIoUUID, byte* pbyIoSrc, byte* pbyBuff, UInt32 byMaxLen);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnAddRxPort onAddRxPort;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate UInt32 OnAddTxPort(ref XcanSportDevTdf pstSurportDev, UInt32 dwBelongDev, UInt16 wIoUUID, byte* pbyIoSrc, byte* pbyBuff, UInt32 byMaxLen);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnAddTxPort onAddTxPort;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate UInt32 OnRemovePort(UInt32 dwUUID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnRemovePort onRemovePort;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate int OnRead(UInt32 dwUUID, UInt16 wSrcAdr, UInt32 dwLen, UInt32 dwInTimeOut, bool blWaitAck);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnRead onRead;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate int OnWrite(UInt32 dwUUID, UInt16 wSrcAdr, UInt32 dwLen, byte* pbyDataIn, UInt32 dwInTimeOut, bool blWaitAck);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnWrite onWrite;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate int OnServicePortInit(UInt32 dwUUID, byte* pbyShadow, byte* pbyShadowBuff, byte* pbyIoCache);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnServicePortInit onServicePortInit;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate UInt32 OnTickClock();
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnSetTimPort(OnTickClock pfHandler);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSetTimPort onSetTimPortCallBack;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnService();
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnService onService;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate int OnSynSet(UInt32 dwUUID, bool blSetOn, UInt16 wSyncStart, UInt16 wSyncEnd, UInt16 wIoSyncStart, UInt16 wIoDEnd);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSynSet onSynSet;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate int OnSynSetDown (UInt32 dwUUID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSynSetDown onSynSetDown;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate int OnAddGroup (UInt32 dwUUID1, UInt32 dwUUID2);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnAddGroup onAddGroup;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate bool OnSynOutLink(UInt32 dwUUID); 
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSynOutLink onSynOutLink;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate bool OnSynInLink(UInt32 dwUUID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSynInLink onSynInLink;

            /// <summary>
            /// 确定将数据写入到远程设备，主要将等待影子区的数据写入，影子区的数据写入后，数据被发送
            /// </summary> 
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnDelayHanlder();
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate bool OnMustToRemote(UInt32 dwUUID, UInt32 dwPos, byte byDat, OnDelayHanlder handler);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnMustToRemote onMustToRemote;

            /// <summary>
            /// 数据在接收到后将自动触发此接口
            /// </summary> 
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void OnRegReceiveNoticeHanlder(UInt32 dwUUID);
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate bool OnRegReceiveNotice(UInt32 dwUUID, OnRegReceiveNoticeHanlder handler);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnRegReceiveNotice onRegReceiveNotice;

            /// <summary>
            /// 取消收到数据触发接口
            /// </summary>
            /// <param name="dwUUID"></param>
            /// <returns></returns>
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate bool OnUnRegReceiveNotice();
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnUnRegReceiveNotice onUnRegReceiveNotice;

            /// <summary>
            /// 数据传输统计接口，用于取得数据传输质量
            /// </summary>
            /// <param name="dwUUID"></param>
            /// <returns></returns>
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate int OnGetCommSta(UInt32 dwUUID, UInt16* pwTxSN, UInt16* pwTxID, UInt16* pwRxSN,
                UInt16* pwRxID, UInt32* pwTrsLost);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnGetCommSta onGetCommSta;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate void OnWriteEndHandler (UInt32 dwUUID, UInt32 dwOp);
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate int OnRegReadWriteNotice(UInt32 dwUUID, OnWriteEndHandler hanlder);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnRegReadWriteNotice onRegReadWriteNotice;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate int OnUnRegReadWriteNotice (UInt32 dwUUID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnUnRegReadWriteNotice onUnRegReadWriteNotice;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate bool OnIsRdBusy(UInt32 dwUUID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnIsRdBusy onIsRdBusy;

            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate bool OnIsWrBusy(UInt32 dwUUID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnIsWrBusy onIsWrBusy;

            /*
             * 向内核设置本机的ID，重要
            */
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate void OnSetMyDevID(UInt16 wMyDevID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSetMyDevID onSetMyDevID;

            /*
             * 获取内核的数据输入接口。
            */
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate void OnXcanCoreInPortCb(UInt32 dwCommID, UInt32 dwId, IntPtr pbyIn, UInt32 dwLen);
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate OnXcanCoreInPortCb OnGetCoreInPort();
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnGetCoreInPort onGetCoreInPort;

            /*
             * 向内核增加IO资源，不用绑定通信设备
            */
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate UInt32 OnAddIo (UInt16 wRemoteDevID, UInt32 dwComChnID, UInt16 wIoUUID,
                   byte* pbyIoSrc, byte* pbyBuff, UInt32 dwMaxLen, bool blIsRxIO);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnAddIo onAddIo;

            /*
             * 向内核绑定通信设备
            */
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate int OnAddCommIF (ref XcanSportDevTdf pstSurportDev);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnAddCommIF onAddCommIF;

            /*
             * 启动资源的同步功能
            */
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate int OnPortActive(UInt32 dwUUID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnPortActive onPortActive;

            /*
             * 关闭接口的同步功能，但资源还在内和，只影响本地不启动同步
            */
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate int OnPortDisActive(UInt32 dwUUID);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnPortDisActive onPortDisActive;

            /*
             * 设置锁接口
            */
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate void OnXcanMutexHandler();
            [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public unsafe delegate void OnSetMutex(OnXcanMutexHandler getmutex, OnXcanMutexHandler putmutex);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            public OnSetMutex onSetMutex;
        }

        [DllImport("XcanCore.dll", EntryPoint = "XcanInit", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XcanInit();

        #endregion
    }
}
