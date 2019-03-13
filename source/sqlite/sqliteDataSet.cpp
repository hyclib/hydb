#include "../predefine.h"
#include "sqliteDataSet.h"

#ifdef DB_SUPPORT_SQLITE

//#include "sqlitedataSet.h"
#include "../../ext/sqlite/sqlite3.h"


#define GET_VALUE_KEY(key, outValue)    \
    s32 nCol = _FindCol(key);\
    if (nCol < 0) { return NULL; }\
    return GetValue(nCol, outValue);

static bool DB_CN_IS_UTF8_CSTRING() {
    static bool unchecked = true;
    static bool isUtf8 = false;
    if (unchecked)
    {
        unchecked = false;
        if (0 == strcmp("严", (char*)"\xe4\xb8\xa5"))
        {
            isUtf8 = true;
        }
    }

    return isUtf8;
}

namespace hydb
{
    SqliteDataSet::SqliteDataSet( sqlite3* pDB, sqlite3_stmt* pVM, SqliteDB* pDb, DB_SyncObject& sync )
        : _pDb(pDb), m_sync(sync)
    {
        m_pDB = pDB;
        m_pVM = pVM;
        m_nCols = sqlite3_column_count(pVM);
    }

    SqliteDataSet::~SqliteDataSet( void )
    {
        sqlite3_finalize(m_pVM);
        m_sync.unlock();
    }

    bool SqliteDataSet::Read()
    {
        s32 nRet = sqlite3_step(m_pVM);

        if (nRet == SQLITE_DONE)
        {
            // no rows
            return false;
        }
        else if (nRet == SQLITE_ROW)
        {
            // more rows
            return true;
        }
        else
        {
            const char* szError = sqlite3_errmsg(m_pDB);
            _pDb->SetError(szError);
            return false;
        }
    }

    u64 SqliteDataSet::GetColNum()
    {
        return (u64)m_nCols;
    }

    //----------------------------------------
    bool SqliteDataSet::GetValue( const tchar* key, bool& outValue )  { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue( const tchar* key, u8& outValue)  { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, u16& outValue)  { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, u32& outValue)    { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, u64& outValue)  { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, s8& outValue)    { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, s16& outValue)   { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, s32& outValue)     { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, s64& outValue){ GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, f32& outValue)   { GET_VALUE_KEY(key, outValue); }
    bool SqliteDataSet::GetValue(const tchar* key, f64& outValue)  { GET_VALUE_KEY(key, outValue); }

	const void* SqliteDataSet::GetValue_Stream( const tchar* key, s32& dataLen )
	{
		s32 nCol = _FindCol(key);
		if (nCol < 0)
		{
			return NULL;
		}
		return GetValue_Stream(nCol, dataLen);
	}

    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, bool& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = (0!=sqlite3_column_int(m_pVM, nCol));
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, u8& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = (u8)sqlite3_column_int(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, u16& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = (u16)sqlite3_column_int(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, u32& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = (u32)sqlite3_column_int(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, u64& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = (u64)sqlite3_column_int64(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, s8& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = (tchar)sqlite3_column_int(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, s16& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = (s16)sqlite3_column_int(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, s32& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = sqlite3_column_int(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, s64& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = sqlite3_column_int64(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, f32& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = (f32)sqlite3_column_double(m_pVM, nCol);
        return true;
    }
    //----------------------------------------
    bool SqliteDataSet::GetValue(const u32& nCol, f64& outValue)
    {
        if(!_ColsCheck(nCol))   { return false; }
        outValue = sqlite3_column_double(m_pVM, nCol);
        return true;
    }

    //----------------------------------------
    const tchar* SqliteDataSet::GetValue( const tchar* key )
    {
        s32 nCol = _FindCol(key);
        if (nCol < 0)
        {
            return NULL;
        }
        return GetValue(nCol);
    }

    //----------------------------------------
    const tchar* SqliteDataSet::GetValue( const u32& nCol )
    {
        if(!_ColsCheck(nCol))   { return NULL; }
        const unsigned char* p = sqlite3_column_text(m_pVM, nCol);
        if (NULL == p)
            return NULL;

        if (_pDb->StatusUtf8())
        {
            if (DB_CN_IS_UTF8_CSTRING())
                return (tchar*)p;
            else
            {
                unsigned int r = DB_EncodingUtils::UTF8ToStr(p, strlen((char*)p), _pDb->mTmpBuf, GET_STR_VALUE_TEMP_BUF_LENGTH);
                _pDb->mTmpBuf[r] = 0;
                return _pDb->mTmpBuf;
            }
        }
        else
        {
            return (tchar*)p;
        }
    }


    //----------------------------------------
    const void* SqliteDataSet::GetValue_Stream(const u32& nCol, s32& dataLen)
    {
        if(!_ColsCheck(nCol))   { dataLen = 0; return NULL; }
        dataLen = sqlite3_column_bytes(m_pVM, nCol);
        return sqlite3_column_blob(m_pVM, nCol);
    }

    void SqliteDataSet::Release()
    {
        delete this;
    }

    bool SqliteDataSet::_ColsCheck( u32 nCol )
    {
        if (nCol >= m_nCols)
        {
            _pDb->SetError("nCol is out of range!", false);
            return false;
        }
        return true;
    }

    s32 SqliteDataSet::_FindCol( const tchar* key )
    {
        if (key)
        {
#if defined(UNICODE) || defined(_UNICODE)
            char str[KEY_BUFFER_LENGTH] = {0};
            uinteger l = KEY_BUFFER_LENGTH;
            if (!utils_string_Wide2narrow(key, wcslen(key), str, l))
                return false;

            const char* pKey = str;
#else
            const char* pKey = key;
#endif

            for (u32 nField = 0; nField < m_nCols; nField++)
            {
                const char* szTemp = sqlite3_column_name(m_pVM, nField);

                if (strcmp(pKey, szTemp) == 0)
                {
                    return nField;
                }
            }
        }
        return -1;
    }	
}

#endif // DB_SUPPORT_SQLITE
