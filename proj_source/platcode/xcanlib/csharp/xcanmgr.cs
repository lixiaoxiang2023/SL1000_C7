/*******************************************************************************
文件名称：   XcanSdk.cs
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   201801021
程序用途：   xcan协议C#主体类,管理类

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
修改时间：
修改人：     
修改内容：  
*******************************************************************************/
using System;
using System.Threading;

namespace XcanSdk
{
    public class XcanMgr : XcanNet
    {
        // 私有参数
        private static Thread mXcanThread;
        private static bool blStackOn = false;
        private static bool blServiceOn = false;

        /// <summary>
        /// 构造
        /// </summary>
        public XcanMgr()
        {
            blStackOn = false;
        }

        /// <summary>
        /// 析构
        /// </summary>
        ~XcanMgr()
        {
            if (mXcanThread != null) mXcanThread.Abort();
        }

        /// <summary>
        /// 取得系统时间
        /// </summary>
        private unsafe static UInt32 GetSysTim()
        {
            return (UInt32)((DateTime.Now.ToUniversalTime().Ticks - 621355968000000000) / 10000);
        }

        /// <summary>
        /// 启动同步服务线程
        /// </summary>
        public static void ServiceUp()
        {
            if (mXcanThread != null) mXcanThread.Abort();

            unsafe
            {
                mXcan.onSetTimPortCallBack(GetSysTim);
            }

            StackUp();
            blServiceOn = true;

            mXcanThread = new Thread(new ThreadStart(RunThread));
            mXcanThread.Start();
        }

        /// <summary>
        /// 关闭协议栈服务
        /// </summary>
        public static void ServiceDown()
        {
            if (mXcanThread == null) return;

            StackDown();
            blServiceOn = false;
            Thread.Sleep(200);
        }

        /// <summary>
        /// 启动协议栈
        /// </summary>
        public static void StackUp()
        {
            blStackOn = true;
        }

        /// <summary>
        /// 关闭协议栈的执行
        /// </summary>
        public static void StackDown()
        {
            blStackOn = false;
        }

        /// <summary>
        /// 服务接口
        /// </summary>
        private static void RunThread()
        {
            int pre = 0;

            while (blServiceOn == true) {
                // 处理输入数据
                if (blStackOn == false) {
                    Thread.Sleep(100);
                    continue;
                }

                XcanIO.BridgeService();
                XcanNet.ProcessReceiveQueue();

                // 服务
                if (pre ++ > 2) {
                    if (blStackOn == true) mXcan.onService();
                    pre = 0;
                }

                Thread.Sleep(5);
            }
        }
    }

    // end
}
