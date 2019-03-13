
#include "../predefine.h"
#include "mysqlDataBase.h"
#include "mysqlDataSet.h"
#include "pingThread.hpp"

#ifdef DB_SUPPORT_MYSQL


namespace hydb
{
    //----------------------------------------
    MysqlDataBase::MysqlDataBase()
        : mTimeOut(0)
    	, mErrorCode(0)
        , mConnectErrorFunc(NULL)
    	, mConnectInfo()
        , mPreConnTime(0)
    	, mPingThread(NULL)
    {
        mysql_init(&mMySql);
        mPingThread = new PingThread(this);
    }


    //----------------------------------------
    MysqlDataBase::~MysqlDataBase()
    {
        if (mPingThread)
        {
            mPingThread->exit();
            mPingThread->abort();
            delete mPingThread;
            mPingThread = NULL;
        }
        mysql_close(&mMySql);
    }

    //----------------------------------------
    bool MysqlDataBase::Connect( const tchar* host, const tchar* username, const tchar* password, s32 port, const tchar* dbName, const tchar* character)
    {
        mSync.lock();

        if (tcscmp(character, _T("utf8")) == 0 
            || tcscmp(character, _T("UTF8")) == 0)
        {
            mStatusUtf8 = true;
        }
        else
        {
            mStatusUtf8 = false;
        }

        tcscpy(mConnectInfo.character, character);
        tcscpy(mConnectInfo.dbName, dbName);
        tcscpy(mConnectInfo.host, host);
        tcscpy(mConnectInfo.password, password);
        mConnectInfo.port = port;
        tcscpy(mConnectInfo.username,username);

        // 设置断线自动重连.
        my_bool reconnect = 1;

#ifdef _DEBUG
        if (
#endif // _DEBUG
            mysql_options(&mMySql, MYSQL_OPT_RECONNECT, (char*)&reconnect)
#ifdef _DEBUG
            )
            printf("ERROR: MYSQL_OPT_RECONNECT\r\n")
#endif // _DEBUG
            ;

#ifdef _DEBUG
        if (
#endif // _DEBUG
            mysql_options(&mMySql,MYSQL_INIT_COMMAND,"set interactive_timeout = 24*3600")
#ifdef _DEBUG
            )
            printf("ERROR: set interactive_timeout 2\r\n")
#endif // _DEBUG
            ;

#ifdef _DEBUG
        if (
#endif // _DEBUG
            mysql_options(&mMySql,MYSQL_INIT_COMMAND,"set wait_timeout = 24*3600")
#ifdef _DEBUG
            )
            printf("ERROR: set wait_timeout 2\r\n")
#endif // _DEBUG
            ;


        // connect
#if defined(UNICODE) || defined(_UNICODE)

        char cHost[STRING_BUFFER_LENGTH] = {0};
        char cUsername[STRING_BUFFER_LENGTH] = {0};
        char cPassword[STRING_BUFFER_LENGTH] = {0};
        char cDbName[STRING_BUFFER_LENGTH] = {0};

        uinteger l = STRING_BUFFER_LENGTH;
        utils_string_Wide2narrow(host, wcslen(host), cHost, l);
        l = STRING_BUFFER_LENGTH;
        utils_string_Wide2narrow(username, wcslen(username), cUsername, l);
        l = STRING_BUFFER_LENGTH;
        utils_string_Wide2narrow(password, wcslen(password), cPassword, l);
        l = STRING_BUFFER_LENGTH;
        utils_string_Wide2narrow(dbName, wcslen(dbName), cDbName, l);

        if(!mysql_real_connect(&mMySql, cHost, cUsername,cPassword,cDbName, (u32)port, NULL, CLIENT_MULTI_STATEMENTS))
#else
        if(!mysql_real_connect(&mMySql, host, username,password,dbName, (u32)port, NULL, CLIENT_MULTI_STATEMENTS))
#endif
        {
            mPreConnTime = 0;

            MysqlDataBase::SetLastError(mysql_error(&mMySql));
            mSync.unlock();
            return false;
        }

        // 如果mysql版本是5.1.6需要在mysql_real_connect后再调用一次MYSQL_OPT_RECONNECT
        unsigned long ver = mysql_get_server_version(&mMySql);
        if (5*10000 + 1*100 + 6 > ver)
        {
#ifdef _DEBUG
            if (
#endif // _DEBUG
                mysql_options(&mMySql, MYSQL_OPT_RECONNECT, (char*)&reconnect)
#ifdef _DEBUG
                )
                printf("ERROR: MYSQL_OPT_RECONNECT 2\r\n")
#endif // _DEBUG
                ;
        }


        // 字符集.
        if(NULL != character)
        {
#if defined(UNICODE) || defined(_UNICODE)
            char cCharacter[STRING_BUFFER_LENGTH] = {0};
            uinteger l = STRING_BUFFER_LENGTH;
            utils_string_Wide2narrow(character, wcslen(character), cCharacter, l);
            mysql_set_character_set(&mMySql, cCharacter);
#else
            mysql_set_character_set(&mMySql, character);
#endif
        }

        mPreConnTime = (s64)time(NULL);
        mSync.unlock();

        if (mPingThread->getStatus() != DB_ThreadStatus_Running)
            mPingThread->start();

        return true;
    }

