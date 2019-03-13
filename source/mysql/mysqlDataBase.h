////////////////////////////////////////////////////////
// Copyright(c) 1986-2100, TT, All Rights Reserved
// 作者:  Hu Tianzhi
// 日期: 2011-8-2 15:42:18
// 描述: 
////////////////////////////////////////////////////////

#ifndef	_Tian_201182154218123_H_
#define	_Tian_201182154218123_H_

#include "../predefine.h"

#ifdef DB_SUPPORT_MYSQL

namespace hydb
{
    class PingThread;

    struct ConnectInfo{
    	ConnectInfo()
    	:port(0)
    	{
            ZeroMemory(host, sizeof(tchar)*STRING_BUFFER_LENGTH);
            ZeroMemory(username, sizeof(tchar)*STRING_BUFFER_LENGTH);
            ZeroMemory(password, sizeof(tchar)*STRING_BUFFER_LENGTH);
            ZeroMemory(dbName, sizeof(tchar)*STRING_BUFFER_LENGTH);
            ZeroMemory(character, sizeof(tchar)*STRING_BUFFER_LENGTH);
    	}

        tchar host[STRING_BUFFER_LENGTH];		// 服务器名称或IP
        tchar username[STRING_BUFFER_LENGTH];	// 用户名
        tchar password[STRING_BUFFER_LENGTH];	// 密码
        s32    port;		// 端口
        tchar dbName[STRING_BUFFER_LENGTH];
        tchar character[STRING_BUFFER_LENGTH];
    };

	class MysqlDataBase:public IDataBase
	{
	public:
        MysqlDataBase();
        virtual ~MysqlDataBase();

        // 释放资源
        virtual void			Release(void)   { delete this; }

		bool	Connect(
			const tchar* host,		// 服务器名称或IP
			const tchar* username,	// 用户名
			const tchar* password,	// 密码
			s32 port,				// 端口
            const tchar* dbName,
            const tchar* character);

        virtual bool        StatusUtf8() const          { return mStatusUtf8; }
        virtual s32         StatusPingInterval() const  { return MYSQL_PING_INTERVAL; }

        // 执行sql
        virtual IDataSet*		ExecQuery(const tchar* strSQL, bool* pIsSuccess);
        virtual bool  			ExecNoneQuery(const tchar* strSQL, u64* pLastInsertId = NULL);
        virtual bool			ExecNoneQuery(const tchar* strSQL, BLOB_ITEM* datas, s32 nCount, u64* pLastInsertId = NULL);

        virtual u64             lastInsertId();

		// 开始事务
        virtual TransactionRef			BeginTransaction(void);

		// 提交事务
		virtual bool			CommitTransaction(void);

		// 回滚事务
		virtual bool			RollBackTransaction(void);


		// 获得或设置超时时间
		virtual void			SetBusyTimeout(s32 nMillisecs);
		virtual s32				GetBusyTimeout(void)const;

		virtual void			WaitForEnd(void);			// mySql忽略


        // 获得最后一个错误说明
        const tchar*         GetLastErrorStr(void) const   { return mLastError; }
        void				 SetLastError(const char* str);
        void				 SetLastError(const wchar_t* str);
        // Last SQL
        const tchar*         GetLastErrorSql(void) const     {return mLastSQL; }
        void                 SetLastErrorSQL(const tchar* sql) { 
            if (sql)
                db_log(sql);
            //u32 l = tcslen(sql); if (l > STRING_BUFFER_LENGTH) l = STRING_BUFFER_LENGTH-1; mSync.lock(); memcpy(mLastSQL, sql, sizeof(tchar)*l); mLastSQL[l] = 0; mSync.unlock();
        }

        virtual void         setCallback_connectError( ConnectError func )   { mConnectErrorFunc = func; }

	protected:
		MYSQL	mMySql;
		s32		mTimeOut;
		s32		mErrorCode;
        tchar     mLastSQL[STRING_BUFFER_LENGTH];
		tchar		mLastError[STRING_BUFFER_LENGTH];
        DB_SyncObject   mSync;
        ConnectError    mConnectErrorFunc;
        ConnectInfo     mConnectInfo;
        s64             mPreConnTime;
        DB_Thread*         mPingThread;
        
        bool    mStatusUtf8;
        s32     mStatusPingInterval;

    public:
        tchar   mTmpBuf[GET_STR_VALUE_TEMP_BUF_LENGTH]; // 临时指针用于DataSet GetStringValue 返回.


        friend class PingThread;
	};
}

#endif // DB_SUPPORT_MYSQL
#endif // _Tian_201182154218123_H_
