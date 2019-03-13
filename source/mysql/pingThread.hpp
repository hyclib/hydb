
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy hdqs
// author:       lipengxiang
// create at:    2014-02-25 14:27:14
// description:  
//

#ifndef __BP_PINGTHREAD_HPP_20140225142714__
#define __BP_PINGTHREAD_HPP_20140225142714__

#ifdef DB_SUPPORT_MYSQL
#include "mysqlDataBase.h"

namespace hydb
{
    class PingThread : public DB_Thread
    {
        MysqlDataBase*  _pDB;
        u32             _sleepTime;
    public:
        PingThread( MysqlDataBase* pDB )    : _pDB(pDB), _sleepTime(MYSQL_PING_INTERVAL)   {}

        virtual bool    onStart(void* lpParam)   { _sleepTime = MYSQL_PING_INTERVAL; return true; }
        virtual void    onFinish(void* lpParam)   {}
        virtual bool    onInvoke(void* lpParam)
        {
            DB_Thread::Sleep(_sleepTime*1000);

            s64 now = (s64)time(NULL) - _pDB->mPreConnTime;
            if (now < 0)
                now = 0;

            if (now < MYSQL_PING_INTERVAL)
            {
                _sleepTime = MYSQL_PING_INTERVAL-now;
                return false;
            }

            _pDB->mSync.lock();
            if (0 != mysql_ping(&_pDB->mMySql))
            {
                _pDB->Connect(_pDB->mConnectInfo.host, _pDB->mConnectInfo.username, _pDB->mConnectInfo.password, _pDB->mConnectInfo.port, _pDB->mConnectInfo.dbName, _pDB->mConnectInfo.character);
            }
            _pDB->mSync.unlock();

            _sleepTime = MYSQL_PING_INTERVAL;
            return false;
        }
    };
	

} // end namespace hydb

#endif
#endif // end of define __BP_PINGTHREAD_HPP_20140225142714__
