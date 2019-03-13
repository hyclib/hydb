////////////////////////////////////////////////////////
// Copyright(c) 1986-2100, TT, All Rights Reserved
// Author:  Hu Tianzhi
// Datetime: 2011-8-2 15:35:46
// Description:  1. 无需hyc库支持;
//               2. 最大sql语句长度1024.
//               3. key最大长度 64.
////////////////////////////////////////////////////////

#ifndef	_Tian_201182153546_H_df325fd
#define	_Tian_201182153546_H_df325fd

#include "hyc/_platform.h"

#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#   ifdef HYDB_EXPORTS
#       define HYDB_API __declspec(dllexport)
#   elif  defined(_LIB) && defined(HYC_COMPILE_STATIC_LIB)
#       define HYDB_API
#   else
#       define HYDB_API __declspec(dllimport)
#   endif // HYDB_EXPORTS
#else
#   define  HYDB_API
#endif // CN_PLATFORM == CN_PLATFORM_WINDOWS

//----------------------------------------------------------------------------------------------------
//!	编译选项
//----------------------------------------------------------------------------------------------------

//! 是否支持sqlite
#define DB_SUPPORT_SQLITE

//! 是否支持mysql
#if defined(CN_PLATFORM_MOBILE) || !defined(__i386__) && defined(__arm__)
#else
#define DB_SUPPORT_MYSQL
#endif

#ifndef DB_ELIMINATE

#include "hyc/hycPlatform.h"
#include "hyc/common/cnObjectRef.h"
using namespace hyc;


namespace hydb
{
    class IDataSet;
    class IDataBase;
    class Transaction;

    //----------------------------------------------------------------------------------------------------
    //!	数据库类型
    //----------------------------------------------------------------------------------------------------
    enum DataBaseType
    {
        // mysql 5.0
        DataBaseType_Mysql,
        // sqlite
        DataBaseType_Sqlite,
    };

    //----------------------------------------------------------------------------------------------------
    //! 数据类型
    //----------------------------------------------------------------------------------------------------
    enum DataValueType
    {
        DataValueType_DECIMAL, 
        DataValueType_TINY,
        DataValueType_SHORT,  
        DataValueType_LONG,
        DataValueType_FLOAT,  
        DataValueType_DOUBLE,
        DataValueType_NULL,   
        DataValueType_TIMESTAMP,
        DataValueType_LONGLONG,
        DataValueType_INT24,
        DataValueType_DATE,   
        DataValueType_TIME,
        DataValueType_DATETIME, 
        DataValueType_YEAR,
        DataValueType_NEWDATE, 
        DataValueType_VARCHAR,
        DataValueType_BIT,
        DataValueType_NEWDECIMAL = 246,
        DataValueType_ENUM       = 247,
        DataValueType_SET        = 248,
        DataValueType_TINY_BLOB  = 249,
        DataValueType_MEDIUM_BLOB= 250,
        DataValueType_LONG_BLOB  = 251,
        DataValueType_BLOB       = 252,
        DataValueType_VAR_STRING = 253,
        DataValueType_STRING     = 254,
        DataValueType_GEOMETRY   = 255
    };

    //----------------------------------------------------------------------------------------------------
    //! 二进制块结构
    //----------------------------------------------------------------------------------------------------
	struct BLOB_ITEM
	{
		void* pData;
		s32 dataLen;
	};

    //----------------------------------------------------------------------------------------------------
    //!	数据集接口
    //----------------------------------------------------------------------------------------------------
    class IDataSet
    {
    public:
		//! 使用完成后需释放
        virtual void            Release(void) = 0;
    public:
        virtual u64             GetColNum(void) = 0;
        virtual u64             GetRowNum(void) = 0;    // sqlite no support
        virtual bool            Read(void) = 0;

        virtual bool            GetValue(const tchar* key, bool& outValue) = 0;
        virtual bool            GetValue(const tchar* key, u8& outValue) = 0;
        virtual bool            GetValue(const tchar* key, u16& outValue) = 0;
        virtual bool            GetValue(const tchar* key, u32& outValue) = 0;
        virtual bool            GetValue(const tchar* key, u64& outValue) = 0;
        virtual bool            GetValue(const tchar* key, s8& outValue) = 0;
        virtual bool            GetValue(const tchar* key, s16& outValue) = 0;
        virtual bool            GetValue(const tchar* key, s32& outValue) = 0;
        virtual bool            GetValue(const tchar* key, s64& outValue) = 0;
        virtual bool            GetValue(const tchar* key, f32& outValue) = 0;
        virtual bool            GetValue(const tchar* key, f64& outValue) = 0;
        virtual const tchar*    GetValue(const tchar* key) = 0;  // string (返回的指针为临时指针, 需立即存储, 否者可能造成内存错误; 最长10240).
        virtual const void*     GetValue_Stream(const tchar* key, s32& dataLen) = 0; // binary stream