    //----------------------------------------
    bool MysqlDataBase::ExecNoneQuery( const tchar* strSQL, u64* pLastInsertId )
    {
#if defined(UNICODE) || defined(_UNICODE)
        char cStrSql[STRING_BUFFER_LENGTH] = {0};
        uinteger l = STRING_BUFFER_LENGTH;
        if (!utils_string_Wide2narrow(strSQL, wcslen(strSQL), cStrSql, l))
            return false;
        
        const char* pStrSql = cStrSql;
#else
        const char* pStrSql = strSQL;
#endif

        mSync.lock();

        s32 r = mysql_query(&mMySql, pStrSql);
        if (0 != r)
        {
            u32 err = mysql_errno(&mMySql);
            if(CR_SERVER_GONE_ERROR == err || CR_SERVER_LOST == err)
            {
#ifdef _DEBUG
                s32 e = mysql_ping(&mMySql);
                printf("\r\nreconnect %d\r\n", e);
#endif // _DEBUG

                if (0 != mysql_ping(&mMySql))
                {
                    if (mConnectErrorFunc)  mConnectErrorFunc();
                    mPreConnTime = 0;
                    mSync.unlock();
                    return false;
                }
                
                if (0 != mysql_query(&mMySql, pStrSql))
                {
                    MysqlDataBase::SetLastErrorSQL(strSQL);
                    MysqlDataBase::SetLastError(mysql_error(&mMySql));

                    mPreConnTime = 0;
                    mSync.unlock();
                    return false;
                }
            }
            else
            {
                MysqlDataBase::SetLastErrorSQL(strSQL);
                MysqlDataBase::SetLastError(mysql_error(&mMySql));

                mPreConnTime = 0;
                mSync.unlock();
                return false;
            }
        }

        if (pLastInsertId)  *pLastInsertId = lastInsertId();
        mPreConnTime = (s64)time(NULL);
        mSync.unlock();
        return true;
    }

