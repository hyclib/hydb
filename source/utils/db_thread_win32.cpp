//
// Copyright(c) 1999-2010 hyrj. All rights reserved.
// hyc libaray
// author:       lipengxiang
// description:  
//

#include "../predefine.h"
#include "db_thread.hpp"

#if CN_PLATFORM == CN_PLATFORM_WINDOWS


namespace hydb
{
    static DWORD WINAPI CallbackHandler(LPVOID lpThreadParameter)
    {
        DB_Thread* thread = static_cast<DB_Thread*>(lpThreadParameter);
        assert(thread);

        if (thread->onStart(thread->m_lpParam))
        {
            while (true) {
                if (thread->m_exit || thread->onInvoke(thread->m_lpParam))
                    break;
            }
            thread->onFinish(thread->m_lpParam);
        }
        thread->m_exit = false;

        if (thread->m_hThread)
        {
            ::CloseHandle(thread->m_hThread);
            thread->m_hThread = 0;
            thread->m_status  = DB_ThreadStatus_Dead;
            thread->m_id = 0;
        }

        return 0;
    }

    //----------------------------------------------------------------------------------------------------
    //*	Thread
    //----------------------------------------------------------------------------------------------------

    //-----------------------------------------------
    DB_Thread::DB_Thread( void* lpParam )
        : m_exit(false), m_addRef(0)
    {
        // 回调及参数保存
        m_lpParam = lpParam;

        DWORD id;
        // 创建线程
        m_hThread = ::CreateThread(NULL, 0, CallbackHandler, this, CREATE_SUSPENDED, &id);
        //         if (NULL == m_hThread)
        //             UTILSLOG(_T("Thread_windows::Thread_windows"), _T("failure ::CreateThread in Thread_windows::Thread_windows"));

        m_status = (NULL==m_hThread) ? DB_ThreadStatus_Dead : DB_ThreadStatus_Uncreated;
        m_id = (s32)id;
    }

    DB_Thread::~DB_Thread()
    {
        abort();
    }

    //-----------------------------------------------
    DB_ThreadStatus DB_Thread::getStatus() const
    {
        return m_status;
    }

    //-----------------------------------------------
    void DB_Thread::abort()
    {
        if (m_hThread)
        {
            if (CurThreadId() == getThreadId())
            {
                ::ExitThread(0);
                ::CloseHandle(m_hThread);
            }
            else
            {
                ::TerminateThread(m_hThread, 0);
                ::CloseHandle(m_hThread);
            }
            
            m_status = DB_ThreadStatus_Dead;
            m_hThread = NULL;
            m_id = 0;
        }
    }

    //-----------------------------------------------
    bool DB_Thread::start()
    {
        if (getThreadId() == CurThreadId())
            return true;

        if (m_hThread)
        {
            {
                if (0xFFFFFFFF == ::ResumeThread(m_hThread))
                {
                    //                 UTILSLOG(_T("Thread_windows::start"), _T("failure ::ResumeThread in Thread_windows::start"));
                    m_status = DB_ThreadStatus_Dead;
                    return false;
                }
            } // if..else

            m_status = DB_ThreadStatus_Running;
            return true;
        } // if

        //         UTILSLOG(_T("Thread_windows::start"), _T("thread not be created; failure ::ResumeThread in Thread_windows::start"));
        m_status = DB_ThreadStatus_Dead;
        return false;
    }


    //-----------------------------------------------
    void DB_Thread::join()
    {
        if (m_hThread)
        {
            ::WaitForSingleObject( m_hThread, INFINITE );
        }
    }

    //-----------------------------------------------
    uinteger DB_Thread::getExitCode() const
    {
        DWORD exitCode;
        BOOL r = ::GetExitCodeThread( m_hThread, &exitCode );
        if (0 == r || STILL_ACTIVE == exitCode)
            //UTILSLOG(_T("Thread_windows::getExitCode"), _T("failure ::GetExitCodeThread in Thread_windows::getExitCode maybe thread is runing"));
            return 0;

        return exitCode;
    }

    //-----------------------------------------------
    integer DB_Thread::CurThreadId()
    {
        return ::GetCurrentThreadId();
    }

    //-----------------------------------------------
    void DB_Thread::Sleep( u32 milliseconds )
    {
        ::Sleep( milliseconds );
    }

    //-----------------------------------------------
    void DB_Thread::exit()
    {
        m_exit = true;
    }

    DB_SyncObject::DB_SyncObject()
    {
        ::InitializeCriticalSection(&m_csLock);
    }

    //-----------------------------------------------
    DB_SyncObject::~DB_SyncObject()
    {
        //debug_print("[so %d]", (u32)this);

        if ((s32)m_csLock.OwningThread != 0)
        {
            db_log(_T("~DB_SyncObject()"));
            throw ExceptionType_Forbid;
        }
        ::DeleteCriticalSection(&m_csLock);
    }

    //-----------------------------------------------
    void DB_SyncObject::lock()
    {
        ::EnterCriticalSection(&m_csLock);
    }

    //-----------------------------------------------
    void DB_SyncObject::unlock()
    {
        if ((s32)m_csLock.OwningThread != DB_Thread::CurThreadId())
        {
            db_log(_T("DB_SyncObject::unlock()"));
            throw ExceptionType_Forbid;
        }

        ::LeaveCriticalSection(&m_csLock);
    }

    //-----------------------------------------------
    bool DB_SyncObject::tryLock()
    {
        return 0 != ::TryEnterCriticalSection(&m_csLock);
    }



} // end namespace hydb

#endif // CN_PLATFORM == CN_PLATFORM_WINDOWS