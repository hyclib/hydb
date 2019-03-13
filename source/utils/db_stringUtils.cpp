//
// Copyright(c) 1999-2010 hyrj. All rights reserved.
// hyc libaray
// author:       lipengxiang
// description:  
//

#include "../predefine.h"

#include "db_stringUtils.hpp"
#include <string>
#include <sstream>

#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#pragma warning(disable: 4996)
#endif // CN_PLATFORM == CN_PLATFORM_WINDOWS


namespace hydb {


    //-----------------------------------------------
    s32 DB_StringUtils::StrToS32( const char* pszStr )
    {
        return atoi( pszStr );
    }

    //-----------------------------------------------
    s32 DB_StringUtils::StrToS32( const wchar_t* pszStr )
    {
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
        return _wtoi(pszStr);
#else
        s32 value;
        swscanf(pszStr, L"%d", &value);
        return value;
#endif
    }
    //-----------------------------------------------
    s64 DB_StringUtils::StrToS64( const char* pszStr )
    {
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
        return _atoi64( pszStr );
#else
        s64 value;
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_32
        sscanf(pszStr, "%lld", &value);
#else
        sscanf(pszStr, "%lld", &value);
#endif
        return value;
#endif
    }

    //-----------------------------------------------
    s64 DB_StringUtils::StrToS64( const wchar_t* pszStr )
    {
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
        return _wtoi64( pszStr );
#else
        s64 value;
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_32
        swscanf(pszStr, L"%lld", &value);
#else
        swscanf(pszStr, L"%lld", &value);
#endif
        return value;
#endif
    }
    //-----------------------------------------------
    f64 DB_StringUtils::StrToF64( const char* pszStr )
    {
        return atof( pszStr );
    }

    //-----------------------------------------------
    f64 DB_StringUtils::StrToF64( const wchar_t* pszStr )
    {
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
        return _wtof(pszStr);
#else
        f64 value;
        swscanf(pszStr, L"%lf", &value);
        return value;
#endif
    }

    //-----------------------------------------------
    f32 DB_StringUtils::StrToF32( const char* pszStr )
    {
        return (f32)atof( pszStr );
    }

    //-----------------------------------------------
    f32 DB_StringUtils::StrToF32( const wchar_t* pszStr )
    {
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
        return (f32)_wtof(pszStr);
#else
        f32 value;
        swscanf(pszStr, L"%f", &value);
        return value;
#endif
    }

    //----------------------------------------
    hyc::u32 DB_StringUtils::StrToU32( const char* pszStr )
    {
        u32 value;
        sscanf(pszStr, "%u", &value);
        return value;
//         u32 value;
//         std::stringstream ss(pszStr);
//         ss >> value;
//         return value;
    }

    //----------------------------------------
    hyc::u64 DB_StringUtils::StrToU64( const char* pszStr )
    {
        u64 value;
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_32
        sscanf(pszStr, "%llu", &value);
#else
        sscanf(pszStr, "%llu", &value);
#endif
        return value;
    }

    //----------------------------------------
    hyc::u32 DB_StringUtils::StrToU32( const wchar_t* pszStr )
    {
        u32 value;
        swscanf(pszStr, L"%u", &value);
        return value;
    }

    //----------------------------------------
    hyc::u64 DB_StringUtils::StrToU64( const wchar_t* pszStr )
    {
        u64 value;
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_32
        swscanf(pszStr, L"%llu", &value);
#else
        swscanf(pszStr, L"%llu", &value);
#endif
        return value;
    }
    
    //----------------------------------------
    integer DB_StringUtils::StrToInteger( const wchar_t* pszStr )
    {
        integer value;
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_32
        swscanf(pszStr, L"%ld", &value);
#else
        swscanf(pszStr, L"%lld", &value);
#endif
        return value;
    }
    
    //----------------------------------------
    integer DB_StringUtils::StrToInteger( const char* pszStr )
    {
        integer value;
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_32
        sscanf(pszStr, "%ld", &value);
#else
        sscanf(pszStr, "%lld", &value);
#endif
        return value;
    }
    
    
    //----------------------------------------
    uinteger DB_StringUtils::StrToUInteger( const wchar_t* pszStr )
    {
        uinteger value;
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_32
        swscanf(pszStr, L"%lu", &value);
#else
        swscanf(pszStr, L"%llu", &value);
#endif
        return value;
    }
    
    //----------------------------------------
    uinteger DB_StringUtils::StrToUInteger( const char* pszStr )
    {
        uinteger value;
#if CN_CPU_ARCH == CN_CPU_ARCHITECTURE_32
        sscanf(pszStr, "%lu", &value);
#else
        sscanf(pszStr, "%llu", &value);
#endif
        return value;
    }


} // end namespace hydb
