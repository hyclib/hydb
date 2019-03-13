////////////////////////////////////////////////////////
// Copyright(c) 1986-2100, TT, All Rights Reserved
// 作�?  Hu Tianzhi
// 日期: 2011-8-2 16:31:07
// 描述: 
////////////////////////////////////////////////////////

#ifndef	_Tian_201182163107_H_
#define	_Tian_201182163107_H_

#include "../predefine.h"
#include <string>

#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring   db_string;
#else
typedef std::string    db_string;
#endif

#ifdef DB_SUPPORT_MYSQL

namespace hydb
{
    class MysqlDataBase;

	struct COL_INFO
	{
		db_string   name;
		s32			valueType;
        bool        unsignedFlag;
	};

	class MysqlDataSet:public IDataSet
	{
    public:
        MysqlDataSet( MysqlDataBase* pDatabase, DB_SyncObject& sync )
    : _pDatabase(pDatabase)
    , mRes(NULL)
    , mColNum(0)
    , mRow(NULL)
    , mRowNum(0)
    , mSync(sync)
    {}
	public:
		virtual u64     GetColNum(void);
		virtual u64		GetRowNum(void);
		virtual bool    Read(void);

        virtual bool            GetValue(const tchar* key, bool& outValue);
        virtual bool            GetValue(const tchar* key, u8& outValue);
        virtual bool            GetValue(const tchar* key, u16& outValue);
        virtual bool            GetValue(const tchar* key, u32& outValue);
        virtual bool            GetValue(const tchar* key, u64& outValue);
        virtual bool            GetValue(const tchar* key, s8& outValue);
        virtual bool            GetValue(const tchar* key, s16& outValue);
        virtual bool            GetValue(const tchar* key, s32& outValue);
        virtual bool            GetValue(const tchar* key, s64& outValue);
        virtual bool            GetValue(const tchar* key, f32& outValue);
        virtual bool            GetValue(const tchar* key, f64& outValue);
        virtual const tchar*     GetValue(const tchar* key);  // string
        virtual const void*     GetValue_Stream(const tchar* key, s32& dataLen); // binary stream

        virtual bool            GetValue(const u32& nCol, bool& outValue);
        virtual bool            GetValue(const u32& nCol, u8& outValue);
        virtual bool            GetValue(const u32& nCol, u16& outValue);
        virtual bool            GetValue(const u32& nCol, u32& outValue);
        virtual bool            GetValue(const u32& nCol, u64& outValue);
        virtual bool            GetValue(const u32& nCol, s8& outValue);
        virtual bool            GetValue(const u32& nCol, s16& outValue);
        virtual bool            GetValue(const u32& nCol, s32& outValue);
        virtual bool            GetValue(const u32& nCol, s64& outValue);
        virtual bool            GetValue(const u32& nCol, f32& outValue);
        virtual bool            GetValue(const u32& nCol, f64& outValue);
        virtual const tchar*     GetValue(const u32& nCol);
        virtual const void*     GetValue_Stream(const u32& nCol, s32& dataLen); // binary stream
		
        virtual const void*		GetValueAllType(const u32& col){ 
            return mRow[col];
        }
		virtual DataValueType   GetColType(const u32& col);
		virtual const tchar*     GetColName(const u32& col);

		virtual void    Release(void);

		void			_Init(MYSQL_RES* pRes);
	protected:
        MysqlDataBase*      _pDatabase;
		MYSQL_RES*		mRes;
		s32				mColNum;
		MYSQL_ROW		mRow;
		u64			mRowNum;
        std::map<db_string, u32/*index*/>	mColMap;
		std::vector<COL_INFO>		mColInfo;
        DB_SyncObject&      mSync;
	};
}

#endif // DB_SUPPORT_MYSQL
#endif // _Tian_201182163107_H_