    //----------------------------------------
    IDataSet* MysqlDataBase::ExecQuery( const tchar* strSQL, bool* pIsSuccess )
    {
#if defined(UNICODE) || defined(_UNICODE)
        char cStrSql[STRING_BUFFER_LENGTH] = {0};
        uinteger l = STRING_BUFFER_LENGTH;
        if (!utils_string_Wide2narrow(strSQL, wcslen(strSQL), cStrSql, l))
            return NULL;

        const char* pStrSql = cStrSql;
#else
        const char* pStrSql = strSQL;
#endif

        mSync.lock();

        s32 r = mysql_query(&mMySql, pStrSql);
        if (0 != r)
        {
            u32 err = mysql_errno(&mMySql);
            if(CR_SERVER_GONE_ERROR == err || CR_SERVER_LOST == err)
            {
#ifdef _DEBUG
                s32 e = mysql_ping(&mMySql);
                printf("\r\nreconnect %d\r\n", e);
#endif // _DEBUG

                if (0 != mysql_ping(&mMySql))
                {
                    if (mConnectErrorFunc)  mConnectErrorFunc();
                    mSync.unlock();
                    if (pIsSuccess) *pIsSuccess = false;
                    mPreConnTime = 0;
                    return NULL;
                }
                
                if (0 != mysql_query(&mMySql, pStrSql))
                {
                    MysqlDataBase::SetLastErrorSQL(strSQL);
                    MysqlDataBase::SetLastError(mysql_error(&mMySql));
                    mSync.unlock();
                    if (pIsSuccess) *pIsSuccess = false;
                    mPreConnTime = 0;
                    return NULL;
                }
            }
            else
            {
                MysqlDataBase::SetLastErrorSQL(strSQL);
                MysqlDataBase::SetLastError(mysql_error(&mMySql));

                mSync.unlock();
                if (pIsSuccess) *pIsSuccess = false;
                mPreConnTime = 0;
                return NULL;
            }
        }


        MYSQL_RES* pRes = NULL;
        do 
        {
            pRes =  mysql_store_result(&mMySql);
            if (NULL != pRes)
            {
                break;
            }
        } while (!mysql_next_result(&mMySql));

        if (NULL == pRes)
        {
            //MysqlDataBase::SetLastErrorSQL(strSQL);
            //MysqlDataBase::SetLastError(mysql_error(&mMySql));

            if (pIsSuccess) *pIsSuccess = true;
            mPreConnTime = 0;
            mSync.unlock();
            return NULL;
        }

        mPreConnTime = (s64)time(NULL);
        if (pIsSuccess) *pIsSuccess = true;
        MysqlDataSet* pSet = new MysqlDataSet(this, mSync);
        pSet->_Init(pRes);
        if (!pSet->Read())
        {
            pSet->Release();
            //mSync.unlock();
            return NULL;
        }

        return pSet;
    }

    //----------------------------------------
    bool MysqlDataBase::ExecNoneQuery( const tchar* strSQL, BLOB_ITEM* datas, s32 nCount, u64* pLastInsertId )
    {
        MYSQL_BIND* bind = new MYSQL_BIND[nCount];
        for(s32 i = 0; i < nCount; i++)
        {
            memset((void*)&bind[i], 0, sizeof(MYSQL_BIND));
            bind[i].buffer_type = MYSQL_TYPE_BLOB;
            bind[i].buffer_length = datas[i].dataLen;
            bind[i].buffer = datas[i].pData;
        }

        MYSQL_STMT* stmt;
        mSync.lock();

        stmt = mysql_stmt_init(&mMySql);
        if (NULL == stmt)
        {
            MysqlDataBase::SetLastErrorSQL(strSQL);
            MysqlDataBase::SetLastError(mysql_error(&mMySql));
            mPreConnTime = 0;
            mSync.unlock();
            return false;
        }

#if defined(UNICODE) || defined(_UNICODE)
        char cStrSql[STRING_BUFFER_LENGTH] = {0};
        uinteger l = STRING_BUFFER_LENGTH;
        if (!utils_string_Wide2narrow(strSQL, wcslen(strSQL), cStrSql, l))
            return false;

        const char* pStrSql = cStrSql;
#else
        const char* pStrSql = strSQL;
#endif

        if (mysql_stmt_prepare( stmt, pStrSql, (unsigned long)strlen(pStrSql)))
        {
            u32 err = mysql_errno(&mMySql);
            if(CR_SERVER_GONE_ERROR == err || CR_SERVER_LOST == err)
            {
#ifdef _DEBUG
                s32 e = mysql_ping(&mMySql);
                printf("\r\nreconnect %d\r\n", e);
#endif // _DEBUG

                if (0 != mysql_ping(&mMySql))
                {
                    if (mConnectErrorFunc)  mConnectErrorFunc();
                    mPreConnTime = 0;
                    mSync.unlock();
                    return false;
                }
                
                if (0 != mysql_stmt_prepare( stmt, pStrSql, (unsigned long)strlen(pStrSql)))
                {
                    MysqlDataBase::SetLastErrorSQL(strSQL);
                    MysqlDataBase::SetLastError(mysql_error(&mMySql));
                    mPreConnTime = 0;
                    mSync.unlock();
                    return false;
                }
            }
            else
            {
                MysqlDataBase::SetLastErrorSQL(strSQL);
                MysqlDataBase::SetLastError(mysql_error(&mMySql));
                mPreConnTime = 0;
                mSync.unlock();
                return false;
            }
        }

        if (mysql_stmt_bind_param(stmt, bind))
        {
            MysqlDataBase::SetLastErrorSQL(strSQL);
            MysqlDataBase::SetLastError(mysql_error(&mMySql));
            mSync.unlock();
            return false;
        }

        if(mysql_stmt_execute(stmt))
        {
            MysqlDataBase::SetLastErrorSQL(strSQL);
            MysqlDataBase::SetLastError(mysql_error(&mMySql));
            mSync.unlock();
            return false;
        }

        mPreConnTime = (s64)time(NULL);
        if (pLastInsertId)  *pLastInsertId = lastInsertId();
        mSync.unlock();
        return true;
    }

