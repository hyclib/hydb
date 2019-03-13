////////////////////////////////////////////////////////
// Copyright(c) 1986-2100, TT, All Rights Reserved
// Author:  Hu Tianzhi
// Created: 2010-12-22 13:49:20
// Describe:
////////////////////////////////////////////////////////

#ifndef	_Tian_20101222134920_H_
#define	_Tian_20101222134920_H_

#include "../predefine.h"
#include "sqliteDataBase.h"
#include "hy_sqlite3.h"


#ifdef DB_SUPPORT_SQLITE

struct sqlite3;
struct sqlite3_stmt;

namespace hydb
{
    class SqliteDataSet:public IDataSet
    {
    public:
        SqliteDataSet(sqlite3* pDB, sqlite3_stmt* pVM, SqliteDB* pDb, DB_SyncObject& sync);
         ~SqliteDataSet(void);
         virtual void    Release();
    public:
         virtual u64     GetColNum();
         virtual bool    Read();

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

		 virtual const void*	 GetValueAllType(const u32& col) {return NULL;}


         virtual u64		        GetRowNum(void) { return -1; }
         virtual DataValueType	    GetColType(const u32& col) { return DataValueType_NULL; }
         virtual const tchar*		GetColName(const u32& col) { return NULL; }


    public:

        bool    _ColsCheck(u32 nCol);


        s32     _FindCol(const tchar* key);

        SqliteDB*       _pDb;
        sqlite3*        m_pDB;
        sqlite3_stmt*   m_pVM;
        u32             m_nCols;
        DB_SyncObject&     m_sync;
    };
}

#endif // DB_SUPPORT_SQLITE
#endif // _Tian_20101222134920_H_