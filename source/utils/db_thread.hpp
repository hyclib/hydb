//
// Copyright(c) 1999-2010 hyrj. All rights reserved.
// hyc libaray
// author:       lipengxiang
// description:  
//

#ifndef __BP_DB_THREAD_HPP_201011081009551__
#define __BP_DB_THREAD_HPP_201011081009551__

#if CN_PLATFORM != CN_PLATFORM_WINDOWS
#include <pthread.h>
#include <signal.h>
#endif


namespace hydb
{
    //----------------------------------------------------------------------------------------------------
    //!	Synchronism object; use to mark a synchronization
    //!  只能用于进程内线程的同步; 因为可能需要在网络中使用, 故不使用私类
    //----------------------------------------------------------------------------------------------------
    class DB_SyncObject
    {
    public:
        DB_SyncObject();
        ~DB_SyncObject();

        /**
         * @brief 使用引用计数的方式进行锁
         *      调用一次lock, 本线程对锁的引用计数+1
         *      调用一次unlock, 本线程对锁的引用计数-1
         *      没有引用计数时, 本线程对锁无锁
         */
        void            lock();
        void            unlock();

        /**
         * @brief 成功锁定时返回true, 否则为false已被其他线程占用
         */
        bool            tryLock();

    protected:
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
        CRITICAL_SECTION    m_csLock;
#else
        pthread_mutex_t     m_hMutex;
#endif
    };

    //--------------------------------
    typedef enum {
        DB_ThreadStatus_Uncreated,
        DB_ThreadStatus_Running,
        DB_ThreadStatus_Dead,
    } DB_ThreadStatus;

    //--------------------------------
    class DB_Thread
    {
    public:
        DB_Thread(void* lpParam = NULL);
        virtual ~DB_Thread();

        /*---------------------------
          对当前线程的操作
        ---------------------------*/
        static  integer CurThreadId();
        static  void    Sleep( u32 milliseconds );

        /*---------------------------
          线程的操作
        ---------------------------*/

        /**
         * @brief wait for thread abort
         */
        void            join();

        /**
         * @brief it would be best not to use this function
         */
        void            abort();

        /**
         * @brief start or resume thread
         */
        bool            start();

        uinteger        getExitCode() const;
        DB_ThreadStatus    getStatus() const;
        integer         getThreadId() const { return m_id; }
        
        /**
         * @brief 安全退出函数, 调用此函数后, 在onInvoke中安全退出.
         */
        virtual void    exit();

        /**
         * @brief After thread start, invoke will run
         *  onStart 函数返回 false时,将不再执行线程,onInvoke及onFinish函数不会被调用.
         *  onInvoke函数将不断地循环执行, 只有当返回true时退出.
         *      onStart -> onInvoke-> ... ->onInvoke -> onFinish.
         */
        virtual bool    onStart(void* lpParam)   { return true; }
        virtual bool    onInvoke(void* lpParam) = 0;
        virtual void    onFinish(void* lpParam)   {}

    public:
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
        HANDLE              m_hThread;
        LPVOID              m_lpParam;
        DB_ThreadStatus        m_status;
        s32                 m_id;
        bool                m_exit;
        volatile integer    m_addRef;
#else
        DB_ThreadStatus    m_status;
        pthread_t       m_hThread;
        void*           m_lpParam;
        u32             m_exitCode;
        long            m_id;
//        ConditionVariable   m_cv;
        DB_SyncObject          m_cvSync;
        bool            m_exit;
        volatile integer    m_addRef;
#endif
    };



} // end namespace hydb

#endif // end of define __BP_DB_THREAD_HPP_201011081009551__