    //----------------------------------------
    u64 MysqlDataBase::lastInsertId()
    {
        //mSync.lock();
        return mysql_insert_id(&mMySql);
        //mSync.unlock();
        //return id;
    }

    //----------------------------------------
    TransactionRef MysqlDataBase::BeginTransaction(void)
    {
        mSync.lock();
        if(!mysql_real_query(&mMySql, "START TRANSACTION" ,
            (unsigned long)strlen("START TRANSACTION") ))
        {
            mPreConnTime = (s64)time(NULL);
            Transaction* t = new Transaction(this);
            return TransactionRef(t);
        }
        else
        {
            u32 err = mysql_errno(&mMySql);
            if(CR_SERVER_GONE_ERROR == err || CR_SERVER_LOST == err)
            {
#ifdef _DEBUG
                s32 e = mysql_ping(&mMySql);
                printf("\r\nreconnect %d\r\n", e);
#endif // _DEBUG

                if (0 != mysql_ping(&mMySql))
                {
                    if (mConnectErrorFunc)  mConnectErrorFunc();
                    mPreConnTime = 0;
                    mSync.unlock();
                    return TransactionRef((Transaction*)NULL);
                }
                
                if (0 == mysql_real_query(&mMySql, "START TRANSACTION" ,
                    (unsigned long)strlen("START TRANSACTION") ))
                {
                    mPreConnTime = (s64)time(NULL);
                    Transaction* t = new Transaction(this);
                    return TransactionRef(t);
                }
            }

            mPreConnTime = 0;
            MysqlDataBase::SetLastErrorSQL(_T("BeginTransaction"));
            MysqlDataBase::SetLastError(mysql_error(&mMySql));
            mSync.unlock();
            return TransactionRef((Transaction*)NULL);
        }
    }

    //----------------------------------------
    bool MysqlDataBase::CommitTransaction( void )
    {
        if(!mysql_real_query(&mMySql, "COMMIT",
            (unsigned long)strlen("COMMIT") ) )
        {
            mPreConnTime = (s64)time(NULL);
            mSync.unlock();
            return true;
        }
        else
        {
            u32 err = mysql_errno(&mMySql);
            if(CR_SERVER_GONE_ERROR == err || CR_SERVER_LOST == err)
            {
#ifdef _DEBUG
                s32 e = mysql_ping(&mMySql);
                printf("\r\nreconnect %d\r\n", e);
#endif // _DEBUG

                if (0 != mysql_ping(&mMySql))
                {
                    if (mConnectErrorFunc)  mConnectErrorFunc();
                    mPreConnTime = 0;
                    mSync.unlock();
                    return false;
                }
                
                if (0 == mysql_real_query(&mMySql, "COMMIT",
                    (unsigned long)strlen("COMMIT") ))
                {
                    MysqlDataBase::SetLastErrorSQL(_T("CommitTransaction"));
                    MysqlDataBase::SetLastError(mysql_error(&mMySql));
                    mPreConnTime = 0;
                    mSync.unlock();
                    return false;
                }
            }

            MysqlDataBase::SetLastErrorSQL(_T("CommitTransaction"));
            MysqlDataBase::SetLastError(mysql_error(&mMySql));
            mPreConnTime = 0;
            mSync.unlock();
            return false;
        }
    }

