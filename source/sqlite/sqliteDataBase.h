////////////////////////////////////////////////////////
// Copyright(c) 1986-2100, TT, All Rights Reserved
// Author:  Hu Tianzhi
// Created: 2010-12-22 14:57:55
// Describe:
////////////////////////////////////////////////////////

#ifndef	_Tian_20101222145755_H_
#define	_Tian_20101222145755_H_


#include "../predefine.h"

#ifdef DB_SUPPORT_SQLITE

#include "hy_sqlite3.h"
#include "sqlite/sqlite3.h"


struct sqlite3;
struct sqlite3_stmt;

namespace hydb
{    
    class SqliteDB:public IDataBase
    {
    public:
         SqliteDB(void);
         ~SqliteDB(void);

    public:
        virtual bool        StatusUtf8() const          { return true; }
        virtual s32         StatusPingInterval() const  { return -1; }

        // 返回影响的行数
        virtual IDataSet*   ExecQuery(const tchar* strSQL, bool* pIsSuccess = 0);
        virtual bool        ExecNoneQuery(const tchar* strSQL, u64* pLastInsertId = NULL);
		virtual bool		ExecNoneQuery(const tchar* strSQL, BLOB_ITEM* datas, s32 nCount, u64* pLastInsertId = NULL);

        virtual u64         lastInsertId();

        virtual void        SetBusyTimeout(s32 nMillisecs);
        virtual s32         GetBusyTimeout() const;

        // 开始事务
        virtual TransactionRef  BeginTransaction();

        // 提交事务
        virtual bool        CommitTransaction();

        // 回滚事务
        virtual bool        RollBackTransaction();

        virtual void        Release();

		virtual void		WaitForEnd(void);



    public:
        sqlite3_stmt*   _compile(const tchar* strSql);

    public:
        sqlite3*    m_pDB;
        s32         m_nBusyTimeoutMs;

        void        SetError(const char* strError, bool bDelete = true);
        void        SetError(const wchar_t* strError, bool bDelete = true);
        const tchar* GetLastErrorStr(void) const   { return g_strError; }
        const tchar* GetLastErrorSql(void) const     {return mLastSQL; }
        void        SetLastErrorSQL(const tchar* sql) {
            if (sql)
                db_log(sql);
        //    u32 l = tcslen(sql); if (l > STRING_BUFFER_LENGTH) l = STRING_BUFFER_LENGTH-1; mSync.lock(); memcpy(mLastSQL, sql, sizeof(tchar)*l); mLastSQL[l] = 0; mSync.unlock(); 
        }

        virtual void        setCallback_connectError( ConnectError func )   { mConnectErrorFunc = func; }

        tchar g_strError[STRING_BUFFER_LENGTH];
        tchar mLastSQL[STRING_BUFFER_LENGTH];

        DB_SyncObject  mSync;
        ConnectError    mConnectErrorFunc;

    public:
        tchar   mTmpBuf[GET_STR_VALUE_TEMP_BUF_LENGTH]; // 临时指针用于DataSet GetStringValue 返回.

    };
}

#endif // DB_SUPPORT_SQLITE
#endif // _Tian_20101222145755_H_