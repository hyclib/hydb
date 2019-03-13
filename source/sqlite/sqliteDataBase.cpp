
#include "sqliteDataBase.h"

#ifdef DB_SUPPORT_SQLITE

#include "sqliteDataSet.h"

namespace hydb
{   
    SqliteDB::SqliteDB( void )
        : mConnectErrorFunc(NULL)
    {
        m_pDB = NULL;
    }

    SqliteDB::~SqliteDB( void )
    {

	}

	bool SqliteDB::ExecNoneQuery( const tchar* strSQL, u64* pLastInsertId )
	{
#if defined(UNICODE) || defined(_UNICODE)
        char cStrSql[STRING_BUFFER_LENGTH] = {0};
        uinteger l = STRING_BUFFER_LENGTH;
        if (!utils_string_Wide2narrow(strSQL, wcslen(strSQL), cStrSql, l))
        {
            SqliteDB::SetError(_T("utils_string_Wide2narrow err"));
            return false;
        }

        const char* pStrSql = cStrSql;
#else
        const char* pStrSql = strSQL;
#endif

		char* szError=0;
		try
		{
            mSync.lock();

			s32 nRet = sqlite3_exec(m_pDB, pStrSql, 0, 0, &szError);
            if (pLastInsertId) *pLastInsertId = lastInsertId();
            mSync.unlock();

			if (nRet == SQLITE_OK)
			{
                return true;
			}

            SqliteDB::SetLastErrorSQL(strSQL);
			SqliteDB::SetError(szError);
		}
		catch(...)
		{
            db_log(_T("ExecNoneQuery error"));
			throw ExceptionType_Error;
		}
		return false;
	}

	IDataSet* SqliteDB::ExecQuery( const tchar* strSQL, bool* pIsSuccess )
	{
		try
		{
            mSync.lock();

			sqlite3_stmt* pVM = _compile(strSQL);

			s32 nRet = sqlite3_step(pVM);

			if (nRet == SQLITE_DONE)
			{
				// no rows
				//SqliteDB::SetError("no rows be found", false)a
				sqlite3_finalize(pVM);
                mSync.unlock();

                if (pIsSuccess) *pIsSuccess = true;
				return NULL;
			}
			else if (nRet == SQLITE_ROW)
			{
				// at least 1 row
                if (pIsSuccess) *pIsSuccess = true;
				return new SqliteDataSet(m_pDB, pVM, this, mSync);
			}

			nRet = sqlite3_finalize(pVM);
			const char* szError= sqlite3_errmsg(m_pDB);
            SqliteDB::SetLastErrorSQL(strSQL);
			SqliteDB::SetError(szError);
            mSync.unlock();

            if (pIsSuccess) *pIsSuccess = false;
		}
		catch(...)
		{
            if (pIsSuccess) *pIsSuccess = false;
            return NULL;
		}
		return NULL;

    }

	bool SqliteDB::ExecNoneQuery( const tchar* strSQL, BLOB_ITEM* datas, s32 nCount, u64* pLastInsertId )
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

		tchar* szError = 0;
		const char* szTail = 0;
		sqlite3_stmt* pVM;

        mSync.lock();

        s32 nRet = sqlite3_prepare(m_pDB, pStrSql, -1, &pVM, &szTail);
		if (nRet != SQLITE_OK)
		{
            SqliteDB::SetLastErrorSQL(strSQL);
			SqliteDB::SetError(szError);
            mSync.unlock();

			return false;
		}

		for(s32 i = 0; i < nCount; i++)
		{
			nRet = sqlite3_bind_blob(pVM, i + 1, datas[i].pData, datas[i].dataLen, NULL);  
			if (nRet != SQLITE_OK)  
			{
				sqlite3_finalize(pVM);
                mSync.unlock();

				return false;
			}
		}

		nRet = sqlite3_step(pVM);
		if (nRet != SQLITE_DONE)  
		{  
			sqlite3_finalize(pVM);
            mSync.unlock();

			return false;
		} 
		sqlite3_finalize(pVM);

        if (pLastInsertId) *pLastInsertId = lastInsertId();
        mSync.unlock();