        virtual bool            GetValue(const u32& nCol, bool& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, u8& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, u16& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, u32& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, u64& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, s8& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, s16& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, s32& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, s64& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, f32& outValue) = 0;
        virtual bool            GetValue(const u32& nCol, f64& outValue) = 0;
        virtual const tchar*    GetValue(const u32& nCol) = 0;  // string (返回的指针为临时指针, 需立即存储, 否者可能造成内存错误; 最长10240).
        virtual const void*     GetValue_Stream(const u32& nCol, s32& dataLen) = 0; // binary stream

        virtual const void*     GetValueAllType(const u32& col) = 0;  //sqlite no support
        virtual DataValueType   GetColType(const u32& col) = 0;       // sqlite no support
        virtual const tchar*    GetColName(const u32& col) = 0;       // sqlite no support
	};

    //----------------------------------------------------------------------------------------------------
    //!	事务对象. 若不提交对象析构后将自动Rollback.
    //----------------------------------------------------------------------------------------------------
    typedef hyc::ObjectRef<Transaction> TransactionRef;
    class HYDB_API Transaction
    {
    public:
        Transaction( IDataBase* pDb );
        ~Transaction();

        // 提交事务之后, 将不能再提交.
        bool Commit();

    protected:
        IDataBase*  _pDB;
    };

    //----------------------------------------------------------------------------------------------------
    //!	数据库接口
    //----------------------------------------------------------------------------------------------------
	class IDataBase
	{
    public:
        //! 使用完成后需释放
        virtual void         Release(void) = 0;
        virtual const tchar* GetLastErrorStr(void) const = 0;
        virtual const tchar* GetLastErrorSql(void) const = 0;

    public:
        virtual bool        StatusUtf8() const = 0;
        virtual s32         StatusPingInterval() const = 0;

	public:
        //! IDataSet指向查询结果的第一行, 使用Read来指向下一行,使用完需释放.
        //! @param pLastInsertId 如果是自增主键,则返回产生的自增主键.
        //! @param pIsSuccess    执行是否成功.
        virtual IDataSet*   ExecQuery(const tchar* strSQL, bool* pIsSuccess = 0) = 0;
        virtual bool        ExecNoneQuery(const tchar* strSQL, u64* pLastInsertId = 0) = 0;
        virtual bool        ExecNoneQuery(const tchar* strSQL, BLOB_ITEM* datas, s32 nCount, u64* pLastInsertId = 0) = 0;

		// 获得或设置超时时间
        virtual void        SetBusyTimeout(s32 nMillisecs) = 0;
		virtual s32         GetBusyTimeout(void)const = 0;

		// Transaction.
        // 返回对象非空即开始事务.
        //  TransactionRef trans = db->BeginTransaction();
        //  if (trans) { /* begin */ }
        //  trans.release(); // end.
        virtual TransactionRef BeginTransaction(void) = 0;

        virtual void        WaitForEnd(void) = 0;   // mySql no need.

    public:
        // 连接发生错误的响应.
        typedef void(*ConnectError)();
        virtual void        setCallback_connectError( ConnectError func ) = 0;

    protected:
        virtual bool        CommitTransaction(void) = 0;
        virtual bool        RollBackTransaction(void) = 0;

        friend class Transaction;
	};

    //----------------------------------------------------------------------------------------------------
    //!	导出接口
    //----------------------------------------------------------------------------------------------------
     
    //!  连接数据库
    //! 失败返回NULL
	extern HYDB_API IDataBase*   ConnectDB(
        DataBaseType dbType,    // 数据库类型
        const tchar* host,      // 服务器名称或IP   (sqlite忽略)
        s32          port,      // 端口             (sqlite忽略)
        const tchar* username,  // 用户名           (sqlite忽略)
        const tchar* password,  // 密码             (sqlite忽略)
        const tchar* dbName,    // 数据库名称
        const tchar* character = 0,     // 字符集编码方式	     (sqlite忽略)
        s32          pingInterval = 10  // 数据库连接ping间隔(s) (sqlite忽略)
        );
}

#endif // DB_ELIMINATE
#endif // _Tian_201182153546_H_df325fd
