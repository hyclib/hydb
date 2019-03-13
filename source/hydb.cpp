//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// vines
// author:       lipengxiang
// description:  
//


#include "../include/hydb.h"

#ifdef DB_SUPPORT_MYSQL
#include "mysql/mysqlDataBase.h"
#endif

#ifdef DB_SUPPORT_SQLITE
#include "sqlite/sqliteDataBase.h"
#endif

#include "predefine.h"
s32 MYSQL_PING_INTERVAL = 10;

namespace hydb
{
    HYDB_API IDataBase* ConnectDB(
        DataBaseType dbType,    // 数据库类型
        const tchar* host,		// 服务器名称或IP
        s32 port,	 			// 端口
        const tchar* username,	// 用户名
        const tchar* password,	// 密码
        const tchar* dbName,   	// 数据库名称
		const tchar* character,/* = 0*/
        s32          pingInterval/* = 10*/
        )
    {
        MYSQL_PING_INTERVAL = pingInterval;

        switch (dbType)
        {
#ifdef DB_SUPPORT_MYSQL
        case DataBaseType_Mysql:
            {
                MysqlDataBase* pDb = new MysqlDataBase();
                if(pDb->Connect(host, username, password, port, dbName, character))
                {
                    return pDb;
                }
                pDb->Release();
                return NULL;
            }
            break;
#endif
                
#ifdef DB_SUPPORT_SQLITE
        case DataBaseType_Sqlite:
            {
                SqliteDB* pDB = new SqliteDB();
#if defined(UNICODE) || defined(_UNICODE)
                char str[STRING_BUFFER_LENGTH] = {0};
                uinteger l = STRING_BUFFER_LENGTH;
                if (!utils_string_Wide2narrow(dbName, wcslen(dbName), str, l))
                    return NULL;
                const char* pDBName = str;
#else
                const char* pDBName = dbName;
#endif
                s32 nRet = sqlite3_open(pDBName, &pDB->m_pDB);

                if (nRet != SQLITE_OK)
                {
                    pDB->SetError(sqlite3_errmsg(pDB->m_pDB));
                    return NULL;
                }
                pDB->SetBusyTimeout(6000); // 6s

                return pDB;
            }
            break;
#endif
        default:
            return NULL;
        }
    }


    //-----------------------------------------------
    Transaction::Transaction(IDataBase* pDb)
        : _pDB(pDb)
    {

    }

    //-----------------------------------------------
    Transaction::~Transaction()
    {
        if (_pDB)
        {
            _pDB->RollBackTransaction();
            _pDB = NULL;
        }
    }

    //-----------------------------------------------
    bool Transaction::Commit()
    {
        bool r = false;
        if (_pDB)
        {
            r = _pDB->CommitTransaction();
            _pDB = NULL;
        }
        return r;
    }

} // end namespace hydb