		return true;
	}

    void SqliteDB::SetBusyTimeout( s32 nMillisecs )
    {
        m_nBusyTimeoutMs = nMillisecs;
        mSync.lock();
        sqlite3_busy_timeout(m_pDB, nMillisecs);
        mSync.unlock();
    }

    s32 SqliteDB::GetBusyTimeout() const
    {
        return m_nBusyTimeoutMs;
    }

    void SqliteDB::Release()
    {
        sqlite3_close(m_pDB);
        delete this;
    }

    sqlite3_stmt* SqliteDB::_compile( const tchar* strSql )
    {
#if defined(UNICODE) || defined(_UNICODE)
        char cStrSql[STRING_BUFFER_LENGTH] = {0};
        uinteger l = STRING_BUFFER_LENGTH;
        if (!utils_string_Wide2narrow(strSql, wcslen(strSql), cStrSql, l))
            return NULL;

        const char* pStrSql = cStrSql;
#else
        const char* pStrSql = strSql;
#endif

        tchar* szError=0;
        const char* szTail=0;
        sqlite3_stmt* pVM;

        s32 nRet = sqlite3_prepare(m_pDB, pStrSql, -1, &pVM, &szTail);

        if (nRet != SQLITE_OK)
        {
            SqliteDB::SetError(szError);
        }

        return pVM;
    }

    void SqliteDB::SetError( const char* str , bool bDelete/* = true*/)
    {
        if (!str)
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
//         memcpy(g_strError, str, l);
//         g_strError[l] = 0;
//         trace(_T("db err: %s"), str);
// #endif
//         mSync.unlock();
//        if (bDelete)
//        {
//            sqlite3_free((void*)strError);
//        }        
    }

    //-----------------------------------------------
    void SqliteDB::SetError( const wchar_t* str, bool bDelete /*= true*/ )
    {
        if (!str)
            return;

        db_log(str);

//         mSync.lock();
// #if defined(UNICODE) || defined(_UNICODE)
//         trace(_T("db err: %s"), str);
// #else
//         tchar sql[STRING_BUFFER_LENGTH] = {0};
//         uinteger l = STRING_BUFFER_LENGTH-1;
//         utils_string_Wide2narrow(str, wcslen(str), sql, l);
//         trace(_T("db err: %s"), sql);
// #endif
//         mSync.unlock();

//        if (bDelete)
//        {
//            sqlite3_free((void*)strError);
//        }
    }

    TransactionRef SqliteDB::BeginTransaction()
    {
        char* szError = 0;
        mSync.lock();
        s32 nRet = sqlite3_exec(m_pDB, "begin transaction", 0, 0, &szError);
        if (nRet != SQLITE_OK)
        {
            SqliteDB::SetLastErrorSQL(_T("BeginTransaction"));
            SqliteDB::SetError(szError);
            mSync.unlock();


            return TransactionRef((Transaction*)NULL);
        }

        Transaction* t = new Transaction(this);
        return TransactionRef(t);
    }

    bool SqliteDB::CommitTransaction()
    {
        char* szError = 0;
        s32 nRet = sqlite3_exec(m_pDB, "commit transaction", 0, 0, &szError);

        if (nRet != SQLITE_OK)
        {
            SqliteDB::SetLastErrorSQL(_T("CommitTransaction"));
            SqliteDB::SetError(szError);
            mSync.unlock();

            return false;
        }

        mSync.unlock();
        return true;
    }

    bool SqliteDB::RollBackTransaction()
    {
        char* szError = 0;
        s32 nRet = sqlite3_exec(m_pDB, "rollback transaction", 0, 0, &szError);

        if (nRet != SQLITE_OK)
        {
            SqliteDB::SetLastErrorSQL(_T("RollBackTransaction"));
            SqliteDB::SetError(szError);
            mSync.unlock();

            return false;
        }
        mSync.unlock();

        return true;
    }

	void SqliteDB::WaitForEnd( void )
	{
        mSync.lock();
		wait_for_end(m_pDB);
        mSync.unlock();
	}

    //----------------------------------------
    u64 SqliteDB::lastInsertId()
    {
        //mSync.lock();
        return sqlite3_last_insert_rowid(m_pDB);
        //mSync.unlock();
        //return r;
    }

}

#endif // DB_SUPPORT_SQLITE