    //----------------------------------------
    bool MysqlDataBase::RollBackTransaction( void )
    {
        if(!mysql_real_query(&mMySql, "ROLLBACK",
            (unsigned long)strlen("ROLLBACK") ) )
        {
            mPreConnTime = (s64)time(NULL);
            mSync.unlock();
            return true;
        }
        else
        {
            u32 err = mysql_errno(&mMySql);
            if(CR_SERVER_GONE_ERROR == err || CR_SERVER_LOST == err)
            {
#ifdef _DEBUG
                s32 e = mysql_ping(&mMySql);
                printf("\r\nreconnect %d\r\n", e);
#endif // _DEBUG

                if (0 != mysql_ping(&mMySql))
                {
                    if (mConnectErrorFunc)  mConnectErrorFunc();
                    mPreConnTime = 0;
                    mSync.unlock();
                    return false;
                }
                
                if (0 == mysql_real_query(&mMySql, "ROLLBACK",
                    (unsigned long)strlen("ROLLBACK") ))
                {
                    mPreConnTime = (s64)time(NULL);
                    mSync.unlock();
                    return true;
                }
            }

            //执行查询失败
            MysqlDataBase::SetLastErrorSQL(_T("RollBackTransaction"));
            MysqlDataBase::SetLastError(mysql_error(&mMySql));
            mPreConnTime = 0;
            mSync.unlock();
            return false;
        }
    }

    //----------------------------------------
    void MysqlDataBase::SetLastError( const char* str )
    {
        if (str == NULL)
            return;

        db_log(str);

//         u32 l = strlen(str); if (l > STRING_BUFFER_LENGTH) l = STRING_BUFFER_LENGTH-1; 
//         mSync.lock();
// #if defined(UNICODE) || defined(_UNICODE)
//         tchar sql[STRING_BUFFER_LENGTH] = {0};
//         uinteger ll = STRING_BUFFER_LENGTH-1;
//         utils_string_Narrow2wide(str, strlen(str), sql, ll);
//         trace(_T("db err: %s"), sql);
// #else
//         trace(_T("db err: %s"), str);
//         memcpy(mLastError, str, l);
//         mLastError[l] = 0;
//#endif
//         mSync.unlock();
    }

    //-----------------------------------------------
    void MysqlDataBase::SetLastError( const wchar_t* str )
    {
        if (str == NULL)
            return;

        db_log(str);

//        mSync.lock();
//#if defined(UNICODE) || defined(_UNICODE)
//         u32 l = wcslen(str); if (l > STRING_BUFFER_LENGTH) l = STRING_BUFFER_LENGTH-1;
//         wmemcpy(mLastError, str, l);
//         mLastError[l] = 0;
//         trace(_T("db err: %s"), str);
// #else
//         tchar sql[STRING_BUFFER_LENGTH] = {0}
//         uinteger l = STRING_BUFFER_LENGTH-1;
//         utils_string_Wide2narrow(str, wcslen(str), sql, l);
//         trace(_T("db err: %s"), sql);
// #endif
//        mSync.unlock();
    }

    //----------------------------------------
    void MysqlDataBase::SetBusyTimeout( s32 nMillisecs )
    {
        mTimeOut = nMillisecs;
        mysql_options(&mMySql, MYSQL_OPT_READ_TIMEOUT, (const char*)&mTimeOut);
        mysql_options(&mMySql, MYSQL_OPT_WRITE_TIMEOUT,(const char *)&mTimeOut);
    }

    //----------------------------------------
    s32 MysqlDataBase::GetBusyTimeout( void ) const
    {
        return mTimeOut;
    }

    //----------------------------------------
    void MysqlDataBase::WaitForEnd( void )
    {

    }

}

#endif // DB_SUPPORT_MYSQL
