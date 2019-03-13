//
// Copyright(c) 1999-2010 hyrj. All rights reserved.
// hyc libaray
// author:       lipengxiang
// description:  
//

#include "../predefine.h"
#include "db_thread.hpp"


#if CN_PLATFORM != CN_PLATFORM_WINDOWS


#include <pthread.h>

#   include <unistd.h>
#   include <errno.h>
#   include <sys/types.h>
#   include <sys/socket.h>
//#   include <sys/sockio.h>
#   include <sys/ioctl.h>
#   include <netinet/tcp.h>
#   include <netdb.h>
#   include <net/if.h>
#   include <arpa/inet.h>
#   include <fcntl.h>

#if CN_PLATFORM == CN_PLATFORM_ANDROID
#   include <linux/sysctl.h>
#else
#   include <sys/sysctl.h>
#endif

namespace hydb
{
    int g_retval = 0;

    static void* CallbackHandler(void* lpThreadParameter)
    {
        //            sigset_t mask;
        //            sigfillset(&mask);
        //            pthread_sigmask(SIG_BLOCK, &mask, NULL);
        //
        //            // 设置线程退出信号.
        //            struct sigaction actQuit;
        //            ZeroMemory(&actQuit, sizeof(actQuit));
        //            sigemptyset(&actQuit.sa_mask);
        //            actQuit.sa_flags = 0;
        //            actQuit.sa_handler = thread_handle_quit;
        //            sigaction(SIGALRM, &actQuit, NULL);

        DB_Thread* thread = static_cast<DB_Thread*>(lpThreadParameter);
        assert(thread);

        if ( thread->onStart(thread->m_lpParam) )
        {
            while (true) {
                if (thread->m_exit || thread->onInvoke(thread->m_lpParam))
                    break;
            }
            thread->onFinish(thread->m_lpParam);
        }

        thread->m_hThread = 0;
        thread->m_status = DB_ThreadStatus_Dead;
        thread->m_id = 0;

        thread->m_exit = false;

        return &g_retval;
    }

    //----------------------------------------------------------------------------------------------------
    //*	Thread
    //----------------------------------------------------------------------------------------------------
    
    //-----------------------------------------------
    DB_Thread::DB_Thread( void* lpParam )
    {        
        m_status = DB_ThreadStatus_Uncreated;
        m_exitCode = 0;
        m_id = 0;
        m_hThread = 0;
        m_lpParam = 0;
        m_exit = false;
        m_addRef = 0;

        m_lpParam = lpParam;
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
        if ( m_hThread && DB_ThreadStatus_Dead != getStatus() && DB_ThreadStatus_Uncreated != getStatus() )
        {
            //pthread_kill( m_hThread, SIGQUIT );
            
            if (CurThreadId() != this->getThreadId())
            {
                //pthread_cancel( m_hThread );
                //pthread_join( m_hThread, NULL );
                if (pthread_kill(m_hThread, 0) != ESRCH)
                {
                    pthread_kill(m_hThread, SIGALRM);
                    pthread_join(m_hThread, NULL);
                    onFinish(m_lpParam);
                }
            }
            else
            {
                onFinish(m_lpParam);
                m_hThread = 0;
                m_status = DB_ThreadStatus_Dead;
                m_id = 0;
                pthread_exit(&g_retval);
                return;
            }
            
            m_hThread = 0;
            m_status = DB_ThreadStatus_Dead;
            m_id = 0;
        }
    }
    
    //-----------------------------------------------
    bool DB_Thread::start()
    {        
        if ( getThreadId() == CurThreadId() )       return true;
        if ( DB_ThreadStatus_Dead == getStatus() )     return false;
        
        if ( DB_ThreadStatus_Uncreated == getStatus() )
        {
            m_status = DB_ThreadStatus_Running;
            if ( 0 != pthread_create(&m_hThread, NULL, CallbackHandler, this) )
            {
                assert(0); // create thread failure
                m_status = DB_ThreadStatus_Uncreated;
                return false;
            }
            m_id = (long)m_hThread; // gettid();
            return true;
        } // if
        
        return true;
    }

    
    //-----------------------------------------------
    uinteger DB_Thread::getExitCode() const
    {
        return m_exitCode;
    }

    //-----------------------------------------------
    void DB_Thread::join()
    {
        if (m_hThread)
        {
            void* ret;
            pthread_join( m_hThread, &ret );
        }
    }

    //-----------------------------------------------
    void DB_Thread::Sleep( u32 milliseconds )
    {
        /*struct timespec timeOut, remains;
        
        timeOut.tv_sec = milliseconds / 1000;
        timeOut.tv_nsec = (milliseconds % 1000) * 1000000;
        
        nanosleep( &timeOut, &remains );*/
        
        struct timeval timeout;
        
        timeout.tv_sec = milliseconds/1000;
        timeout.tv_usec = milliseconds%1000*1000;
        
        ::select( 0, NULL, NULL, NULL, & timeout );
    }
    
    //-----------------------------------------------
    void DB_Thread::exit()
    {
        m_exit = true;
    }
    
    //-----------------------------------------------
    //void Thread::CurThreadAbort( u32 exitCode/* = 0*/ )
    //{
    //    pthread_exit( (void*)g_retval );
    //}
    
    //-----------------------------------------------
    integer DB_Thread::CurThreadId()
    {
        return (integer)::pthread_self(); // gettid();
    }

    DB_SyncObject::DB_SyncObject()
    {
        pthread_mutexattr_t  attr;
        pthread_mutexattr_init( &attr );
        pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );

        pthread_mutex_init( &m_hMutex, &attr );

        pthread_mutexattr_destroy( &attr );
    }

    //-----------------------------------------------
    DB_SyncObject::~DB_SyncObject()
    {
        //forceUnlock();
        pthread_mutex_destroy( &m_hMutex );
    }

    //-----------------------------------------------
    void DB_SyncObject::lock()
    {
        pthread_mutex_lock( &m_hMutex );
    }

    //-----------------------------------------------
    void DB_SyncObject::unlock()
    {
        pthread_mutex_unlock( &m_hMutex );
    }

    //-----------------------------------------------
    bool DB_SyncObject::tryLock()
    {
        return 0 == pthread_mutex_trylock( &m_hMutex );
    }

} // end namespace hydb

#endif // CN_PLATFORM != CN_PLATFORM_WINDOWS
