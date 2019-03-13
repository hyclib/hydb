#include "../predefine.h"

#include "mysqlDataSet.h"
#include "mysqlDataBase.h"

#ifdef DB_SUPPORT_MYSQL


#define GET_VALUE_KEY(key, outValue)    \
    std::map<db_string, u32>::iterator it = mColMap.find(key);\
    if (it == mColMap.end())    { return false; }\
    return GetValue(it->second, outValue);


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

    void MysqlDataSet::Release( void )
    {
        mysql_free_result(mRes);
        mSync.unlock();
        delete this;
    }

	u64 MysqlDataSet::GetColNum( void )
	{
		return (u64)mColNum;
	}

	bool MysqlDataSet::Read( void )
	{
		mRow = mysql_fetch_row(mRes);
		if (NULL == mRow)
		{
			return false;
		}
		return true;
	}

    //----------------------------------------
    bool MysqlDataSet::GetValue( const tchar* key, bool& outValue )  { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue( const tchar* key, u8& outValue)  { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, u16& outValue)  { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, u32& outValue)    { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, u64& outValue)  { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, s8& outValue)    { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, s16& outValue)   { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, s32& outValue)     { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, s64& outValue){ GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, f32& outValue)   { GET_VALUE_KEY(key, outValue); }
    bool MysqlDataSet::GetValue(const tchar* key, f64& outValue)  { GET_VALUE_KEY(key, outValue); }

    //----------------------------------------
    const void* MysqlDataSet::GetValue_Stream(const tchar* key, s32& dataLen)
    {
        std::map<db_string, u32>::iterator it = mColMap.find(key);
        if (it == mColMap.end())
        {
            return NULL;
        }
        return GetValue_Stream(it->second, dataLen);
    }

    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, bool& outValue)
    {
        if(MYSQL_TYPE_BIT != mColInfo[nCol].valueType)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为bool"));
            return false;
        }
        outValue = mRow[nCol] ? (1 == (mRow[nCol][0])) : false;
        return true;
    }

    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, u8& outValue)
    {
        if(MYSQL_TYPE_TINY != mColInfo[nCol].valueType || !mColInfo[nCol].unsignedFlag)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为u8"));
            return false;
        }
        outValue = mRow[nCol] ? (u8)atoi(mRow[nCol]) : 0;
        return true;
    }

    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, u16& outValue)
    {
        if(MYSQL_TYPE_SHORT != mColInfo[nCol].valueType || !mColInfo[nCol].unsignedFlag)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为u16"));
            return false;
        }
        outValue = mRow[nCol] ? (u16)atoi(mRow[nCol]) : 0;
        return true;
    }

    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, u32& outValue)
    {
        if(MYSQL_TYPE_LONG != mColInfo[nCol].valueType || !mColInfo[nCol].unsignedFlag)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为u32"));
            return false;
        }
        outValue = mRow[nCol] ? DB_StringUtils::StrToU32(mRow[nCol]) : 0;
        return true;
    }

    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, u64& outValue)
    {
        if(MYSQL_TYPE_LONGLONG != mColInfo[nCol].valueType || !mColInfo[nCol].unsignedFlag)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为u64"));
            return false;
        }
        outValue = mRow[nCol] ? DB_StringUtils::StrToU64(mRow[nCol]) : 0;
        return true;
    }

    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, s8& outValue)
    {
        if(MYSQL_TYPE_TINY != mColInfo[nCol].valueType || mColInfo[nCol].unsignedFlag)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为s8"));
            return false;
        }
        outValue = mRow[nCol] ? (s8)atoi(mRow[nCol]) : 0;
        return true;
    }
    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, s16& outValue)
    {
        if(MYSQL_TYPE_SHORT != mColInfo[nCol].valueType || mColInfo[nCol].unsignedFlag)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为s16"));
            return false;
        }
        outValue = mRow[nCol] ? (s16)atoi(mRow[nCol]) : 0;
        return true;
    }
    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, s32& outValue)
    {
        if(MYSQL_TYPE_LONG != mColInfo[nCol].valueType || mColInfo[nCol].unsignedFlag)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为s32"));
            return false;
        }
        outValue = mRow[nCol] ? (s32)atoi(mRow[nCol]) : 0;
        return true;
    }
    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, s64& outValue)
    {
        if(MYSQL_TYPE_LONGLONG != mColInfo[nCol].valueType || mColInfo[nCol].unsignedFlag)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为s64"));
            return false;
        }
        outValue = mRow[nCol] ? DB_StringUtils::StrToS64(mRow[nCol]) : 0;
        return true;
    }
    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, f32& outValue)
    {
        if(MYSQL_TYPE_FLOAT != mColInfo[nCol].valueType)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为f32"));
            return false;
        }
        outValue = mRow[nCol] ? (f32)atof(mRow[nCol]) : 0;
        return true;
    }
    //----------------------------------------
    bool MysqlDataSet::GetValue(const u32& nCol, f64& outValue)
    {
        if(MYSQL_TYPE_DOUBLE != mColInfo[nCol].valueType)
        {
            // 字段类型不为bool
            _pDatabase->SetLastError(_T("字段类型不为f64"));
            return false;
        }
        outValue = mRow[nCol] ? atof(mRow[nCol]) : 0;
        return true;
    }

    //----------------------------------------
    const tchar* MysqlDataSet::GetValue( const tchar* key )
    {
        std::map<db_string, u32>::iterator it = mColMap.find(key);
        if (it == mColMap.end())
        {
            return NULL;
        }
        return GetValue(it->second);
    }

    //----------------------------------------
    const tchar* MysqlDataSet::GetValue( const u32& nCol )
    {
        if(MYSQL_TYPE_VAR_STRING != mColInfo[nCol].valueType && MYSQL_TYPE_STRING != mColInfo[nCol].valueType)
        {
            // 字段类型不为bool
            return NULL;
        }

        if (_pDatabase->StatusUtf8())
        {
            if (DB_CN_IS_UTF8_CSTRING())
                return (tchar*)mRow[nCol];
            else
            {
                const char* p = mRow[nCol];
                if (NULL == p)
                    return NULL;

                unsigned int r = DB_EncodingUtils::UTF8ToStr((utf8*)p, strlen(p), _pDatabase->mTmpBuf, GET_STR_VALUE_TEMP_BUF_LENGTH);
                _pDatabase->mTmpBuf[r] = 0;
                return _pDatabase->mTmpBuf;
            }
        }
        else
        {
            return (tchar*)mRow[nCol];
        }
    }


    //----------------------------------------
    const void* MysqlDataSet::GetValue_Stream(const u32& nCol, s32& dataLen)
    {
        if(MYSQL_TYPE_BLOB != mColInfo[nCol].valueType 
            && MYSQL_TYPE_TINY_BLOB != mColInfo[nCol].valueType 
            && MYSQL_TYPE_MEDIUM_BLOB != mColInfo[nCol].valueType 
            && MYSQL_TYPE_LONG_BLOB != mColInfo[nCol].valueType 
            )
        {
            // 字段类型不为BLOB
            _pDatabase->SetLastError(_T("字段类型不为BLOB"));
            dataLen = 0;
            return NULL;
        }
        unsigned long* FieldLength = mysql_fetch_lengths(mRes);
        dataLen = (s32)FieldLength[nCol];
        return mRow[nCol];
    }

	void MysqlDataSet::_Init( MYSQL_RES* pRes )
	{
		mRes = pRes;
		mRowNum = mysql_num_rows(pRes);
		mColNum = mysql_num_fields(pRes);

		MYSQL_FIELD* fd;
		for (s32 i = 0; i < mColNum; i++)
		{
			fd = mysql_fetch_field(pRes);
			if (NULL == fd)
			{
				return;
			}

#if defined(UNICODE) || defined(_UNICODE)
            COL_INFO info;
            assert(0);
#else
            mColMap[fd->name] = i;

            COL_INFO info;
            info.name = fd->name;
#endif

			info.valueType = fd->type;
            info.unsignedFlag = (0!=(fd->flags & UNSIGNED_FLAG));
			mColInfo.push_back(info);
		}
	}

	u64 MysqlDataSet::GetRowNum( void )
	{
		return (u64)mRowNum;
	}

	DataValueType MysqlDataSet::GetColType( const u32& col )
	{
		return (DataValueType)mColInfo[col].valueType;
	}

	const tchar* MysqlDataSet::GetColName( const u32& col )
	{
		return mColInfo[col].name.c_str();
	}
}

#endif // DB_SUPPORT_MYSQL
